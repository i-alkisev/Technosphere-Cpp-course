#include <iostream>

#include "Service.hpp"
#include "ErrnoExcept.hpp"

#define REQUEST_SIZE 128

struct TestServiceListener : net::IServiceListener
{
    void onNewConnection(net::BufferedConnection & buf_connection, const net::EPoll & epoll){
        std::cout << "New client connected" << std::endl;
        buf_connection.write_to_buf("Hello!\n");
        buf_connection.subscribe(epoll, EPOLLIN);
    }

    void onClose(net::BufferedConnection & buf_connection, const net::EPoll & epoll){
        std::cout << "Client closed output or disconnected" << std::endl;
        buf_connection.close();
    }

    void onWriteAvailable(net::BufferedConnection & buf_connection, const net::EPoll & epoll){
        buf_connection.write_from_buf(buf_connection.get_write_buf().length());
        buf_connection.subscribe(epoll, EPOLLIN);
    }

    void onReadAvailable(net::BufferedConnection & buf_connection, const net::EPoll & epoll){
        buf_connection.read_to_buf(REQUEST_SIZE);
        std::string msg = buf_connection.get_read_buf();
        std::cout << "Client message : \"" << msg.substr(0, msg.length() - 1) << "\"" << std::endl;
        buf_connection.write_to_buf("Your message was: \"" + msg.substr(0, msg.length() - 1) + "\"\n");
        buf_connection.subscribe(epoll, EPOLLOUT);
    }

    void onError(net::BufferedConnection & buf_connection, const net::EPoll & epoll){
        std::cout << "There was a connection error with the client" << std::endl;
    }
    
    int onTimeout(){
        std::cout << "No events happened" << std::endl;
        std::cout << "Do you want continue? [y/n]" << std::endl;
        std::string ans;
        std::getline(std::cin, ans);
        if (ans[0] == 'n' || ans[0] == 'N') return 1;
        return 0;
    }
};

int main(){
    try{
    net::Service service(std::make_unique<TestServiceListener>());
    service.open("127.0.0.1", 8080, 1024);
    service.run();
    }catch(net::ErrnoExcept & e){
        std::cout << e.get_extended_message() << std::endl;
    }
    return 0;
}