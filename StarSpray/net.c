#include <sys/socket.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "net.h"

float ntohf(unsigned int x)
{
    union X { unsigned int i; float f; }; union X xu; xu.i = ntohl(x);
    return xu.f;
}

unsigned int htonf(float x)
{
    union X { unsigned int i; float f; }; union X xu; xu.f = x;
    return htonl(xu.i);
}
