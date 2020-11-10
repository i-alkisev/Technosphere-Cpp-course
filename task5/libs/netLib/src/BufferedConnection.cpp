#include "BufferedConnection.hpp"
#include <algorithm>

namespace net{
BufferedConnection::BufferedConnection(Connection && connection){
    connection_ = std::move(connection);
}

void BufferedConnection::subscribe(const EPoll & epoll, EPollEvents flag) const{
    epoll.mod(connection_.get_ds(), flag);
}

void BufferedConnection::read_to_buf(size_t len){
    std::string new_data(len, '0');
    size_t count_bytes = connection_.read(new_data.data(), len);
    new_data.resize(count_bytes);
    read_buf_.append(new_data);
}

void BufferedConnection::write_from_buf(size_t len){
    size_t count_bytes = connection_.write(write_buf_.data(), len);
    std::rotate(write_buf_.begin(), write_buf_.begin() + count_bytes, write_buf_.end());
    write_buf_.resize(write_buf_.length() - count_bytes);
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

void BufferedConnection::clear_read_buf(){
    read_buf_.clear();
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