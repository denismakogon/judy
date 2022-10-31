#include "server.h"


extern "C" int nativeServerWithHandler(void* (*handler)(void*), int port, int bufferSize, int threadCount) {
    return startServerWithHandler(handler, port, bufferSize, threadCount);
}

extern "C" void boostServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    startServerWithHandlerV2(handler, port, bufferSize, threadCount);
}
