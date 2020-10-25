#include "tcp_except.hpp"
#include <cstring>

namespace tcp{
errno_except::errno_except(int err, const char *msg){
    msg_ = "ERROR NAME: " + std::string(strerror(err)) + "\nMESSAGE: " + msg;
}

const char *errno_except::what() const noexcept{
    return msg_.data();
}
}