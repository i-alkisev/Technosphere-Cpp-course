#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

class Process
{
    std::vector<int> p_in_;
    std::vector<int> p_out_;
    pid_t pid_;
public:
    explicit Process(const std::string& path, const std::vector<char *>& argv);
    ~Process();
    size_t write(const void* data, size_t len);
    void writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void readExact(void* data, size_t len);
    void closeStdin();
    void close();
};

#endif