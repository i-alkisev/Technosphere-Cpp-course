#ifndef SHARED_MAP_HPP
#define SHARED_MAP_HPP

#include <map>
#include "semaphore.hpp"
#include "errnoExcept.hpp"
#include "shAllocator.hpp"

namespace shmem{

template<class K, class V>
class SharedMap
{
    using ShMap = std::map<K, V, std::less<K>, ShAlloc<std::pair<const K, V>>>;

    char *mmap_;
    ShMap *map_;
    Semaphore *sem_;
    size_t shmem_size_;
public:
    SharedMap(size_t block_size, size_t blocks_count){
        shmem_size_ = blocks_count * block_size;
        mmap_ = static_cast<char *>(::mmap(nullptr, shmem_size_,
                                          PROT_READ | PROT_WRITE,
                                          MAP_ANONYMOUS | MAP_SHARED,
                                          -1, 0));
        if(mmap_ == MAP_FAILED) {
            throw errnoExcept(errno, "mmap_ error");
        }
        ShMemState* state = new(mmap_) ShMemState{};
        sem_ = new(mmap_ + sizeof(ShMemState)) Semaphore{};
        SemLock sem_lock(*sem_);
        float header_size = (sizeof(Semaphore) + sizeof(ShMemState) + blocks_count + sizeof(ShMap))
                            / static_cast<float>(block_size);
        state->block_size = block_size;
        size_t header_size_in_blocks = std::ceil(header_size / static_cast<float>(block_size));
        state->blocks_count = blocks_count - header_size_in_blocks;
        state->used_blocks_table = mmap_ + sizeof(ShMemState) + sizeof(Semaphore) + sizeof(ShMap);
        state->first_block = state->used_blocks_table + state->blocks_count;
        ::memset(state->used_blocks_table, FREE_BLOCK, state->blocks_count);
        ShAlloc<std::pair<const K, V>> alloc{state};
        map_ = new(mmap_ + sizeof(ShMemState) + sizeof(Semaphore)) ShMap{alloc};
    }

    auto get_allocator(){
        return map_->get_allocator();
    }

    void update(std::pair<const K, V> pair){
        SemLock sem_lock(*sem_);
        map_->at(pair.first) = pair.second;
    }

    void insert(std::pair<const K, V> pair){
        SemLock sem_lock(*sem_);
        map_->insert(pair);
    }

    V get(K key){
        SemLock sem_lock(*sem_);
        return map_->at(key);
    }

    void remove(K key){
        SemLock sem_lock(*sem_);
        map_->erase(key);
    }

    size_t size(){
        SemLock sem_lock(*sem_);
        return map_->size();
    }

    void destroy(){
        sem_->destroy();
        map_->~map();
    }

    ~SharedMap(){
        ::munmap(mmap_, shmem_size_);
    }
};
} //namespace shmem

#endif