#ifndef BUFFERED_CONNECTION_HPP
#define BUFFERED_CONNECTION_HPP

#include "Connection.hpp"
#include "EPoll.hpp"

namespace net{

class BufferedConnection
{
    std::string read_buf_;
    std::string write_buf_;
    Connection connection_;
    int events_ = 0;
    bool keep_alive_ = true;
public:
    BufferedConnection() = default;
    BufferedConnection(Connection && connection);
    BufferedConnection(BufferedConnection && buf_connection);
    BufferedConnection & operator=(BufferedConnection && buf_connection);
    BufferedConnection & operator=(const BufferedConnection &) = delete;
    BufferedConnection(const BufferedConnection &) = delete;
    void subscribe(const EPoll & epoll_) const;
    void read_to_buf(size_t len);
    void write_from_buf();
    void write_to_buf(const std::string & str);
    std::string & get_read_buf();
    std::string & get_write_buf();
    void clear_read_buf();
    void close();
    bool operator==(int fd);
    bool isValid() const;
    void update_events(EPollEvents flag);
    const Descriptor & get_ds() const;
    ~BufferedConnection() = default;
    void set_keep_alive(bool value);
    bool get_keep_alive() const;
};
}

#endif