#include "EventSelector.hpp"
#include "FdHandler.hpp"
#include "Log.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <ctime>

#ifndef TIME_DIFF_TO_QUIT
#define TIME_DIFF_TO_QUIT 5
#endif

namespace mch
{

EventSelector::EventSelector(unsigned fdSize)
    : _fdArray(0)
    , _arraySize(fdSize)
    , _maxFd(0)
    , _timeSinceLastSignal(0)
    , _quitFlag(false)
{
    if (fdSize > 0)
        handleAlloc(fdSize);
}

void EventSelector::run()
{
    _quitFlag = false;
    while (!_quitFlag)
    {
        fd_set rds, wrs;
        initFdSets(rds, wrs);
        LOG("About to select... MaxFD %d\n", _maxFd);
        const int res = select(_maxFd + 1, &rds, &wrs, 0, 0);
        LOG("Select returned %d\n", res);
        if (res == -1)
        {
            if (errno == EINTR)
                tryBreakLoop();
            else
                break;
        }
        if (res > 0)
            handleRdWr(rds, wrs);
    }
    printf("Terminating own work...\n");
}

void EventSelector::add(FdHandler &fdh)
{
    const int fd = fdh.getFd();
    if (_arraySize == 0)
        handleAlloc(fd > 15 ? fd + 1 : 16);
    if (_arraySize < fd)
        handleRealloc(fd + 1);

    _fdArray[fd] = &fdh;
    if (fd > _maxFd)
        _maxFd = fd;
}

bool EventSelector::remove(FdHandler &fdh)
{
    const int fd = fdh.getFd();
    if (fd > _arraySize || _fdArray[fd] != &fdh)
        return false;

    _fdArray[fd] = 0;
    if (fd == _maxFd)
    {
        while (_maxFd > 0 && !_fdArray[_maxFd])
            _maxFd--; // Decrement _maxFd until a valid FdHandler* is found
    }
    return true;
}

void EventSelector::tryBreakLoop()
{
    // Raise quite flag if a signal has arrived twice in TIME_DIFF_TO_QUIT secs
    const time_t curTime = time(NULL);
    if (difftime(curTime, _timeSinceLastSignal) <= TIME_DIFF_TO_QUIT)
        _quitFlag = true;
    _timeSinceLastSignal = curTime;
}

void EventSelector::handleAlloc(int size)
{
    assert(_arraySize < size);
    _fdArray = new FdHandler*[size];
    for (int i = 0; i < size; i++)
        _fdArray[i] = 0;
    _arraySize = size;
}

void EventSelector::handleRealloc(int newSize)
{
    assert(_arraySize < newSize);
    FdHandler **tmp = new FdHandler*[newSize];
    for (int i = 0; i < newSize; i++)
        tmp[i] = i < _arraySize ? _fdArray[i] : 0;

    delete[] _fdArray;
    _arraySize = newSize;
    _fdArray = tmp;
}

void EventSelector::initFdSets(fd_set &rds, fd_set &wrs)
{
    FD_ZERO(&rds);
    FD_ZERO(&wrs);
    for (int i = 0; i < _arraySize; i++)
    {
        if (_fdArray[i])
        {
            if (_fdArray[i]->wantRead())
                FD_SET(i, &rds);
            if (_fdArray[i]->wantWrite())
                FD_SET(i, &wrs);
        }
    }
}

void EventSelector::handleRdWr(fd_set &rds, fd_set &wrs)
{
    LOG_E("\n");
    for (int i = 0; i <= _maxFd; i++)
    {
        if (!_fdArray[i])
            continue;
        const bool rd = FD_ISSET(i, &rds);
        const bool wr = FD_ISSET(i, &wrs);
        LOG("Handling fd %d... rd & wr: %d%d\n", i, rd, wr);
        if (rd || wr)
            _fdArray[i]->handle(wr, rd);
    }
    LOG_L("\n");
}

} // namespace mch
