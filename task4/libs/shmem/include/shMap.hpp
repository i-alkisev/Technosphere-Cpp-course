#ifndef SHARED_MAP_HPP
#define SHARED_MAP_HPP

#include <map>
#include "semaphore.hpp"
#include "errnoExcept.hpp"
#include "shAllocator.hpp"

namespace shmem{
#define ShMap std::map<K,V,std::less<K>,ShAlloc<std::pair<K,V>>>

template<typename K, typename V>
class SharedMap
{
    char *mmap_;
    ShMap *map_;
    Semaphore *sem_;
    size_t shmem_size_;
public:
    SharedMap(size_t block_size, size_t blocks_count){
        shmem_size_ = blocks_count * block_size;
        mmap_ = static_cast<char *>(::mmap(0, shmem_size_,
                                          PROT_READ | PROT_WRITE,
                                          MAP_ANONYMOUS | MAP_SHARED,
                                          -1, 0));
        if(mmap_ == MAP_FAILED) {
            throw errnoExcept(errno, "mmap_ error");
        }
        ShMemState* state = new(mmap_) ShMemState{};
        sem_ = new(mmap_ + sizeof(state)) Semaphore{};
        SemLock sem_lock(*sem_);
        float header_size = (sizeof(Semaphore) + sizeof(ShMemState) + blocks_count) / static_cast<float>(block_size);
        state->block_size = block_size;
        //state->blocks_count = blocks_count - std::floor(header_size);
        state->blocks_count = blocks_count - std::ceil(header_size);
        state->used_blocks_table = mmap_ + sizeof(ShMemState) + sizeof(Semaphore);
        state->first_block = state->used_blocks_table + state->blocks_count;
        ::memset(state->used_blocks_table, FREE_BLOCK, state->blocks_count);
        ShAlloc<ShMap> alloc{state};
        map_ = new(alloc.allocate(8)) ShMap{alloc};
    }
    void update(K key, V value){
        SemLock sem_lock(*sem_);
        if(map_->count(key)) (*map_)[key] = value;
        else throw std::out_of_range("invalid key");
    }
    void insert(K key, V value){
        SemLock sem_lock(*sem_);
        (*map_)[key] = value;
    }
    V & get(const K & key){
        SemLock sem_lock(*sem_);
        if(map_->count(key)) return (*map_)[key];
        throw std::out_of_range("invalid key");
    }
    void remove(K key){
        SemLock sem_lock(*sem_);
        map_->erase(key);
    }
    ~SharedMap(){
        sem_->~Semaphore();
        map_->~map();
        ::munmap(mmap_, shmem_size_);
    }
};
}

#endif