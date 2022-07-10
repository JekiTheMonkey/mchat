#ifndef EVENT_SELECTOR_HPP
#define EVENT_SELECTOR_HPP


#include <sys/select.h>
namespace mch
{

class FdHandler;

class EventSelector
{
public:
    EventSelector(unsigned fdSize = 0);
    void run();
    void add(FdHandler &fdh);
    bool remove(FdHandler &fdh);

    void tryBreakLoop();

private:
    void handleAlloc(int size);
    void handleRealloc(int newSize);
    void initFdSets(fd_set &rds, fd_set &wrs);
    void handleRdWr(fd_set &rds, fd_set &wrs);

private:
    FdHandler **_fdArray;
    int _arraySize;
    int _maxFd;
    time_t _timeSinceLastSignal;
    bool _quitFlag;
};

} // namespace mch

#endif // EVENT_SELECTOR_HPP
