#include "BufferedConnection.hpp"

namespace net{
BufferedConnection::BufferedConnection(Connection && connection){
    connection_ = std::move(connection);
}

void BufferedConnection::subscribe(const EPoll & epoll, int flag) const{
    epoll.mod(connection_.get_ds(), flag);
}

void BufferedConnection::read_to_buf(size_t len){
    read_buf_ = std::string(len + 1, '\0');
    size_t count_bytes = connection_.read(read_buf_.data(), len);
    read_buf_.resize(count_bytes);
}

void BufferedConnection::write_from_buf(size_t len){
    size_t count_bytes = connection_.write(write_buf_.data(), len);
    write_buf_ = write_buf_.substr(count_bytes, write_buf_.length());
}

void BufferedConnection::write_to_buf(const std::string & str){
    write_buf_ += str;
}

const std::string & BufferedConnection::get_read_buf(){
    return read_buf_;
}

std::string & BufferedConnection::get_write_buf(){
    return write_buf_;
}

void BufferedConnection::close(){
    connection_.close();
}

bool BufferedConnection::operator==(int fd){
    return connection_.get_ds().get_fd() == fd;
}

BufferedConnection::BufferedConnection(BufferedConnection && buf_connection){
    read_buf_ = std::move(buf_connection.read_buf_);
    write_buf_ = std::move(buf_connection.write_buf_);
    connection_ = std::move(buf_connection.connection_);
}
BufferedConnection & BufferedConnection::operator=(BufferedConnection && buf_connection){
    read_buf_ = std::move(buf_connection.read_buf_);
    write_buf_ = std::move(buf_connection.write_buf_);
    connection_ = std::move(buf_connection.connection_);
    return *this;
}
}