#include <iostream>
#include <thread>

#include "server.h"
#include "timestamp.h"

void printRequest(char*, long bytesTransferred, char* host, int port) {
    printf("%s [INFO] packet is %ld bytes long\n", currentDateTime().c_str(), bytesTransferred);
    std::flush(std::cout);
}

void beforeRequest(char* /* data */, char* /* errorCategory */, int /*errorCode*/, long bytesTransferred, char* host, int port) {
    printf("%s [INFO] before request executed\n", currentDateTime().c_str());
    printf("%s [INFO] a new packet from %s:%d received\n", currentDateTime().c_str(), host, port);
    std::flush(std::cout);
}

void afterRequest(char* /* data */, long /* bytesTransferred */, char* /* host */, int /* port */) {
    printf("%s [INFO] after request\n", currentDateTime().c_str());
}

int main() {
    startBoostServer(&printRequest, &beforeRequest, &afterRequest, 20777, 2048, std::thread::hardware_concurrency());
}
