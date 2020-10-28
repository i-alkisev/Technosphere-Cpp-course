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
        Descriptor & operator=(Descriptor &&);
        Descriptor(const Descriptor &) = delete;
        Descriptor & operator=(const Descriptor &) = delete;
        void set_fd(int fd);
        int get_fd();
        void close();
        explicit operator bool() const;
        ~Descriptor();
    };
}

#endif