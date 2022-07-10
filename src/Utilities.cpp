#include "Utilities.hpp"

#include <cstdio>
#include <cstring>

static char addrStr[256];

char *addrToStr(const sockaddr_in &addr)
{
    char *res =
        const_cast<char *>(inet_ntop(AF_INET, &addr, addrStr, sizeof(addrStr)));
    if (!res)
        return res;
    const int len = strlen(res);
    sprintf(addrStr + len, "::%d", ntohs(addr.sin_port));
    return addrStr;
}
