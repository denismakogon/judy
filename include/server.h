#ifndef server_h
#define server_h

int startNativeServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
void startBoostServerWithHandler(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
#endif /* server_h */
