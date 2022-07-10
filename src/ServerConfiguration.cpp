#include "ServerConfiguration.hpp"
#include "Log.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

namespace mch
{

ServerConfiguration::ip_t retriveIp(const char *buf);
ServerConfiguration::port_t retrivePort(const char *buf, unsigned size);

ServerConfiguration::ServerConfiguration(const char *configPath)
    : _configPath(0)
    , _ip(0)
    , _port(0)
    , _correct(false)
{
    const size_t len = strlen(configPath);
    _configPath = new char[len];
    strcpy(_configPath, configPath);

    int fd = open(_configPath, O_RDONLY);
    if (fd == -1)
        return;

    char buf[128];
    int res = read(fd, buf, sizeof(buf));
    if (res == -1)
        return;
    close(fd);

    ip_t ip = retriveIp(buf);
    if (ip == -1)
    {
        LOG("Failed to retrive IP from config file\n");
        return;
    }
    _ip = ip;

    port_t port = retrivePort(buf, res);
    if (port == -1)
    {
        LOG("Failed to retrive port from config file\n");
        return;
    }
    _port = port;

    _correct = true;
}

ServerConfiguration::~ServerConfiguration()
{
    delete[] _configPath;
}

ServerConfiguration::ip_t retriveIp(const char *buf)
{
    const char *end = strchr(buf, ' ');
    if (!end)
        return -1;

    const size_t len = end - buf;
    char *ipStr = new char[len + 1];
    strncpy(ipStr, buf, len);
    ipStr[len + 1] = '\0';
    LOG("IP: %s\n", ipStr);

    ServerConfiguration::ip_t ip = inet_addr(ipStr);
    LOG("String IP: %s Integer IP: %d\n", ipStr, ip);
    delete[] ipStr;

    return ip;
}

ServerConfiguration::port_t retrivePort(const char *buf, unsigned size)
{
    const char *begin = strchr(buf, ' ');
    if (!begin)
        return -1;
    begin++;

    const size_t len = buf + size - begin - 1;
    char *portStr = new char[len + 1];
    strncpy(portStr, begin, len);
    portStr[len + 1] = '\0';

    ServerConfiguration::port_t port = strtol(portStr, 0, 10);
    LOG("String port: %s Integer port: %d\n", portStr, port);
    delete[] portStr;

    return port;
}

} // namespace mch
