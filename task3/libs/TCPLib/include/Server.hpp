#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include "Connection.hpp"

namespace tcp{
    class Server
    {
        Descriptor ds_;
    public:
        Server();
        Server(Server && server);
        Server(const std::string & addr, uint16_t port, int max_connection=SOMAXCONN);
        void open(const std::string & addr, uint16_t port, int max_connection=SOMAXCONN);
        Server & operator=(Server &&);
        Connection accept();
        void close();
        void set_max_connection(int max_connection);
        void set_timeout(__time_t ms);
        void check(const std::string & where);
    };
}

#endif