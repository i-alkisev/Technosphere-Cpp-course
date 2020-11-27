#include "HttpService.hpp"
#include <stdexcept>
#include <thread>
#include "ErrnoExcept.hpp"
#include "HttpService.hpp"
#include <functional>
#include <algorithm>
#include <mutex>

#include <iostream>

#define COUNT_BYTES 1024

namespace http{
bool GET_request::to_request(std::string & str){
    size_t begin = str.find("GET");
    if(begin == str.npos) return false;
    size_t end = str.find("\r\n\r\n", begin);
    if(end == str.npos) return false;
    data_.clear();
    size_t curr_begin = begin, curr_end = str.find("\r\n", begin);
    data_["Header"] = std::string(str.begin() + curr_begin, str.begin() + curr_end);
    while((curr_end += 2) < end){
        curr_begin = curr_end;
        curr_end = str.find(": ", curr_begin);
        if(curr_end == str.npos || curr_end > end){
            data_.clear();
            return false;
        }
        std::string key(str.begin() + curr_begin, str.begin() + curr_end);
        curr_end += 2;
        curr_begin = curr_end;
        curr_end = str.find("\r\n", curr_begin);
        std::string value(str.begin() + curr_begin, str.begin() + curr_end);
        data_[key] = value;
    }
    size_t req_len = end - begin + 4;
    std::rotate(str.begin(), str.begin() + req_len + begin, str.end());
    str.resize(str.length() - req_len - begin);
    return true;
}

std::string GET_request::get_value_of(const std::string & key) const{
    if(data_.empty() || !data_.count(key)) return "";
    return data_.at(key);
}

std::map<std::string, std::string>::iterator GET_request::begin(){
    return data_.begin();
}

std::map<std::string, std::string>::iterator GET_request::end(){
    return data_.end();
}

std::map<std::string, std::string>::const_iterator GET_request::begin() const{
    return data_.begin();
}

std::map<std::string, std::string>::const_iterator GET_request::end() const{
    return data_.end();
}

namespace{
std::mutex cerr_mutex;
}

void worker(HttpService & s, size_t maxivents, int timeout_ms){
    try{
    while(true){
        std::vector<::epoll_event> events_queue = s.epoll_.wait(maxivents, timeout_ms);
        for(::epoll_event & it : events_queue){
            bool error = true;
            if(it.data.fd == s.server_.get_ds().get_fd() && it.events & EPOLLIN){
                try{
                    while(true){
                        net::Connection new_connection(s.server_.accept());
                        s.epoll_.add(new_connection.get_ds(), 0);
                        int key = new_connection.get_ds().get_fd();
                        s.connections_.emplace(std::make_pair(key, std::move(new_connection)));
                        s.connections_.at(key).update_events(net::EPollEvents::RDWR);
                        s.listener_ ->onNewConnection(s.connections_.at(key));
                        s.checkConnection(key);
                        s.connections_.at(key).subscribe(s.epoll_);
                    }
                }
                catch(net::ErrnoExcept & e){
                    if(e.get_errno() != EAGAIN) throw e;
                    s.epoll_.mod(s.server_.get_ds(), EPOLLIN | EPOLLET | EPOLLONESHOT);
                }
            }
            else{
                int key = it.data.fd;
                bool deleted = false;
                if(it.events & EPOLLRDHUP){
                    error = false;
                    s.connections_.erase(key);
                    deleted = true;
                }
                if(!deleted && it.events & EPOLLIN){
                    error = false;
                    try{
                        while(true){
                            s.connections_.at(key).read_to_buf(COUNT_BYTES);
                        }
                    }
                    catch(net::ErrnoExcept & e){
                        if(e.get_errno() != EAGAIN) throw e;
                    }
                    GET_request req;
                    if(req.to_request(s.connections_.at(key).get_read_buf())){
                        if(req.get_value_of("Connection") != "Keep-Alive"){
                            s.connections_.at(key).set_keep_alive(false);
                        }
                        s.listener_->onRequest(s.connections_.at(key), req);
                    }
                    deleted = s.checkConnection(key);
                }
                if(!deleted && it.events & EPOLLOUT){
                    error = false;
                    if(!s.connections_.at(key).get_write_buf().empty()){
                        s.connections_.at(key).write_from_buf();
                    }
                    if(s.connections_.at(key).get_write_buf().empty()){
                        if(s.connections_.at(key).get_keep_alive()){
                            s.connections_.at(key).update_events(net::EPollEvents::RDONLY);
                        }
                        else{
                            s.connections_.erase(key);
                            deleted = true;
                        }
                    }
                    else{
                        s.connections_.at(key).update_events(net::EPollEvents::WRONLY);
                    }
                }
                if(!deleted && error){
                    s.connections_.erase(key);
                }
                else if(!deleted){
                    s.connections_.at(key).subscribe(s.epoll_);
                }
            }
        }
    }
    }catch(std::exception & e){
        std::lock_guard<std::mutex> locker(cerr_mutex);
        std::cerr << "Exception in thread " << std::this_thread::get_id() << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

HttpService::HttpService(net::IServiceListener *listener) : Service(listener) {}

void HttpService::open(const std::string & addr, uint16_t port, int max_connection){
    server_ .open(addr, port, max_connection);
    epoll_.add(server_.get_ds(), EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLET);
}

void HttpService::run(size_t thread_count, size_t maxivents, int timeout_ms){
    if(!listener_) throw std::runtime_error("listener is not valid");
    std::unique_ptr<std::thread[], std::function<void (std::thread *)>> threads(new std::thread[thread_count],
    [thread_count](std::thread *threads){
        for(size_t i = 0; i < thread_count; ++i)
            if(threads[i].joinable()) threads[i].join();
        delete [] threads;
    });
    for(size_t i = 0; i < thread_count; ++i){
        threads[i] = std::thread(worker, std::ref(*this), maxivents, timeout_ms);
    }
}
} //namespace http