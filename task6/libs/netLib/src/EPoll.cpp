#include "EPoll.hpp"
#include "ErrnoExcept.hpp"
#include "cerrno"

namespace net{
EPoll::EPoll(){
    int result = ::epoll_create(1);
    if(result < 0){
        throw ErrnoExcept(errno, "Error creating epoll");
    }
    epoll_fd_.set_fd(result);
}

void EPoll::mod(const Descriptor & ds, int flag) const{
    ::epoll_event event{};
    event.events = flag;
    event.data.fd = ds.get_fd();
    if(::epoll_ctl(epoll_fd_.get_fd(),
                    EPOLL_CTL_MOD,
                    ds.get_fd(), &event) < 0){
        throw ErrnoExcept(errno, "Error modifying epoll");
    }
}
void EPoll::add(const Descriptor & ds, int flag) const{
    ::epoll_event event{};
    event.events = flag;
    event.data.fd = ds.get_fd();
    if(::epoll_ctl(epoll_fd_.get_fd(),
                    EPOLL_CTL_ADD,
                    ds.get_fd(), &event) < 0){
        throw ErrnoExcept(errno, "Error adding fd to epoll");
    }
}

void EPoll::del(const Descriptor & ds) const{
    ::epoll_event event{};
    event.data.fd = ds.get_fd();
    if(::epoll_ctl(epoll_fd_.get_fd(),
                    EPOLL_CTL_DEL,
                    ds.get_fd(), &event) < 0){
        throw ErrnoExcept(errno, "Error deleting fd from epoll");
    }
}

std::vector<::epoll_event> EPoll::wait(int maxivents, int timeout_ms) const{
    std::vector<::epoll_event> events(maxivents);
    int result = ::epoll_wait(epoll_fd_.get_fd(), events.data(), maxivents, timeout_ms);
    if(result < 0){
        throw ErrnoExcept(errno, "Error on waiting event");
    }
    events.resize(result);
    return events;
}
}