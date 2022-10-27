#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

#include "server.h"

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void printRequest(char*) {
    puts(currentDateTime().c_str());
    std::flush(std::cout);
}

int main() {
    startServerWithHandlerV2(&printRequest, 20777, std::thread::hardware_concurrency());
}
