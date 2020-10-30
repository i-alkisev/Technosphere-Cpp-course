#include <iostream>
#include <shMap.hpp>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  try{
  shmem::SharedMap<std::string, int> shmap(128, 128);
  std::string long_str("It is first very very very very very long string here");
  shmap.insert("one", 1);
  shmap.insert("two", 2);
  shmap.insert("three", 3);
  shmap.insert(long_str, -1);
  int fork1 = ::fork();
  if(fork1 == 0){
    shmap.insert("four", 5);
    shmap.update("two", 5);
    shmap.insert("three", 123);
    std::cout << "ch1 shmap[\"one\"]=" << shmap.get("one") << std::endl;
    ::sleep(2);
    std::cout << "ch1 shmap[\"four\"]=" << shmap.get("four") << std::endl;
    return 0;
  }
  int fork2 = ::fork();
  if(fork2 == 0){
    ::sleep(1);
    shmap.insert("five", 5);
    shmap.update("four", 4);
    std::cout << "ch2 shmap[\"two\"]=" << shmap.get("two") << std::endl;
    std::cout << "ch2 shmap[long_str]=" << shmap.get(long_str) << std::endl;
    shmap.update(long_str, -2);
    return 0;
  }
  ::waitpid(fork1, nullptr, 0);
  ::waitpid(fork2, nullptr, 0);
  std::cout << "shmap[long_str]=" << shmap.get(long_str) << std::endl;
  std::cout << "shmap[\"one\"]=" << shmap.get("one") << std::endl;
  std::cout << "shmap[\"two\"]=" << shmap.get("two") << std::endl;
  std::cout << "shmap[\"three\"]=" << shmap.get("three") << std::endl;
  std::cout << "shmap[\"four\"]=" << shmap.get("four") << std::endl;
  std::cout << "shmap[\"five\"]=" << shmap.get("five") << std::endl;
  shmap.remove(long_str);
  shmap.remove("one");
  }
  catch(shmem::errnoExcept & e){
    std::cout << e.get_extended_message() << std::endl;
  }
  catch(std::exception & e){
    std::cout << e.what() << std::endl;
  }
  return 0;
}