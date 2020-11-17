#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

namespace net{
class Descriptor
{
    int fd_;
public:
    Descriptor();
    Descriptor(int fd);
    Descriptor(Descriptor && descriptor);
    Descriptor & operator=(Descriptor && descriptor);
    Descriptor(const Descriptor &) = delete;
    Descriptor & operator=(const Descriptor &) = delete;
    void set_fd(int fd);
    int get_fd() const;
    void close();
    explicit operator bool() const;
    ~Descriptor();
};
}

#endif