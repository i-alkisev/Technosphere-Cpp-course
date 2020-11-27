#include "ErrnoExcept.hpp"
#include <cstring>

namespace net{
ErrnoExcept::ErrnoExcept(int err, const std::string & msg) :
    msg_(msg.data()), err_(err){}

const char *ErrnoExcept::what() const noexcept{
    return msg_.data();
}

int ErrnoExcept::get_errno() const{
    return err_;
}

std::string ErrnoExcept::get_extended_message() const{
    return "ERROR NAME: " + std::string(strerror(err_)) + "\nMESSAGE: " + msg_;
}
}