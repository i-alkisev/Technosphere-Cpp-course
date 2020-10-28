#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include "Descriptor.hpp"

namespace tcp{
    class Connection{
        Descriptor ds_;
        void check(const std::string & where);
        Connection(Descriptor && ds);
    public:
        Connection() = default;
        Connection(const Connection &) = delete;
        Connection & operator=(const Connection &) = delete;
        Connection(Connection && connection);
        Connection(const std::string & addr, uint16_t port);
        void Connect(const std::string & addr, uint16_t port);
        Connection & operator=(Connection && connection);
        void close();
        size_t read(char *buf, size_t size);
        size_t write(const char *buf, size_t size);
        void readExact(char *buf, size_t size);
        void writeExact(const char *buf, size_t size);
        void set_timeout(size_t ms);
        friend class Server;
    };
}

#endif