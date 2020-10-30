#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>

namespace shmem{
class Semaphore
{
    sem_t val_;
    void destroy();
public:
    Semaphore();
    void post();
    void wait();
    Semaphore(const Semaphore &) = delete;
    Semaphore & operator=(const Semaphore &) = delete;
    ~Semaphore();
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