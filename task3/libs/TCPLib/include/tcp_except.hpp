#ifndef TCPEXCEPT_HPP
#define TCPEXCEPT_HPP

#include <exception>
#include <string>

namespace tcp{
class errno_except : public std::exception
{
    std::string msg_;
public:
    errno_except(int err, const char *msg);
    const char *what() const noexcept;
};
};

#endif