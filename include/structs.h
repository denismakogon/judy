#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>

#ifndef structs_h
#define structs_h

struct udpRequest {
    int des;
    int bytesReceived;
    char data [2048];
    socklen_t addlen;
    struct sockaddr_in clientaddr;
};

struct udp_request {
    size_t bytesReceived;
    char* data;
    char* host;
    int port;
};

#endif /* structs_h */
