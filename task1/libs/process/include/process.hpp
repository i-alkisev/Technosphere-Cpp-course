#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <unistd.h>
#include <vector>
#include <string>

class Process
{
    pid_t pid_;
    int p_in_[2];
    int p_out_[2];
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

#endif