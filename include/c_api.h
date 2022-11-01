#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif
    int nativeServerWithHandler(void* (*handler)(void*), int port, int bufferSize, int threadCount);
    void boostServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
#ifdef __cplusplus
}
#endif
