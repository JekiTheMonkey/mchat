#include "ChatServer.hpp"
#include "ChatSession.hpp"
#include "ConnectionsLogger.hpp"
#include "EventSelector.hpp"
#include "Log.hpp"
#include "ServerConfiguration.hpp"
#include "Utilities.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <errno.h>

namespace mch
{

ChatServer::ChatServer(EventSelector &eSel, ConnectionsLogger &cLog, int lfd)
    : FdHandler(lfd, true)
    , _sessList(0)
    , _cLog(cLog)
    , _eSel(eSel)
{
    eSel.add(*this);
}

ChatServer::~ChatServer()
{
    Item *tmp;
    while (_sessList)
    {
        tmp = _sessList;
        _sessList = _sessList->next;
        delete tmp;
    }
}

int create_socket()
{
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    LOG("Server socket(%d) has been created\n", fd);
    return fd;
}

int allow_reuse_port(int fd)
{
    const int opt = 1;
    const int res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    LOG("Server socket has been allowed to reuse ports\n");
    return res != -1;
}

int bind_socket(int fd, const sockaddr_in &addr)
{
    const int res = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    LOG("Server socket has been binded to %s\n", addrToStr(addr));
    return res != -1;
}

int make_listening(int fd)
{
    const int res = listen(fd, ChatServer::listen_queue_len);
    LOG("Server socket has been made listening\n");
    return res != -1;
}

ChatServer *ChatServer::start(
    EventSelector &eSel, ConnectionsLogger &cLog, ServerConfiguration &cfg)
{
    int fd;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    LOG("(Host based & Integers) IP: %d Port: %d\n",
        cfg.getIp(), cfg.getPort());
    addr.sin_port = htons(cfg.getPort());
    addr.sin_addr.s_addr = htonl(cfg.getIp());

    if ((fd = create_socket()) == 0 ||
        !allow_reuse_port(fd) ||
        !bind_socket(fd, addr) ||
        !make_listening(fd))
        return 0;
    LOG("Server socket has been created successfully\n");
    return new ChatServer(eSel, cLog, fd);
}

void ChatServer::handle(bool wr, bool rd)
{
    UNUSED1(wr);
    if (!rd) // must not happen
        return;
    int fd;
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    LOG("Accepting new client...\n");
    fd = accept(getFd(), (sockaddr*) &addr, &len);
    if (fd == -1)
    {
        PELOG("Failed to accept a client");
        return;
    }

    const char *addrStr = addrToStr(addr);
    _cLog.log("Opened connection - %s\n", addrStr);
    LOG("New client has been accepted. Details: %s\n", addrStr);

    Item *p = new Item;
    p->next = _sessList;
    p->sess = new ChatSession(this, fd, addr);
    _sessList = p;
    _eSel.add(*p->sess);
}

void ChatServer::removeSession(ChatSession *sess)
{
    sockaddr_in addr;
    memcpy(&addr, &sess->_addr, sizeof(addr));
    _eSel.remove(*sess);

    Item **it, *tmp;
    for (it = &_sessList; *it; it = &((*it)->next))
    {
        if ((*it)->sess == sess)
        {
            tmp = *it;
            *it = tmp->next;
            delete tmp->sess;
            delete tmp;
            break;
        }
    }

    const char *addrStr = addrToStr(addr);
    LOG("A session has been removed. Details: %s\n", addrStr);
    _cLog.log("Closed connection - %s\n", addrStr);
}

void ChatServer::sendAll(const char *str, const ChatSession *except)
{
    for (Item *it = _sessList; it; it = it->next)
        if (it->sess != except && it->sess->isAllowedMessages())
            it->sess->send(str);
}

} // namespace mch
