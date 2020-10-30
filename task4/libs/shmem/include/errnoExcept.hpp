#ifndef ERRNO_EXCEPT_HPP
#define ERRNO_EXCEPT_HPP

#include <exception>
#include <string>
#include <cerrno>

namespace shmem{
class errnoExcept : public std::exception
{
    std::string msg_;
    int err_;
public:
    errnoExcept(int err, const std::string & msg);
    const char *what() const noexcept;
    int get_errno() const;
    std::string get_extended_message() const;
};
};

#endif