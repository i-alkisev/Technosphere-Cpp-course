#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include <iostream>

#include "Connection.hpp"
#include "Server.hpp"

namespace tcp{
    Server::Server(){
        ds_.set_fd(-1);
    }

    Server::Server(Server && server){
        ds_.set_fd(server.ds_.get_fd());
        server.ds_.set_fd(-1);
    }

    Server::Server(const std::string & addr, uint16_t port, int max_connection){
        open(addr, port, max_connection);
    }

    void Server::open(const std::string & addr, uint16_t port, int max_connection){
        if(ds_.get_fd() != -1){close();}
        ds_.set_fd(::socket(AF_INET, SOCK_STREAM, 0));
        if(ds_.get_fd() == -1){
            throw std::runtime_error("server error");
        }
        int opt = 1;
        if(::setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_REUSEADDR,
                      &opt, sizeof(opt)) < 0){
            throw std::runtime_error("Set SO_REUSEADDR error");
        }
        sockaddr_in sock_addr{};
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = ::htons(port);
        if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
            throw std::runtime_error("the adress is not valid");
        }
        if((::bind(ds_.get_fd(),
                   reinterpret_cast<sockaddr*>(&sock_addr),
                   sizeof(sock_addr))) == -1){
            throw std::runtime_error("connection failed");
        }
        if(::listen(ds_.get_fd(), max_connection)){
            throw std::runtime_error("listen error");
        }
    }

    Server & Server::operator=(Server && server){
        int tmp = server.ds_.get_fd();
        server.ds_.set_fd(-1);
        ds_.set_fd(tmp);
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
            throw std::runtime_error("accept error");
        }
        return Connection(Descriptor(client_fd));
    }

    void Server::close(){
        ds_.close();
    }

    void Server::set_max_connection(int max_connection){
        check("set_max_connection");
        if(::listen(ds_.get_fd(), max_connection)){
            throw std::runtime_error("set max connection error");
        }
    }

    void Server::set_timeout(__time_t ms){
        check("set_timeout");
        timeval timeout{.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
        if(::setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_SNDTIMEO,
                      &timeout, sizeof(timeout)) < 0){
            throw std::runtime_error("Set timeout error");
        }
        if(setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_RCVTIMEO,
                      &timeout, sizeof(timeout)) < 0){
            throw std::runtime_error("Set timeout error");
        }
    }
    
    void Server::check(const std::string & where){
        if(ds_.get_fd() == -1){
            throw std::runtime_error("server not initialized (" + where + ")");
        }
    }
}