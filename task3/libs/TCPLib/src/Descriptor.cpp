#include <unistd.h>
#include "Descriptor.hpp"

namespace tcp{
    Descriptor::Descriptor() : fd_(-1){}

    Descriptor::Descriptor(int fd) : fd_(fd){}

    Descriptor & Descriptor::operator=(Descriptor && ds){
        int tmp = fd_;
        fd_ = ds.fd_;
        ds.fd_ = tmp;
        return *this;
    }

    Descriptor::Descriptor(Descriptor && ds){
        int tmp = fd_;
        fd_ = ds.fd_;
        ds.fd_ = tmp;
    }

    void Descriptor::set_fd(int fd){
        if((fd_ != -1) && (fd_ != fd)){close();}
        fd_ = fd;
    }

    int Descriptor::get_fd(){return fd_;}

    void Descriptor::close(){
        if(fd_ != -1){
            ::close(fd_);
        }
    }

    Descriptor::operator bool() const{
        return fd_ == -1 ? false : true;
    }
    
    Descriptor::~Descriptor(){close();}
}