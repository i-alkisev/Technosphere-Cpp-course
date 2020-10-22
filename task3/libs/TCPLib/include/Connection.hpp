#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include "Descriptor.hpp"

namespace tcp{
    class Connection{
        Descriptor ds_;
    public:
        Connection();
        Connection(Connection && connection);
        Connection(Descriptor && ds);
        Connection(const std::string & addr, uint16_t port);
        void Connect(const std::string & addr, uint16_t port);
        Connection & operator=(Connection && connection);
        void close();
        size_t read(char *buf, size_t size);
        size_t write(const char *buf, size_t size);
        void readExact(char *buf, size_t size);
        void writeExact(const char *buf, size_t size);
        void set_timeout(__time_t ms);
        void check(const std::string & where);
    };
}

#endif