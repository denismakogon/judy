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

#include "structs.h"

int fd = 0;

void sig_handler(int signo) {
    if (signo == SIGINT) {
        std::cout << "\t Exiting..." << '\n';
        close(fd);
        exit(1);
    }
}

int start_server_with_handler(void* (*handler)(void*), int port, int bufferSize, int threadCount) {
    pthread_t threads[threadCount];
    int threadno = 0, fd;

    std::cout << "starting UDP server...\n";
    sockaddr_in serveraddr;
    sockaddr_in clientaddr;

    socklen_t addrlen = sizeof(clientaddr);
    int recvlen;
    char buf [bufferSize];

    /* Create socket */
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cout << "Socket creation failed...\n\t Exiting..." << '\n';
        return 0;
    }
    std::cout << "socket allocated...\n";

    memset ((sockaddr*)&serveraddr, 0, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);
    serveraddr.sin_port = htons (port);

    if (bind (fd, (sockaddr*)&serveraddr, sizeof (serveraddr)) == -1) {
        std::cout << "Binding failed... Exiting..." << '\n';
        return 0;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);

    std::cout << "listening on 0.0.0.0:" << port << "...\n";
    while (1) {
        recvlen = recvfrom (fd, buf, bufferSize, 0, (sockaddr*) &clientaddr, &addrlen);
        req *r = new req;
        bzero (r, sizeof (req));
        r->bytesReceived = recvlen;
        r->addlen = addrlen;
        r->clientaddr = clientaddr;
        r->des = fd;
        strcpy (r->data, buf);
        pthread_create(&threads[threadno++], NULL, handler, (void*) r);
        if (threadno == threadCount)
            threadno = 0;
        memset (buf, 0, sizeof (buf));
    }
}
