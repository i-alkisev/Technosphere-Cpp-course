#include "semaphore.hpp"
#include "errnoExcept.hpp"

namespace shmem{
Semaphore::Semaphore(){
    if(::sem_init(&val_, 1, 1) < 0){
        throw errnoExcept(errno, "sem_init error");
    }
}

void Semaphore::post(){
    if(::sem_post(&val_) < 0){
        throw errnoExcept(errno, "sem_post error");
    }
}

void Semaphore::wait(){
    if(::sem_wait(&val_) < 0){
        throw errnoExcept(errno, "sem_wait error");
    }
}

void Semaphore::destroy(){
    if(::sem_destroy(&val_) < 0){
        throw errnoExcept(errno, "sem_destroy error");
    }
}

Semaphore::~Semaphore(){
    destroy();
}

SemLock::SemLock(Semaphore & sem) : sem_(sem){
    sem_.wait();
}
SemLock::~SemLock(){
    sem_.post();
}
}