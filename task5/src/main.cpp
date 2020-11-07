#include <iostream>

#include "Service.hpp"
#include "ErrnoExcept.hpp"

struct TestServiceListener : net::IServiceListener
{
    void onNewConnection(net::BufferedConnection & buf_connection, const net::EPoll & epoll) override{
        std::cout << "New client connected" << std::endl;
        buf_connection.write_to_buf("Hello!\n");
        buf_connection.subscribe(epoll, net::EPollEvents::RDONLY);
    }

    void onClose(net::BufferedConnection & buf_connection, const net::EPoll & epoll) override{
        std::cout << "Client closed output or disconnected" << std::endl;
        buf_connection.close();
    }

    void onWriteAvailable(net::BufferedConnection & buf_connection, const net::EPoll & epoll) override{
        buf_connection.write_from_buf(buf_connection.get_write_buf().length());
        buf_connection.subscribe(epoll, net::EPollEvents::RDONLY);
    }

    void onReadAvailable(net::BufferedConnection & buf_connection, const net::EPoll & epoll) override{
        std::string msg = buf_connection.get_read_buf();
        if(msg.back() != '\n') return;
        std::cout << "Client message : \"" << msg.substr(0, msg.length() - 1) << "\"" << std::endl;
        buf_connection.write_to_buf("Your message was: \"" + msg.substr(0, msg.length() - 1) + "\"\n");
        buf_connection.clear_read_buf();
        buf_connection.subscribe(epoll, net::EPollEvents::WRONLY);
    }

    void onError(net::BufferedConnection & buf_connection, const net::EPoll & epoll) override{
        std::cout << "There was a connection error with the client" << std::endl;
    }
};

int main(){
    try{
    TestServiceListener listener;
    net::Service service(&listener);
    service.open("127.0.0.1", 8080, 1024);
    service.run(1024);
    }catch(net::ErrnoExcept & e){
        std::cout << e.get_extended_message() << std::endl;
    }
    return 0;
}