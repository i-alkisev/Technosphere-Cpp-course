#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <vector>

#include "Descriptor.hpp"

namespace net{
class EPoll
{
    net::Descriptor epoll_fd_;
public:
    EPoll();
    EPoll(const EPoll &) = delete;
    EPoll & operator=(const EPoll &) = delete;
    void mod(const Descriptor & descriptor, int flags) const;
    void add(const Descriptor & descriptor, int flags = EPOLLIN | EPOLLOUT | EPOLLRDHUP) const;
    void del(const Descriptor & descriptor) const;
    std::vector<::epoll_event> wait(int maxevents, int timeout_ms) const;
    ~EPoll() = default;
};
}

#endif