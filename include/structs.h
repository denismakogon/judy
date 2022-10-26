#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>

#ifndef structs_h
#define structs_h

struct req {
    int des;
    int bytesReceived;
    char data [2048];
    socklen_t addlen;
    struct sockaddr_in clientaddr;
};

#endif /* structs_h */
