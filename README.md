# Multithreaded CPP UDP server consumable through the C ABI consumption

## How to build

```shell
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=debug
cmake --build .
```

## How to use it

```cpp
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
    udpRequest rq = *((udpRequest*)r);
    printf("%s | %s:%d | body length: %d\n", 
            currentDateTime().c_str(), inet_ntoa (rq.clientaddr.sin_addr),
            ntohs (rq.clientaddr.sin_port), rq.bytesReceived
    );
    std::flush(std::cout);

    return NULL;
}

int main(int argc, char const *argv[]) {
    return startServerWithHandler(printRequest, 20777, 2048, 1000);
}
```

see [main.cc](src/main.cc) for more details.
