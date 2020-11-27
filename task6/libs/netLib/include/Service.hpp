#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "BufferedConnection.hpp"
#include "Server.hpp"
#include <map>

namespace http{
class GET_request;
}

namespace net{

struct IServiceListener
{
    virtual void onNewConnection (net::BufferedConnection & buf_connection) {}
    virtual void onClose         (net::BufferedConnection & buf_connection) {}
    virtual void onWriteDone     (net::BufferedConnection & buf_connection) {}
    virtual void onReadAvailable (net::BufferedConnection & buf_connection) {}
    virtual void onError         (net::BufferedConnection & buf_connection) {}
    virtual void onRequest       (net::BufferedConnection & buf_connection, const http::GET_request & req) = 0;
};

class Service
{
protected:
    IServiceListener *listener_;
    std::map<int, BufferedConnection> connections_;
    Server server_;
    EPoll epoll_;
    void closeConnection(BufferedConnection);
public:
    Service(IServiceListener *listener);
    void setListener(IServiceListener *listener);
    void open(const std::string & addr, uint16_t port, int max_connection = SOMAXCONN);
    void close();
    void run(size_t maxivents, int timeout_ms = 20000);
    Service(const Service &) = delete;
    Service & operator=(const Service &) = delete;
    bool checkConnection(int pos);
    ~Service() = default;
};

} //namespace net

#endif