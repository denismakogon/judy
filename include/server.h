#ifndef server_h
#define server_h

int start_server_with_handler(void* (*handler)(void*), int port, int bufferSize, int threadCount);

#endif /* server_h */
