#include "server.h"


extern "C" int nativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    return startNativeServer(handler, port, bufferSize, threadCount);
}

extern "C" void boostServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    startBoostServer(handler, port, bufferSize, threadCount);
}
