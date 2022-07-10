#ifndef CONNECTIONS_LOGGER_HPP
#define CONNECTIONS_LOGGER_HPP

namespace mch
{

class ConnectionsLogger
{
public:
    ConnectionsLogger(const char *filepath);
    ~ConnectionsLogger();
    void log(const char *format, ...);

private:
    enum { invalidFd = -1 };
    int _fd;
};

} // namespace mch

#endif // CONNECTIONS_LOGGER_HPP
