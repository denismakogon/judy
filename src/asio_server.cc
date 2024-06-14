#include <iostream>
#include <csignal>
#include <unistd.h>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "timestamp.h"
#include "structs.h"

using namespace boost::asio;

namespace {
    std::function<void(int)> shutdownHandler;
    void signal_handler(int signal) { shutdownHandler(signal); }
}

struct UdpServer {
    explicit UdpServer(ip::udp::socket socket, void (*handler_)(struct udp_request* request), void (*before_handler_)(char*, char*, int, long, char*, int), void (*after_handler_)(char*, long, char*, int), int bufferSize, int threadPoolSize)
        : socket_(std::move(socket)), pool(threadPoolSize) {
            this->handler = handler_;
            this->before_handler = before_handler_;
            this->after_handler = after_handler_;
            this->bufferSize = bufferSize;
            this->data_ = new char[bufferSize];
            read();
    }
private:

    void printData(char* data_, std::size_t bytes_transferred) {
        printf("%s [INFO] data: [", currentDateTime().c_str());
        for(auto i = 0; i < bytes_transferred; i++) {
            printf("%d, ", data_[i]);
        }
        printf("]\n");
    }

    void handleRequest(char* data_, std::size_t bytes_transferred, const char* clientAddress, int clientPort) {
        udp_request req = {
            bytes_transferred, data_, (char*) clientAddress, clientPort
        };
        boost::asio::post(this->pool, boost::bind(
            this->handler, &req
        ));

        printf("%s [INFO] passing UDP request to judy.server.udp[%s:%d] foreign handler | payload size: %lu bytes\n",
                currentDateTime().c_str(),
                clientAddress, 
                clientPort,
                bytes_transferred 
        );
        std::flush(std::cout);

        boost::asio::post(this->pool, boost::bind(
            this->after_handler, data_, bytes_transferred,
            (char*) clientAddress,
            clientPort
        ));
    }

    void handle_receive(const boost::system::error_code& error, char* data_, std::size_t bytes_transferred) {
        boost::asio::post(this->pool, boost::bind(
            this->before_handler, 
            data_, (char*) error.category().name(), error.value(),
            bytes_transferred, (char*) remote_endpoint_.address().to_string().c_str(), remote_endpoint_.port()
        ));
        if (error || strcmp(data_, "\n") == 0) {
            printf("%s [ERROR] error occured: %s, %d\n", currentDateTime().c_str(), error.category().name(), error.value());
            printData(data_, bytes_transferred);
            return;
        }
        printf("%s [INFO] a new request to judy.server.udp[%s:%d] received | payload size: %lu bytes\n",
            currentDateTime().c_str(),
            remote_endpoint_.address().to_string().c_str(), 
            remote_endpoint_.port(),
            bytes_transferred 
        );
        std::flush(std::cout);
        boost::asio::post(this->pool, boost::bind(&UdpServer::handleRequest, this, data_, 
            bytes_transferred, remote_endpoint_.address().to_string().c_str(), remote_endpoint_.port()
        ));
        printf("%s [INFO] waiting for a new request\n", currentDateTime().c_str());
        this->read();
    }

    void read() {
        socket_.async_receive_from(
            buffer(data_, this->bufferSize), remote_endpoint_,
            boost::bind(
                &UdpServer::handle_receive, this,
                boost::asio::placeholders::error,
                data_,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    int bufferSize = 2048;
    char* data_;
    boost::asio::thread_pool pool;
    ip::udp::socket socket_;
    ip::udp::endpoint remote_endpoint_;
    void (*handler)(struct udp_request* request);
    void (*before_handler)(char*, char*, int, long, char*, int);
    void (*after_handler)(char*, long, char*, int);
};

void startBoostServer(void (*handler)(struct udp_request* request), 
                      void (*before_handler)(char*, char*, int, long, char*, int),
                      void (*after_handler)(char*, long, char*, int), 
                      int port, int bufferSize, int threadCount) {
    try {
        io_context ctx;
        ip::udp::endpoint endpoint(ip::udp::v4(), port);
        ip::udp::socket socket(ctx, endpoint);
        std::signal(SIGINT, signal_handler);
        shutdownHandler = [&](int signal) {
            printf("%s [INFO] boost.server.udp | shutting down...goodbye!\n", currentDateTime().c_str());
            socket.close();
            exit(0);
        };

        UdpServer server(std::move(socket), handler, before_handler, after_handler, 2048, threadCount);
        std::vector<std::thread> threadGroup;
        for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i) {
            std::thread thread([&ctx]() { ctx.run(); });
            threadGroup.push_back(std::move(thread));
        }
        for (auto & th : threadGroup) {
            if (th.joinable())
                th.join();
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
