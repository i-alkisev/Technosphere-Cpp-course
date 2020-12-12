#include "Service.hpp"
#include <stdexcept>
#include "ErrnoExcept.hpp"
#include <algorithm>

//#include <unistd.h>
#include <iostream>

#define COUNT_BYTES 1024

namespace net{
Service::Service(IServiceListener *listener){
    listener_ = listener;
}

void Service::setListener(IServiceListener *listener){
    listener_ = listener;
}

void Service::open(const std::string & addr, uint16_t port, int max_connection){
    server_ .open(addr, port, max_connection);
    epoll_.add(server_.ds_, EPOLLIN | EPOLLRDHUP);
}

void Service::close(){
    server_.close();
}

//true - if connection was erased
bool Service::checkConnection(int key){
    if(!connections_.at(key).isValid()){
        connections_.erase(key);
        return true;
    }
    return false;
}

void Service::run(size_t maxivents, int timeout_ms){
    if(!listener_) throw std::runtime_error("listener is not valid");
    while(true){
        std::vector<::epoll_event> events_queue = epoll_.wait(maxivents, timeout_ms);
        for(::epoll_event & it : events_queue){
            if(it.data.fd == server_.ds_.get_fd() && it.events & EPOLLIN){
                Connection new_connection(server_.accept());
                int key = new_connection.get_ds().get_fd();
                epoll_.add(new_connection.get_ds(), EPOLLIN | EPOLLOUT | EPOLLRDHUP);
                auto check_size = connections_.size();
                connections_.emplace(std::make_pair(key, std::move(new_connection)));
                if(check_size >= connections_.size()) throw std::runtime_error("Error adding new connection");
                listener_ ->onNewConnection(connections_.at(key));
                checkConnection(key);
            }else{
                int key = it.data.fd;
                bool error = true;
                if(it.events & EPOLLIN){
                    error = false;
                    connections_.at(key).read_to_buf(COUNT_BYTES);
                    listener_->onReadAvailable(connections_.at(key));
                    checkConnection(key);
                }
                if(it.events & EPOLLRDHUP){
                    error = false;
                    listener_->onClose(connections_.at(key));
                    connections_.erase(key);
                }else if(it.events & EPOLLOUT){
                    error = false;
                    connections_.at(key).write_from_buf();
                    if(connections_.at(key).get_write_buf().empty()){
                        listener_->onWriteDone(connections_.at(key));
                        checkConnection(key);
                    }
                }
                if(error){
                    listener_->onError(connections_.at(key));
                    checkConnection(key);
                }
            }
        }
    }
}

} //namespace net