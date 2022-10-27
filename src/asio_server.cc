#include <iostream>
#include <unistd.h>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

using namespace boost::asio;

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

struct UdpServer {
    explicit UdpServer(ip::udp::socket socket, void (*handler_)(char*), int threadPoolSize)
        : socket_(std::move(socket)), pool(threadPoolSize) {
            this->handler = handler_;
            read();
    }
private:

    void handle_receive(const boost::system::error_code& error, char* data_, std::size_t /*bytes_transferred*/) {
        auto requestUUID = boost::uuids::random_generator()();
        auto uuidString = boost::lexical_cast<std::string>(requestUUID);

        if (error || strcmp(data_, "\n") == 0) {
            std::cerr << currentDateTime() << error.category().name() << ':' << error.value();
            return;
        }
        printf("request ID: %s | from '%s:%d' at %s ", uuidString.c_str(), remote_endpoint_.address().to_string().c_str(), 
            remote_endpoint_.port(), currentDateTime().c_str()
        );
        boost::asio::post(this->pool, boost::bind(this->handler, data_));
        this->read();
    }

    void read() {
        char data_[2048];
        socket_.async_receive_from(
            buffer(data_, 2048), remote_endpoint_,
            boost::bind(
                &UdpServer::handle_receive, this,
                boost::asio::placeholders::error,
                data_,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

    boost::asio::thread_pool pool;
    ip::udp::socket socket_;
    ip::udp::endpoint remote_endpoint_;
    void (*handler)(char*);
};

void startServerWithHandlerV2(void (*handler)(char*), int port, int threadCount) {
    try {
        io_context ctx;
        ip::udp::endpoint endpoint(ip::udp::v4(), port);
        ip::udp::socket socket(ctx, endpoint);
        UdpServer server(std::move(socket), handler, threadCount);
        // ctx.run();
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
