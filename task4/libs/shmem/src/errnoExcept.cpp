#include "errnoExcept.hpp"
#include <cstring>

namespace shmem{
errnoExcept::errnoExcept(int err, const std::string & msg) :
    msg_(msg.data()), err_(err){}

const char *errnoExcept::what() const noexcept{
    return msg_.data();
}

int errnoExcept::get_errno() const{
    return err_;
}

std::string errnoExcept::get_extended_message() const{
    return "ERROR NAME: " + std::string(strerror(err_)) + "\nMESSAGE: " + msg_;
}
}