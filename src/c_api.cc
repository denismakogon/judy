#include "server.h"


extern "C" int nativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    return startNativeServer(handler, port, bufferSize, threadCount);
}

extern "C" void boostServer(void (*handler)(char*, long, char*, int), void (*before_handler)(char*, char*, int, long, char*, int), void (*after_handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    startBoostServer(handler, before_handler, after_handler, port, bufferSize, threadCount);
}
