#include "BufferedConnection.hpp"
#include <algorithm>
#include "ErrnoExcept.hpp"
#include <errno.h>

#include <iostream>

namespace net{
BufferedConnection::BufferedConnection(Connection && connection){
    connection_ = std::move(connection);
}

const Descriptor & BufferedConnection::get_ds() const{
    return connection_.get_ds();
}

void BufferedConnection::subscribe(const EPoll & epoll) const{
    epoll.mod(connection_.get_ds(), events_);
}

void BufferedConnection::update_events(EPollEvents flag){
    if (flag == EPollEvents::RDONLY)
        events_ = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLET;
    else if (flag == EPollEvents::WRONLY)
        events_ = EPOLLOUT | EPOLLRDHUP | EPOLLONESHOT | EPOLLET;
    else if (flag == EPollEvents::RDWR)
        events_ = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLONESHOT | EPOLLET;
    else
        events_ = EPOLLRDHUP | EPOLLONESHOT | EPOLLET;
}

void BufferedConnection::set_keep_alive(bool val){
    keep_alive_ = val;
}

bool BufferedConnection::get_keep_alive() const{
    return keep_alive_;
}

void BufferedConnection::read_to_buf(size_t len){
    std::string new_data(len, '0');
    size_t count_bytes = connection_.read(new_data.data(), len);
    new_data.resize(count_bytes);
    read_buf_.append(new_data);
}

void BufferedConnection::write_from_buf(){
    size_t count_bytes = connection_.write(write_buf_.data(), write_buf_.length());
    std::rotate(write_buf_.begin(), write_buf_.begin() + count_bytes, write_buf_.end());
    write_buf_.resize(write_buf_.length() - count_bytes);
}

void BufferedConnection::write_to_buf(const std::string & str){
    write_buf_ += str;
}

std::string & BufferedConnection::get_read_buf(){
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

bool BufferedConnection::isValid() const{
    return connection_.isValid();
}
}