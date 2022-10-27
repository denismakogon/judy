#include <iostream>
#include <thread>

#include "server.h"

void printRequest(char*, long bytesTransferred) {
    printf("a new packet of %ld bytes received!\n", bytesTransferred);
    std::flush(std::cout);
}

int main() {
    startServerWithHandlerV2(&printRequest, 20777, std::thread::hardware_concurrency());
}
