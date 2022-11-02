#include "server.h"


extern "C" int nativeServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    return startNativeServerWithHandler(handler, port, bufferSize, threadCount);
}

extern "C" void boostServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    startBoostServerWithHandler(handler, port, bufferSize, threadCount);
}
