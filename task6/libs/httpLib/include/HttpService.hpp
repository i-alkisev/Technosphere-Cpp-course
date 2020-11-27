#ifndef HTTP_SERVICE_HPP
#define HTTP_SERVICE_HPP

#include "Service.hpp"
#include <map>

namespace http{
class GET_request{
    std::map<std::string, std::string> data_;
public:
    bool to_request(std::string & str);
    std::string get_value_of(const std::string & key) const;
    std::map<std::string, std::string>::iterator begin();
    std::map<std::string, std::string>::iterator end();
    std::map<std::string, std::string>::const_iterator begin() const;
    std::map<std::string, std::string>::const_iterator end() const;
    void show();
};

class HttpService : public net::Service
{
public:
    HttpService(net::IServiceListener *listener);
    void open(const std::string & addr, uint16_t port, int max_connection = SOMAXCONN);
    void run(size_t thread_count, size_t maxivents, int timeout_ms = 20000);

    friend void worker(HttpService & s, size_t maxivents, int timeout_ms);
};

} //namespace http

#endif