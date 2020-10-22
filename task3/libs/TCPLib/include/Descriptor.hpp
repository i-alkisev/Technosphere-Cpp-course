#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

namespace tcp{
    class Descriptor
    {
        int fd_;
    public:
        Descriptor();
        Descriptor(int fd);
        Descriptor(Descriptor && descriptor);
        void set_fd(int fd);
        int get_fd();
        void close();
        ~Descriptor();
    };
}

#endif