#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <vector>

#include "Descriptor.hpp"

namespace net{
enum class EPollEvents{NONE, RDONLY, WRONLY, RDWR};

class EPoll
{
    net::Descriptor epoll_fd_;
    int needed_events(EPollEvents flag) const;
public:
    EPoll();
    EPoll(const EPoll &) = delete;
    EPoll & operator=(const EPoll &) = delete;
    void mod(const Descriptor & descriptor, EPollEvents flag) const;
    void add(const Descriptor & descriptor, EPollEvents flag) const;
    void del(const Descriptor & descriptor) const;
    std::vector<::epoll_event> wait(int maxevents, int timeout_ms) const;
    ~EPoll() = default;
};
}

#endif