#include "../include/process.hpp"

Process::Process(const std::string& path, const std::vector<char *>& argv){
    p_in_.reserve(2);
    p_out_.reserve(2);
    pipe(p_in_.data());
    pipe(p_out_.data());
    switch (pid_ = fork()){
    case -1:
        _exit(1);
        break;
    case 0:
        dup2(p_in_[0], STDIN_FILENO);
        ::close(p_in_[1]);
        dup2(p_out_[1], STDOUT_FILENO);
        ::close(p_out_[0]);
        execvp(path.data(), argv.data());
        _exit(1);
        break;
    default:
        ::close(p_in_[0]);
        ::close(p_out_[1]);
        break;
    }
}
Process::~Process(){
    close();
}
size_t Process::write(const void* data, size_t len){
    ssize_t n = ::write(p_in_[1], data, len);
    if (n == -1){
        _exit(1);
    }
    return n;
}
void Process::writeExact(const void* data, size_t len){
    size_t n = 0;
    while (n < len){
        n += write(static_cast<const char *>(data) + n, len - n);
    }
}
size_t Process::read(void* data, size_t len){
    ssize_t n = ::read(p_out_[0], data, len);
    if (n == -1){
        _exit(1);
    }
    return n;
}
void Process::readExact(void* data, size_t len){
    size_t n = 0;
    while (n < len){
        n += read(static_cast<char *>(data) + n, len - n);
    }
}
void Process::closeStdin(){
    ::close(p_in_[1]);
}
void Process::close(){
    if (pid_ > 0){
        kill(pid_, SIGINT);
        waitpid(pid_, nullptr, 0);
    }
}