#include "process.hpp"

#include <stdexcept>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

Process::Process(const std::string& path, const std::vector<char *>& argv){
    if (pipe(p_in_) == -1){
        throw std::runtime_error("pipe-in creation error");
    }
    if (pipe(p_out_) == -1){
        ::close(p_in_[0]);
        p_in_[0] = -1;
        ::close(p_in_[1]);
        p_in_[1] = -1;
        throw std::runtime_error("pipe-out creation error");
    }
    int tmp_pipe[2];
    if (pipe2(tmp_pipe, O_CLOEXEC) == -1){
        ::close(p_in_[0]);
        p_in_[0] = -1;
        ::close(p_in_[1]);
        p_in_[1] = -1;
        ::close(p_out_[0]);
        p_out_[0] = -1;
        ::close(p_out_[1]);
        p_out_[1] = -1;
        throw std::runtime_error("tmp_pipe creation error");
    }
    switch (pid_ = fork()){
        case -1:
            ::close(p_in_[0]);
            p_in_[0] = -1;
            ::close(p_in_[1]);
            p_in_[1] = -1;
            ::close(p_out_[0]);
            p_out_[0] = -1;
            ::close(p_out_[1]);
            p_out_[1] = -1;
            throw std::runtime_error("fork error");
            break;
        case 0:
        {
            int dup_status1 = dup2(p_in_[0], STDIN_FILENO);
            int dup_status2 = dup2(p_out_[1], STDOUT_FILENO);
            ::close(p_in_[0]);
            ::close(p_in_[1]);
            ::close(p_out_[0]);
            ::close(p_out_[1]);
            if (dup_status1 == -1 || dup_status2 == -1){
                exit(1);
            }
            execvp(path.data(), argv.data());
            exit(1);
            break;
        }
        default:
        {
            ::close(tmp_pipe[1]);
            ::close(p_in_[0]);
            ::close(p_out_[1]);
            char tmp;
            int size = ::read(tmp_pipe[0], &tmp, 1);
            int status;
            if (waitpid(pid_, &status, WNOHANG) == -1){
                ::close(p_in_[1]);
                p_in_[1] = -1;
                ::close(p_out_[0]);
                p_out_[0] = -1;
                throw std::runtime_error{"exec error"};
            }
            break;
        }
    }
}

Process::~Process(){
    close();
}

size_t Process::write(const void* data, size_t len){
    if (p_in_[1] == -1){
        return 0;
    }
    ssize_t n = ::write(p_in_[1], data, len);
    if (n == -1){
        throw std::runtime_error{"write error"};
    }
    return n;
}

void Process::writeExact(const void* data, size_t len){
    if (p_in_[1] == -1){
        return;
    }
    size_t n = 0;
    while (n < len){
        n += write(static_cast<const char *>(data) + n, len - n);
    }
}

size_t Process::read(void* data, size_t len){
    if (p_out_[0] == -1){
        return 0;
    }
    ssize_t n = ::read(p_out_[0], data, len);
    if (n == -1){
        throw std::runtime_error{"read error"};
    }
    return n;
}

void Process::readExact(void* data, size_t len){
    if (p_out_[0] == -1){
        return;
    }
    size_t n = 0;
    while (n < len){
        n += read(static_cast<char *>(data) + n, len - n);
    }
}

void Process::closeStdin(){
    if (p_in_[1] != -1){
        ::close(p_in_[1]);
        p_in_[1] = -1;
    }
}

void Process::close(){
    if (pid_ > 0){
        kill(pid_, SIGINT);
        waitpid(pid_, nullptr, 0);
    }
    if (p_in_[1] != -1){
        ::close(p_in_[1]);
        p_in_[1] = -1;
    }
    if (p_out_[0] != -1){
        ::close(p_out_[0]);
        p_out_[0] = -1;
    }
}
