#include <iostream>
#include <thread>

#include "server.h"

void printRequest(char*, long bytesTransferred, char* host, int port) {
    printf("a new packet from %s:%d of %ld bytes received!\n", host, port, bytesTransferred);
    std::flush(std::cout);
}

int main() {
    startServerWithHandlerV2(&printRequest, 20777, 2048, std::thread::hardware_concurrency());
}
