#ifndef TCPEXCEPT_HPP
#define TCPEXCEPT_HPP

#include <exception>
#include <string>

namespace net{
class ErrnoExcept : public std::exception
{
    std::string msg_;
    int err_;
public:
    ErrnoExcept(int err, const std::string & msg);
    const char *what() const noexcept;
    int get_errno() const;
    std::string get_extended_message() const;
};
};

#endif