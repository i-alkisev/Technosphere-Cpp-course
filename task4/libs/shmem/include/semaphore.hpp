#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>

namespace shmem{
class Semaphore
{
    sem_t val_;
public:
    Semaphore();
    void post();
    void wait();
    void destroy();
    Semaphore(const Semaphore &) = delete;
    Semaphore & operator=(const Semaphore &) = delete;
    ~Semaphore() = default;
};

class SemLock
{
    Semaphore & sem_;
public:
    SemLock(Semaphore &);
    SemLock(const SemLock &) = delete;
    SemLock & operator=(const SemLock &) = delete;
    ~SemLock();
};
}

#endif