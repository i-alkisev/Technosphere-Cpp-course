#include <unistd.h>
#include "Descriptor.hpp"

namespace tcp{
    Descriptor::Descriptor() : fd_(-1){}

    Descriptor::Descriptor(int fd) : fd_(fd){}

    Descriptor::Descriptor(Descriptor && dscr){
        fd_ = dscr.get_fd();
        dscr.set_fd(-1);
    }

    void Descriptor::set_fd(int fd){
        fd_ = fd;
    }

    int Descriptor::get_fd(){return fd_;}

    void Descriptor::close(){
        if(fd_ != -1){
            ::close(fd_);
        }
    }
    
    Descriptor::~Descriptor(){close();}
}