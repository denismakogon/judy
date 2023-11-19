#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif
    int nativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
    void boostServer(void (*handler)(char*, long, char*, int), void (*before_handler)(char*, char*, int, long, char*, int), void (*after_handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
#ifdef __cplusplus
}
#endif
