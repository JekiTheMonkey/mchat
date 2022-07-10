#include "ChatServer.hpp"
#include "ConnectionsLogger.hpp"
#include "EventSelector.hpp"
#include "ServerConfiguration.hpp"

#include <csignal>
#include <cstdio>

#ifndef CFG_NAME
#define CFG_NAME "mch-config"
#endif

#ifndef LOG_NAME
#define LOG_NAME "mch-connections.log"
#endif

void sigint_handler(int sig_no)
{
    (void) sig_no; // unused
    signal(SIGINT, &sigint_handler);
    // disallow OS from stopping the program with SIGINT,
    // make the program itself able to react to it and shut down as it wishes
}

int main()
{
    signal(SIGINT, &sigint_handler);
    mch::ServerConfiguration cfg(CFG_NAME);
    if (!cfg.isCorrect())
    {
        perror("Failed to read the config \"" CFG_NAME "\"");
        return 1;
    }
    mch::EventSelector eSel;
    mch::ConnectionsLogger cLog(LOG_NAME);
    mch::ChatServer *server = mch::ChatServer::start(eSel, cLog, cfg);
    if (!server)
    {
        perror("Failed to start the server");
        return 1;
    }
    eSel.run();
    return 0;
}
