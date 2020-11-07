#include "Service.hpp"

#define REQUEST_SIZE 128

namespace net{
Service::Service(IServiceListener *listener){
    listener_ = listener;
}

void Service::setListener(IServiceListener *listener){
    listener_ = listener;
}

void Service::open(const std::string & addr, uint16_t port, int max_connection){
    server_ = Server(addr, port, max_connection);
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
    Connection new_connection;
    while(true){
        std::vector<::epoll_event> events_queue = epoll_.wait(maxivents, timeout_ms);
        for(auto it : events_queue){
            if(it.data.fd == server_.ds_.get_fd() && it.events & EPOLLIN){
                new_connection = server_.accept();
                epoll_.add(new_connection.get_ds(), EPollEvents::RDWR);
                connections_.push_back(BufferedConnection(std::move(new_connection)));
                listener_ ->onNewConnection(connections_.back(), epoll_);
            }else if(it.events & EPOLLRDHUP){
                unsigned pos = find_connection_pos(it.data.fd);
                listener_->onClose(connections_[pos], epoll_);
                connections_.erase(connections_.begin() + pos);
            }else if(it.events & EPOLLOUT){
                unsigned pos = find_connection_pos(it.data.fd);
                listener_->onWriteAvailable(connections_[pos], epoll_);
            }else if(it.events & EPOLLIN){
                unsigned pos = find_connection_pos(it.data.fd);
                connections_[pos].read_to_buf(REQUEST_SIZE);
                listener_->onReadAvailable(connections_[pos], epoll_);
            }else{
                unsigned pos = find_connection_pos(it.data.fd);
                listener_->onError(connections_[pos], epoll_);
            }
        }
    }
}
}