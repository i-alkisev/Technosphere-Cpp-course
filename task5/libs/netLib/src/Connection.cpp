#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include <cerrno>

#include "ErrnoExcept.hpp"
#include "Connection.hpp"

namespace net{
Connection::Connection(Connection && connection) : ds_(std::move(connection.ds_)){}

Connection::Connection(Descriptor && ds) : ds_(std::move(ds)){}

Connection::Connection(const std::string & addr, uint16_t port){
    connect(addr, port);
}

void Connection::connect(const std::string & addr, uint16_t port){
    Descriptor result(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
    if(!result){
        throw ErrnoExcept(errno, "Error creating connection");
    }
    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(port);
    if((::inet_aton(addr.data(), &sock_addr.sin_addr) == 0)){
        throw ErrnoExcept(errno, "Error translating address");
    }
    if((::connect(result.get_fd(),
                    reinterpret_cast<sockaddr*>(&sock_addr),
                    sizeof(sock_addr))) == -1){
        throw ErrnoExcept(errno, "Error establishing connection");
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
        throw ErrnoExcept(errno, "Error read from connection");
    }
    return n;
}

size_t Connection::write(const char* data, size_t len){
    check("write");
    ssize_t n = ::write(ds_.get_fd(), data, len);
    if (n == -1){
        throw ErrnoExcept(errno, "Error write to connection");
    }
    return n;
}

void Connection::check(const std::string & where){
    if(!ds_){
        throw std::runtime_error("Connection not initialized (" + where + ")");
    }
}

const Descriptor & Connection::get_ds() const{
    return ds_;
}

bool Connection::isValid() const{
    return (bool)ds_;
}
}
