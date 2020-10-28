#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include <iostream>
#include <errno.h>

#include "tcp_except.hpp"
#include "Connection.hpp"

namespace tcp{
    Connection::Connection(Connection && connection) : ds_(std::move(connection.ds_)){}

    Connection::Connection(Descriptor && ds) : ds_(std::move(ds)){}

    Connection::Connection(const std::string & addr, uint16_t port){
        Connect(addr, port);
    }

    void Connection::Connect(const std::string & addr, uint16_t port){
        Descriptor result(::socket(AF_INET, SOCK_STREAM, 0));
        if(!result){
            throw errnoExcept(errno, "socket() error");
        }
        sockaddr_in sock_addr{};
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = ::htons(port);
        if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
            throw errnoExcept(errno, "inet_aton() error");
        }
        if((::connect(result.get_fd(),
                      reinterpret_cast<sockaddr*>(&sock_addr),
                      sizeof(sock_addr))) == -1){
            throw errnoExcept(errno, "connection was not established");
        }
        ds_ = std::move(result);
    }

    Connection & Connection::operator=(Connection && connection){
        ds_ = std::move(connection.ds_);
        return *this;
    }

    void Connection::close(){
        ds_.close();
    }

    size_t Connection::read(char* data, size_t len){
        check("read");
        ssize_t n = ::read(ds_.get_fd(), data, len);
        if (n == -1){
            throw errnoExcept(errno, "read error");
        }
        return n;
    }

    size_t Connection::write(const char* data, size_t len){
        check("write");
        ssize_t n = ::write(ds_.get_fd(), data, len);
        if (n == -1){
            throw errnoExcept(errno, "write error");
        }
        return n;
    }

    void Connection::writeExact(const char* data, size_t len){
        check("writeExact");
        size_t n = 0, count_bytes;
        while (n < len){
            signal(SIGPIPE, SIG_IGN);
            count_bytes = write(data + n, len - n);
            n += count_bytes;
            if (!count_bytes){
                throw errnoExcept
                (errno, "could not write the specified number of bytes");
            }
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
                throw errnoExcept
                (errno, "could not read the specified number of bytes");
            }
        }
    }

    void Connection::set_timeout(size_t ms){
        check("set_timeout");
        timeval timeout{.tv_sec = static_cast<__time_t>(ms) / 1000,
                        .tv_usec = (static_cast<__time_t>(ms) % 1000) * 1000};
        if((::setsockopt(ds_.get_fd(),
                         SOL_SOCKET,
                         SO_RCVTIMEO,
                         &timeout,
                         sizeof(timeout)) == -1) ||
            (::setsockopt(ds_.get_fd(),
                         SOL_SOCKET,
                         SO_SNDTIMEO,
                         &timeout,
                         sizeof(timeout)) == -1)){
            throw errnoExcept(errno, "set timeout error");
        }
    }
    
    void Connection::check(const std::string & where){
        if(!ds_){
            throw std::runtime_error("connection not initialized (" + where + ")");
        }
    }
}
