#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include "Connection.hpp"

namespace net{
class Server
{
    Descriptor ds_;
    void check(const std::string & where);
public:
    Server() = default;
    Server(Server && server);
    Server(const std::string & addr, uint16_t port, int max_connection=SOMAXCONN);
    void open(const std::string & addr, uint16_t port, int max_connection=SOMAXCONN);
    Server & operator=(Server && server);
    Server(const Server &) = delete;
    Server & operator=(const Server &) = delete;
    Connection accept();
    void close();
    ~Server() = default;
    const Descriptor & get_ds() const;
    friend class Service;
};
}

#endif