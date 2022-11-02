#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <algorithm>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <functional>

#include "structs.h"
#include "timestamp.h"

namespace {
    std::function<void(char*, long, char*, int)> upcallRequestDataHandler;
    void* requestHandler(void* data) { 
        udpRequest rq = *((udpRequest*) data);
        printf("%s INFO   boost.server.udp | payload size: %d bytes | from '%s:%d'\n",
            currentDateTime().c_str(),
            rq.bytesReceived,
            inet_ntoa(rq.clientaddr.sin_addr),
            ntohs(rq.clientaddr.sin_port)
        );
        upcallRequestDataHandler(rq.data, rq.bytesReceived, inet_ntoa(rq.clientaddr.sin_addr), ntohs(rq.clientaddr.sin_port));
        return NULL;
    }

    std::function<void(int)> shutdownHandler;
    void signal_handler(int signal) { shutdownHandler(signal); }
}

int startNativeServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    pthread_t threads[threadCount];
    int threadno = 0, fd = 0;

    printf("%s INFO   native.server.udp | starting native UDP server", currentDateTime().c_str());
    sockaddr_in serveraddr;
    sockaddr_in clientaddr;

    socklen_t addrlen = sizeof(clientaddr);
    int recvlen;
    char buf [bufferSize];

    /* Create socket */
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "socket creation failed, exiting..." << std::endl;
        return 0;
    }
    printf("%s INFO   native.server.udp | socket allocated\n", currentDateTime().c_str());

    memset ((sockaddr*)&serveraddr, 0, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);
    serveraddr.sin_port = htons (port);

    if (bind (fd, (sockaddr*)&serveraddr, sizeof (serveraddr)) == -1) {
        std::cerr << "Binding failed... exiting..." << std::endl;
        return 0;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    shutdownHandler = [&](int signo) {
        printf("%s INFO   native.server.udp | shutting down...\n", currentDateTime().c_str());
        close(fd);
        exit(signo);
    };

    upcallRequestDataHandler = [&](char* data, long bytesReceived, char* clientAddress, int port) {
        handler(data, bytesReceived, clientAddress, port);
    };

    printf("%s INFO   native.server.udp | listening on 0.0.0.0:%d\n", currentDateTime().c_str(), port);
    while (1) {
        recvlen = recvfrom (fd, buf, bufferSize, 0, (sockaddr*) &clientaddr, &addrlen);
        udpRequest *req = new udpRequest;
        bzero (req, sizeof (udpRequest));
        req->bytesReceived = recvlen;
        req->addlen = addrlen;
        req->clientaddr = clientaddr;
        req->des = fd;
        strcpy (req->data, buf);
        pthread_create(&threads[threadno++], NULL, requestHandler, (void*) req);
        if (threadno == threadCount)
            threadno = 0;
        memset (buf, 0, sizeof (buf));
    }
}
