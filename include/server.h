#ifndef server_h
#define server_h

int startNativeServer(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
void startBoostServer(void (*handler)(char*, long, char*, int), void (*before_handler)(char*, char*, int, long, char*, int), void (*after_handler)(char*, long, char*, int), int port, int bufferSize, int threadCount);
#endif /* server_h */
