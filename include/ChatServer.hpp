#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "FdHandler.hpp"

namespace mch
{
class ChatSession;
class ConnectionsLogger;
class EventSelector;
class ServerConfiguration;

class ChatServer : public FdHandler
{
public:
    enum { listen_queue_len = 16 };

public:
    ~ChatServer();
    static ChatServer *start(
        EventSelector &eSel, ConnectionsLogger &cLog, ServerConfiguration &cfg);
    void removeSession(ChatSession *sess);
    void sendAll(const char *str, const ChatSession *except = 0);

private:
    ChatServer(EventSelector &eSel, ConnectionsLogger &cLog, int lfd);
    virtual void handle(bool wr, bool rd);

private:
    struct Item
    {
        ChatSession *sess;
        Item *next;
    };

private:
    Item *_sessList;
    ConnectionsLogger &_cLog;
    EventSelector &_eSel;
};

} // namespace mch

#endif // CHAT_SERVER_HPP
