#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include <iostream>
#include <errno.h>

#include "Server.hpp"
#include "tcp_except.hpp"

namespace tcp{
    Server::Server(Server && server){
        ds_.set_fd(server.ds_.get_fd());
        server.ds_.set_fd(-1);
    }

    Server::Server(const std::string & addr, uint16_t port, int max_connection){
        open(addr, port, max_connection);
    }

    void Server::open(const std::string & addr, uint16_t port, int max_connection){
        Descriptor result(::socket(AF_INET, SOCK_STREAM, 0));
        if(!result){
            throw errnoExcept(errno, "socket() error");
        }
        int opt = 1;
        if(::setsockopt(result.get_fd(),
                      SOL_SOCKET,
                      SO_REUSEADDR,
                      &opt,
                      sizeof(opt)) < 0){
            throw errnoExcept(errno, "set SO_REUSEADDR error");
        }
        sockaddr_in sock_addr{};
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = ::htons(port);
        if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
            throw errnoExcept(errno, "inet_aton() error");
        }
        if((::bind(result.get_fd(),
                   reinterpret_cast<sockaddr*>(&sock_addr),
                   sizeof(sock_addr))) == -1){
            throw errnoExcept(errno, "bind() error");
        }
        if(::listen(result.get_fd(), max_connection)){
            throw errnoExcept(errno, "listen() error");
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
            throw errnoExcept(errno, "accept() error");
        }
        return Connection(Descriptor(client_fd));
    }

    void Server::close(){
        ds_.close();
    }

    void Server::set_max_connection(int max_connection){
        check("set_max_connection");
        if(::listen(ds_.get_fd(), max_connection)){
            throw errnoExcept(errno, "set max connection error");
        }
    }

    void Server::set_timeout(size_t ms){
        check("set_timeout");
        timeval timeout{.tv_sec = static_cast<__time_t>(ms) / 1000,
                        .tv_usec = (static_cast<__time_t>(ms) % 1000) * 1000};
        if(::setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_SNDTIMEO,
                      &timeout,
                      sizeof(timeout)) < 0 ||
           ::setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_RCVTIMEO,
                      &timeout,
                      sizeof(timeout)) < 0){
            throw errnoExcept(errno, "set timeout error");
        }
    }
    
    void Server::check(const std::string & where){
        if(ds_.get_fd() == -1){
            throw std::runtime_error("server not initialized (" + where + ")");
        }
    }
}