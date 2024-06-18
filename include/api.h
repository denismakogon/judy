#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif
    int nativeServer(void (*handler)(char* /* data */, long /* bytesReceived*/, 
                                        char* /* host */, int /* port */), 
                                        int port, int bufferSize, int threadCount);
    void boostServer(void (*handler)(struct udp_request* request), 
                     void (*before_handler)(char* /* data */, char* /* errorCategory */, 
                                            int /* errorCode */, long /* bytesReceived */, 
                                            char* /* client */, int /* port */), 
                     void (*after_handler)(char* /* data */, long /* bytesReceived*/, 
                                            char* /* host */, int /* port */), 
                     int port, int bufferSize, int threadCount);
#ifdef __cplusplus
}
#endif
