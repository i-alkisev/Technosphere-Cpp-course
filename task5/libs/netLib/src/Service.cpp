#include "Service.hpp"
#include <stdexcept>

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
    epoll_.add(server_.ds_, EPollEvents::RDONLY);
}

void Service::close(){
    server_.close();
}

unsigned Service::find_connection_pos(int fd){
    for(unsigned i = 0; i < connections_.size(); ++i){
        if(connections_[i] == fd) return i;
    }
    throw std::runtime_error("Error finding connection position");
}

void Service::run(size_t maxivents, int timeout_ms){
    while(true){
        std::vector<::epoll_event> events_queue = epoll_.wait(maxivents, timeout_ms);
        for(::epoll_event & it : events_queue){
            if(it.data.fd == server_.ds_.get_fd() && it.events & EPOLLIN){
                Connection new_connection(server_.accept());
                epoll_.add(new_connection.get_ds(), EPollEvents::RDWR);
                auto check_size = connections_.size();
                connections_.emplace_back(std::move(new_connection));
                if(check_size >= connections_.size()) throw std::runtime_error("Error adding new connection");
                listener_ ->onNewConnection(connections_.back(), epoll_);
            }else if(it.events & EPOLLRDHUP){
                unsigned pos = find_connection_pos(it.data.fd);
                listener_->onClose(connections_[pos], epoll_);
                connections_.erase(connections_.begin() + pos);
            }else if(it.events & EPOLLOUT){
                unsigned pos = find_connection_pos(it.data.fd);
                size_t buf_len = connections_[pos].get_write_buf().length();
                connections_[pos].write_from_buf(buf_len);
                if(connections_[pos].get_write_buf().empty()) listener_->onWriteDone(connections_[pos], epoll_);
            }else if(it.events & EPOLLIN){
                unsigned pos = find_connection_pos(it.data.fd);
                connections_[pos].read_to_buf(COUNT_BYTES);
                listener_->onReadAvailable(connections_[pos], epoll_);
            }else{
                unsigned pos = find_connection_pos(it.data.fd);
                listener_->onError(connections_[pos], epoll_);
            }
        }
    }
}
}