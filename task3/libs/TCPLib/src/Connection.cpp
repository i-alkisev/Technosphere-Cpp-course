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
#include "Descriptor.hpp"

namespace tcp{
    Connection::Connection(){
        ds_.set_fd(-1);
    }

    Connection::Connection(Connection && connection){
        ds_.set_fd(connection.ds_.get_fd());
        connection.ds_.set_fd(-1);
    }

    Connection::Connection(Descriptor && ds){
        ds_.set_fd(ds.get_fd());
        ds.set_fd(-1);
    }

    Connection::Connection(const std::string & addr, uint16_t port){
        Connect(addr, port);
    }

    void Connection::Connect(const std::string & addr, uint16_t port){
        if(ds_.get_fd() != -1){close();}
        ds_.set_fd(::socket(AF_INET, SOCK_STREAM, 0));
        if(ds_.get_fd() == -1){
            throw std::runtime_error("socket error");
        }
        sockaddr_in sock_addr{};
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = ::htons(port);
        if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
            throw std::runtime_error("the adress is not valid");
        }
        if((::connect(ds_.get_fd(),
                      reinterpret_cast<sockaddr*>(&sock_addr),
                      sizeof(sock_addr))) == -1){
            throw std::runtime_error("connection failed");
        }
    }

    Connection & Connection::operator=(Connection && connection){
        int tmp = connection.ds_.get_fd();
        connection.ds_.set_fd(-1);
        ds_.set_fd(tmp);
        return *this;
    }

    void Connection::close(){
        ds_.close();
    }

    size_t Connection::read(char* data, size_t len){
        check("read");
        ssize_t n = ::read(ds_.get_fd(), data, len);
        if (n == -1){
            throw std::runtime_error{"read error"};
        }
        return n;
    }

    size_t Connection::write(const char* data, size_t len){
        check("write");
        ssize_t n = ::write(ds_.get_fd(), data, len);
        if (n == -1){
            throw std::runtime_error{"write error"};
        }
        return n;
    }

    void Connection::writeExact(const char* data, size_t len){
        check("writeExact");
        size_t n = 0;
        while (n < len){
            n += write(data + n, len - n);
        }
    }

    void Connection::readExact(char* data, size_t len){
        check("readExact");
        size_t n = 0, count_bytes;
        while (n < len){
            count_bytes = read(data + n, len - n);
            n += count_bytes;
            if (!count_bytes){
                ds_.close();
                throw std::runtime_error
                {"could not read the specified number of bytes"};
            }
        }
    }

    void Connection::set_timeout(__time_t ms){
        check("set_timeout");
        timeval timeout{.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
        if(::setsockopt(ds_.get_fd(),
                      SOL_SOCKET,
                      SO_RCVTIMEO,
                      &timeout, sizeof(timeout)) == -1){
            throw std::runtime_error("Set timeout error");
        }
    }
    
    void Connection::check(const std::string & where){
        if(ds_.get_fd() == -1){
            throw std::runtime_error("connection not initialized (" + where + ")");
        }
    }
}
