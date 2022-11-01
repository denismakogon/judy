#ifndef server_h
#define server_h

int startServerWithHandler(void* (*handler)(void*), int port, int bufferSize, int threadCount);
void startServerWithHandlerV2(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
#endif /* server_h */
