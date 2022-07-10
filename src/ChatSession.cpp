#include "ChatSession.hpp"
#include "ChatServer.hpp"
#include "Log.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

static const char welcome_msg[] = "Welcome to the chat, you are known as ";
static const char entered_msg[] = " has entered the chat";
static const char left_msg[] = " has left the chat";

namespace mch
{

ChatSession::ChatSession(ChatServer *server, int fd, sockaddr_in addr)
    : FdHandler(fd, true)
    , _master(server)
    , _addr(addr)
    , _bufferUsed(0)
    , _ignore(false)
    , _restrictFromMessages(true)
    , _username(0)
    , _usernameLen(0)
{
    LOG("New session has been created\n");
    send("What's your name? ");
}

ChatSession::~ChatSession()
{
    LOG("A session has been removed\n");
    if (_username)
        delete[] _username;
}

void ChatSession::handle(bool wr, bool rd)
{
    LOG_E("\n");
    UNUSED1(wr);
    if (!rd) // must not happen
    {
        LOG_L("\n");
        return;
    }
    if (_bufferUsed > static_cast<int>(sizeof(_buffer)))
    {
        _bufferUsed = 0;
        _ignore = true;
    }
    if (_ignore)
        readAndIgnore();
    else
        readAndCheck();
    LOG_L("\n");
}

void ChatSession::send(const char *str)
{
    LOG("Sending a string to '%s'...\n", _username);
    write(getFd(), str, strlen(str));
}

void ChatSession::readAndIgnore()
{
    const int res = read(getFd(), _buffer, sizeof(_buffer));
    if (res < 1)
    {
        _master->removeSession(this);
        return;
    }
    const char *lf = static_cast<const char*>(memchr(_buffer, '\n', res));
    if (!lf)
        return;
    const int rem = res - int(lf - _buffer) - 1;
    _bufferUsed = rem;
    memmove(_buffer, _buffer + res - rem, rem);
    _ignore = false;
    LOG("Read bytes: %d Remaining bytes: %d\n", res, rem);
    checkLines();
}

void ChatSession::readAndCheck()
{
    const int res =
        read(getFd(), _buffer + _bufferUsed, sizeof(_buffer) - _bufferUsed);
    if (res < 1)
    {
        if (_username)
            handleLeft();
        _master->removeSession(this);
        return;
    }
    _bufferUsed += res;
    LOG("Read bytes: %d Used bytes: %d\n", res, _bufferUsed);
    checkLines();
}

void ChatSession::checkLines()
{
    if (_bufferUsed <= 0)
        return;
    LOG_E("\n");
    for (int i = 0; i < _bufferUsed; i++)
    {
        if (_buffer[i] == '\n')
        {
            _buffer[i] = 0;
            if (i > 0 && _buffer[i - 1] == '\r')
                _buffer[i - 1] = 0;
            processLine(_buffer);
            const int rest = _bufferUsed - i - 1;
            memmove(_buffer, _buffer + i + 1, rest);
            _bufferUsed = rest;
            checkLines();
            LOG_L("\n");
            return;
        }
    }
    LOG_L("\n");
}

void ChatSession::processLine(const char *str)
{
    if (!_username)
    {
        handleNameInput(str);
        return;
    }
    LOG_E("\n");
    const int len = strlen(str);
    char *msg = new char[len + _usernameLen + 5];
    sprintf(msg, "<%s> %s\n", _username, str);
    _master->sendAll(msg, this);
    delete[] msg;
    LOG_L("\n");
}

void ChatSession::handleNameInput(const char *str)
{
    assert(!_username);

    const int len = strlen(str);
    char *username = new char[len + 1];
    memcpy(username, str, len);
    username[len] = '\0';
    _username = username;
    _usernameLen = len;
    _restrictFromMessages = false;
    LOG("New user '%s'\n", username);

    handleWelcome();
    handleEntered();
}

void ChatSession::handleWelcome()
{
    assert(_username);
    char *wmsg = new char[_usernameLen + sizeof(welcome_msg) + 2];
    sprintf(wmsg, "%s%s\n", welcome_msg, _username);
    send(wmsg);
    delete[] wmsg;
}

void ChatSession::handleEntered()
{
    assert(_username);
    char *emsg = new char[_usernameLen + sizeof(entered_msg) + 2];
    sprintf(emsg, "%s%s\n", _username, entered_msg);
    _master->sendAll(emsg, this);
    delete[] emsg;
}

void ChatSession::handleLeft()
{
    assert(_username);
    char *lmsg = new char[_usernameLen + sizeof(left_msg) + 2];
    sprintf(lmsg, "%s%s\n", _username, left_msg);
    _master->sendAll(lmsg, this);
    delete[] lmsg;
}

} // namespace mch
