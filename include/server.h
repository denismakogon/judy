#ifndef server_h
#define server_h

#include "structs.h"

int startNativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
void startBoostServer(void (*handler)(struct udp_request* request), 
                      void (*before_handler)(char* /* data */, char* /* errorCategory */, int /* errorCode */, long /* bytesReceived */, char* /* client */, int /* port */), 
                      void (*after_handler)(char* /* data */, long /* bytesReceived*/, char* /* host */, int /* port */), 
                      int port, int bufferSize, int threadCount);
#endif /* server_h */
