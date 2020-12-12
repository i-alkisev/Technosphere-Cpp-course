#include <unistd.h>
#include "Descriptor.hpp"

namespace descr{
Descriptor::Descriptor() : fd_(-1){}

Descriptor::Descriptor(int fd) : fd_(fd){}

Descriptor & Descriptor::operator=(Descriptor && ds){
    int tmp = fd_;
    fd_ = ds.fd_;
    ds.fd_ = tmp;
    return *this;
}

Descriptor::Descriptor(Descriptor && ds) : fd_(ds.fd_){
    ds.fd_ = -1;
}

void Descriptor::set_fd(int fd){
    if((fd_ != -1) && (fd_ != fd)){close();}
    fd_ = fd;
}

int Descriptor::get_fd() const{
    return fd_;
}

void Descriptor::close(){
    if(fd_ != -1){
        ::close(fd_);
        fd_ = -1;
    }
}

Descriptor::operator bool() const{
    return fd_ == -1 ? false : true;
}

Descriptor::~Descriptor(){
    close();
}
} //namespace descr