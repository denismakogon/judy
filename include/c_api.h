#include "structs.h"

extern "C" int nativeServerWithHandler(void* (*handler)(void*), int port, int bufferSize, int threadCount);
extern "C" void boostServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
