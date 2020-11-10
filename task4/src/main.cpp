#include <iostream>
#include <shMap.hpp>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

using ShString = std::basic_string<char, std::char_traits<char>, shmem::ShAlloc<char>>;

int test1(){
  try{
    shmem::SharedMap<ShString, ShString> shmap(128, 128);
    shmem::ShAlloc<char> alloc(shmap.get_allocator().state_);
    std::cout << "shmap.size()=" << shmap.size() << std::endl;
    shmap.insert({{"one", alloc}, {"inserted from father, before creating childs", alloc}});
    pid_t son1 = ::fork();
    if(son1 < 0) throw std::runtime_error("fork 1 error");
    if(!son1){
      shmap.insert({{"two", alloc}, {"inserted from first son", alloc}});
      ::sleep(2);
      std::cout << "son1: shmap.get(\"one\")=" << shmap.get({"one", alloc}) << std::endl;
      std::cout << "son1: shmap.get(\"three\")=" << shmap.get({"three", alloc}) << std::endl;
      std::cout << std::endl;
      ::exit(0);
    }
    pid_t son2 = ::fork();
    if(son2 < 0) throw std::runtime_error("fork 2 error");
    if(!son2){
      ::sleep(1);
      shmap.update({{"one", alloc}, {"updated from second son", alloc}});
      std::cout << "son2: shmap.get(\"one\")=" << shmap.get({"one", alloc}) << std::endl;
      std::cout << "son2: shmap.get(\"two\")=" << shmap.get({"two", alloc}) << std::endl;
      std::cout << "son2: shmap.get(\"three\")=" << shmap.get({"three", alloc}) << std::endl;
      std::cout << std::endl;
      shmap.insert({{"four", alloc}, {"inserted from second son", alloc}});
      ::exit(0);
    }
    shmap.insert({{"three", alloc}, {"inserted from father, after creating childs", alloc}});
    ::waitpid(son1, nullptr, 0);
    ::waitpid(son2, nullptr, 0);
    std::cout << "shmap.size()=" << shmap.size() << std::endl;
    std::cout << "father: shmap.get(\"one\")=" << shmap.get({"one", alloc}) << std::endl;
    std::cout << "father: shmap.get(\"two\")=" << shmap.get({"two", alloc}) << std::endl;
    std::cout << "father: shmap.get(\"three\")=" << shmap.get({"three", alloc}) << std::endl;
    std::cout << "father: shmap.get(\"four\")=" << shmap.get({"four", alloc}) << std::endl;
    shmap.remove({"three", alloc});
    std::cout << "after shmap.remove(\"three\")\nshmap.size()=" << shmap.size() << std::endl;
    shmap.destroy();
    return 0;
  }
  catch(shmem::errnoExcept & e){
    std::cout << "Error in test1" << std::endl;
    std::cout << e.get_extended_message() << std::endl;
    return 1;
  }
  catch(std::exception & e){
    std::cout << "Erro in test1" << std::endl;
    std::cout << e.what() << std::endl;
    return 1;
  }
}

int main(){
  return test1();
}