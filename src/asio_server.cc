#include <iostream>
#include <csignal>
#include <unistd.h>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

namespace {
std::function<void(int)> shutdownHandler;
void signal_handler(int signal) { shutdownHandler(signal); }
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

struct UdpServer {
    explicit UdpServer(ip::udp::socket socket, void (*handler_)(char*, long, char*, int), int bufferSize, int threadPoolSize)
        : socket_(std::move(socket)), pool(threadPoolSize) {
            this->handler = handler_;
            this->bufferSize = bufferSize;
            this->data_ = new char[bufferSize];
            read();
    }
private:

    void printData(char* data_, std::size_t bytes_transferred) {
        printf("data: [");
        for(auto i = 0; i < bytes_transferred; i++) {
            printf("%d, ", data_[i]);
        }
        printf("]\n");
    }

    void handleRequest(char* data_, std::size_t bytes_transferred, const char* clientAddress, int clientPort) {
        boost::asio::post(this->pool, boost::bind(
            this->handler, data_, bytes_transferred,
            (char*) clientAddress,
            clientPort
        ));

        printf("%s INFO   boost.server.udp | payload size: %lu bytes | from '%s:%d'\n",
                currentDateTime().c_str(), 
                bytes_transferred, 
                clientAddress, 
                clientPort
        );
        std::flush(std::cout);
    }

    void handle_receive(const boost::system::error_code& error, char* data_, std::size_t bytes_transferred) {
        if (error || strcmp(data_, "\n") == 0) {
            printData(data_, bytes_transferred);
            std::cerr << currentDateTime() << error.category().name() << ':' << error.value();
            return;
        }
        boost::asio::post(this->pool, boost::bind(&UdpServer::handleRequest, this, data_, 
            bytes_transferred, remote_endpoint_.address().to_string().c_str(), remote_endpoint_.port()
        ));
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
    void (*handler)(char*, long, char*, int);
};

void startServerWithHandlerV2(void (*handler)(char*, long, char*, int), int port, int bufferSize, int threadCount) {
    try {
        io_context ctx;
        ip::udp::endpoint endpoint(ip::udp::v4(), port);
        ip::udp::socket socket(ctx, endpoint);
        std::signal(SIGINT, signal_handler);
        shutdownHandler = [&](int signal) {
            printf("%s INFO   boost.server.udp | shutting down...goodbye!\n", currentDateTime().c_str());
            socket.close();
            exit(0);
        };

        UdpServer server(std::move(socket), handler, 2048, threadCount);
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
