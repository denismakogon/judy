#include "server.h"
#include "structs.h"

extern "C" int nativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    return startNativeServer(handler, port, bufferSize, threadCount);
}

extern "C" void boostServer(void (*handler)(struct udp_request* request), 
                            void (*before_handler)(char* /* data */, char* /* errorCategory */, int /* errorCode */, long /* bytesReceived */, char* /* client */, int /* port */), 
                            void (*after_handler)(char* /* data */, long /* bytesReceived*/, char* /* host */, int /* port */), 
                            int port, int bufferSize, int threadCount) {
    startBoostServer(handler, before_handler, after_handler, port, bufferSize, threadCount);
}
