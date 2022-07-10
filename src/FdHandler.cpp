#include "FdHandler.hpp"

#include <unistd.h>

namespace mch
{

FdHandler::FdHandler(int fd, bool ownership)
    : _fd(fd)
    , _ownership(ownership)
{
}

FdHandler::~FdHandler()
{
    if (_ownership)
        close(_fd);
}

} // namespace mch
