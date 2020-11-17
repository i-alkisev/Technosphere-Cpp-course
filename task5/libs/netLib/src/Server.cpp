#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include <iostream>
#include <errno.h>

#include "Server.hpp"
#include "ErrnoExcept.hpp"

namespace net{
Server::Server(Server && server) : ds_(std::move(server.ds_)){}

Server::Server(const std::string & addr, uint16_t port, int max_connection){
    open(addr, port, max_connection);
}

void Server::open(const std::string & addr, uint16_t port, int max_connection){
    Descriptor result(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
    if(!result){
        throw ErrnoExcept(errno, "Error creating server");
    }
    int opt = 1;
    if(::setsockopt(result.get_fd(),
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &opt,
                    sizeof(opt)) < 0){
        throw ErrnoExcept(errno, "Error seting SO_REUSEADDR");
    }
    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(port);
    if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
        throw ErrnoExcept(errno, "Error translating address");
    }
    if((::bind(result.get_fd(),
                reinterpret_cast<sockaddr*>(&sock_addr),
                sizeof(sock_addr))) == -1){
        throw ErrnoExcept(errno, "Error binding address to server");
    }
    if(::listen(result.get_fd(), max_connection)){
        throw ErrnoExcept(errno, "Error setting listen to server");
    }
    ds_ = std::move(result);
}

Server & Server::operator=(Server && server){
    ds_ = std::move(server.ds_);
    return *this;
}

Connection Server::accept(){
    check("accept");
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    int client_fd = ::accept(ds_.get_fd(),
                                reinterpret_cast<sockaddr*>(&client_addr),
                                &addr_size);
    if(client_fd < 0){
        throw ErrnoExcept(errno, "Error accepting new connection");
    }
    return Connection(Descriptor(client_fd));
}

void Server::close(){
    ds_.close();
}

void Server::check(const std::string & where){
    if(ds_.get_fd() == -1){
        throw std::runtime_error("server not initialized (" + where + ")");
    }
}
}