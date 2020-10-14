#include "process.hpp"

#include <stdexcept>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

namespace proc{
    Process::Process(const std::string& path, const std::vector<char *>& argv){
        int p_in[2];
        if (::pipe(p_in) == -1){
            throw std::runtime_error("pipe-in creation error");
        }
        int p_out[2];
        if (::pipe(p_out) == -1){
            close_pipe(p_out);
            throw std::runtime_error("pipe-out creation error");
        }
        int tmp_pipe[2];
        if (::pipe2(tmp_pipe, O_CLOEXEC) == -1){
            close_pipe(p_in);
            close_pipe(p_out);
            throw std::runtime_error("tmp_pipe creation error");
        }
        switch (pid_ = ::fork()){
            case -1:
                close_pipe(p_in);
                close_pipe(p_out);
                throw std::runtime_error("fork error");
                break;
            case 0:
            {
                int dup_status1 = ::dup2(p_in[0], STDIN_FILENO);
                int dup_status2 = ::dup2(p_out[1], STDOUT_FILENO);
                close_pipe(p_in);
                close_pipe(p_out);
                if (dup_status1 == -1 || dup_status2 == -1){
                    ::exit(1);
                }
                ::execvp(path.data(), argv.data());
                ::exit(1);
                break;
            }
            default:
            {
                ::close(tmp_pipe[1]);
                ::close(p_in[0]);
                fd_in_ = p_in[1];
                ::close(p_out[1]);
                fd_out_ = p_out[0];
                char tmp;
                int size = ::read(tmp_pipe[0], &tmp, 1);
                ::close(tmp_pipe[0]);
                int status;
                if (::waitpid(pid_, &status, WNOHANG) == -1){
                    ::close(fd_in_);
                    fd_in_ = -1;
                    ::close(fd_out_);
                    fd_out_ = -1;
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
        if (fd_in_ == -1){
            throw std::runtime_error("attempt to write on a closed descriptor");
        }
        ssize_t n = ::write(fd_in_, data, len);
        if (n == -1){
            throw std::runtime_error{"write error"};
        }
        return n;
    }

    void Process::writeExact(const void* data, size_t len){
        if (fd_in_ == -1){
            throw std::runtime_error("attempt to write on a closed descriptor");
        }
        size_t n = 0;
        while (n < len){
            n += write(static_cast<const char *>(data) + n, len - n);
        }
    }

    size_t Process::read(void* data, size_t len){
        if (fd_out_ == -1){
            throw std::runtime_error("attempt to read on a closed descriptor");
        }
        ssize_t n = ::read(fd_out_, data, len);
        if (n == -1){
            throw std::runtime_error{"read error"};
        }
        return n;
    }

    void Process::readExact(void* data, size_t len){
        if (fd_out_ == -1){
            throw std::runtime_error("attempt to read on a closed descriptor");
        }
        size_t n = 0, count_bytes;
        int status;
        while (n < len){
            count_bytes = read(static_cast<char *>(data) + n, len - n);
            n += count_bytes;
            if (!count_bytes && (::waitpid(pid_, &status, WNOHANG) == pid_)){
                pid_ = 0;
                ::close(fd_out_);
                fd_out_ = -1;
                if (fd_in_ != -1){
                    ::close(fd_in_);
                    fd_in_ = -1;
                }
                throw std::runtime_error{"the process did not issue the required number of bytes"};
            }
        }
    }

    void Process::closeStdin(){
        if (fd_in_ != -1){
            ::close(fd_in_);
            fd_in_ = -1;
        }
    }

    void Process::close(){
        if (pid_){
            ::kill(pid_, SIGINT);
            ::waitpid(pid_, nullptr, 0);
        }
        if (fd_in_ != -1){
            ::close(fd_in_);
            fd_in_ = -1;
        }
        if (fd_out_ != -1){
            ::close(fd_out_);
            fd_out_ = -1;
        }
    }

    void close_pipe(int *p){
        ::close(p[0]);
        ::close(p[1]);
    }
}
