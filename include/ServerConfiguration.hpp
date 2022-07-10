#ifndef SERVER_CONFIGURATION_H
#define SERVER_CONFIGURATION_H

namespace mch
{

class ServerConfiguration
{
public:
    typedef int ip_t ;
    typedef short port_t;

public:
    ServerConfiguration(const char *configPath);
    ~ServerConfiguration();
    ip_t getIp() const { return _ip; }
    port_t getPort() const { return _port; }
    bool isCorrect() const { return _correct; }

private:
    char *_configPath;
    ip_t _ip;
    port_t _port;
    bool _correct;
};

} // namespace mch

#endif // SERVER_CONFIGURATION_H
