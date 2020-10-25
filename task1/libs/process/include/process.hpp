#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <unistd.h>
#include <vector>
#include <string>

namespace proc{
    class Process
    {
        pid_t pid_;
        int fd_in_;
        int fd_out_;
    public:
        Process(const std::string& path, const std::vector<char *>& arg);
        ~Process();
        size_t write(const void* data, size_t len);
        void writeExact(const void* data, size_t len);
        size_t read(void* data, size_t len);
        void readExact(void* data, size_t len);
        void closeStdin();
        void close();
    };

    void close_pipe(int *p);
}

#endif