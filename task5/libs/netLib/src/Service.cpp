#include "Service.hpp"

namespace net{
Service::Service(std::unique_ptr<IServiceListener> listener){
    listener_ = std::move(listener);
}

void Service::setListener(std::unique_ptr<IServiceListener> listener){
    listener_ = std::move(listener);
}

void Service::open(const std::string & addr, uint16_t port, int max_connection){
    server_ = Server(addr, port, max_connection);
    epoll_.add(server_.ds_);
}

void Service::close(){
    server_.close();
}

int Service::find_connection_pos(int fd){
    for(unsigned i = 0; i < connections_.size(); ++i){
        if(connections_[i] == fd) return i;
    }
    return -1;
}

void Service::run(int timeout_ms, int maxivents){
    Connection new_connection;
    while(true){
        std::vector<::epoll_event> events_queue = epoll_.wait(maxivents, timeout_ms);
        if(events_queue.size() == 0){
            if(listener_->onTimeout()) break;
        }
        for(auto it : events_queue){
            if(it.data.fd == server_.ds_.get_fd() && it.events & EPOLLIN){
                new_connection = std::move(server_.accept());
                epoll_.add(new_connection.get_ds());
                connections_.push_back(BufferedConnection(std::move(new_connection)));
                listener_ ->onNewConnection(connections_[connections_.size() - 1], epoll_);
            }else if(it.events & EPOLLRDHUP){
                int pos = find_connection_pos(it.data.fd);
                listener_->onClose(connections_[pos], epoll_);
                connections_.erase(connections_.begin() + pos);
            }else if(it.events & EPOLLOUT){
                int pos = find_connection_pos(it.data.fd);
                listener_->onWriteAvailable(connections_[pos], epoll_);
            }else if(it.events & EPOLLIN){
                int pos = find_connection_pos(it.data.fd);
                listener_->onReadAvailable(connections_[pos], epoll_);
            }else{
                int pos = find_connection_pos(it.data.fd);
                listener_->onError(connections_[pos], epoll_);
            }
        }
    }
}
}