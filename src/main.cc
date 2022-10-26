#include <iostream>
#include <unistd.h>
#include <cstring>
#include <stdio.h>

#include "server.h"
#include "structs.h"

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void* printRequest(void* r) {
    req rq = *((req*)r);
    printf("%s | %s:%d | body length: %d\n", 
            currentDateTime().c_str(), inet_ntoa (rq.clientaddr.sin_addr),
            ntohs (rq.clientaddr.sin_port), rq.bytesReceived
    );
    std::flush(std::cout);

    return NULL;
}

int main(int argc, char const *argv[]) {
    return start_server_with_handler(printRequest, 20777, 2048, 1000);
}
