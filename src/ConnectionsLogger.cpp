#include "ConnectionsLogger.hpp"
#include "FdHandler.hpp"
#include "Log.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

namespace mch
{

static char logBuffer[16384];

ConnectionsLogger::ConnectionsLogger(const char *filepath)
    : _fd(invalidFd)
{
    int fd = open(filepath, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1)
    {
        PELOG("Failed to open log file");
        return;
    }
    _fd = fd;
}

ConnectionsLogger::~ConnectionsLogger()
{
    if (_fd >= 0)
        close(_fd);
}

void ConnectionsLogger::log(const char *format, ...)
{
    if (_fd == invalidFd)
    {
        LOG("Log action cannot be performed due invalid file descriptor\n");
        return;
    }

    int res;
    va_list vl;
    va_start(vl, format);
    res = vsnprintf(logBuffer, sizeof(logBuffer), format, vl);
    res = write(_fd, logBuffer, res);
    if (res == -1)
    {
        _fd = invalidFd;
        PELOG("Failed to write to log file\n");
    }
    va_end(vl);
}

} // namespace mch
