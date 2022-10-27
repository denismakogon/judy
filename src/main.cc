#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

#include "server.h"

void printRequest(char*) {
    puts("a new packet received!");
    std::flush(std::cout);
}

int main() {
    startServerWithHandlerV2(&printRequest, 20777, std::thread::hardware_concurrency());
}
