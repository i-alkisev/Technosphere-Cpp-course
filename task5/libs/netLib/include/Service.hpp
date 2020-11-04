#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <memory>

#include "BufferedConnection.hpp"
#include "Server.hpp"

namespace net{

struct IServiceListener
{
    virtual void onNewConnection (BufferedConnection & buf_connection, const EPoll & epoll) = 0;
    virtual void onClose         (BufferedConnection & buf_connection, const EPoll & epoll) = 0;
    virtual void onWriteAvailable(BufferedConnection & buf_connection, const EPoll & epoll) = 0;
    virtual void onReadAvailable (BufferedConnection & buf_connection, const EPoll & epoll) = 0;
    virtual void onError         (BufferedConnection & buf_connection, const EPoll & epoll) = 0;
    virtual int  onTimeout       () = 0;
};

class Service
{
    std::unique_ptr<IServiceListener> listener_;
    std::vector<BufferedConnection> connections_;
    Server server_;
    EPoll epoll_;
    int find_connection_pos(int fd);
    void closeConnection(BufferedConnection);
public:
    Service(std::unique_ptr<IServiceListener> listener);
    void setListener(std::unique_ptr<IServiceListener> listener);
    void open(const std::string & addr, uint16_t port, int max_connection = SOMAXCONN);
    void close();
    void run(int timeout_ms = 10000, int maxivents = 1024);
    Service(const Service &) = delete;
    Service & operator=(const Service &) = delete;
    ~Service() = default;
};
}

#endif