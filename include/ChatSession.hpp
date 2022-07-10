#ifndef CHAT_SESSION_HPP
#define CHAT_SESSION_HPP

#include "FdHandler.hpp"

#include <netinet/in.h>

namespace mch
{
class ChatServer;

class ChatSession : public FdHandler
{
private:
    friend class ChatServer;

private:
    ChatSession(ChatServer *server, int fd, sockaddr_in addr);
    ~ChatSession();

    virtual void handle(bool wr, bool rd);
    void send(const char *str);
    void readAndIgnore();
    void readAndCheck();
    void checkLines();
    void processLine(const char *str);
    bool isAllowedMessages() const { return !_restrictFromMessages; }

    void handleNameInput(const char *str);
    void handleWelcome();
    void handleEntered();
    void handleLeft();

private:
    enum { bufferSize = 1024 };

    ChatServer *_master;
    sockaddr_in _addr;
    char _buffer[bufferSize];
    int _bufferUsed;
    bool _ignore;
    bool _restrictFromMessages;
    char *_username;
    int _usernameLen;
};

} // namespace mch

#endif // CHAT_SESSION_HPP
