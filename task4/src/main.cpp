#include <iostream>
#include <shMap.hpp>
#include <sys/wait.h>
#include <unistd.h>

int test_1(){
  try{
  shmem::SharedMap<std::string, int> shmap(128, 128);
  shmap.insert("one", 1);
  int fork1 = ::fork();
  if(fork1 == 0){
    std::cout << "ch1 : shmap[1]=" << shmap.get("one") << std::endl;
    shmap.update("one", 11);
    std::cout << "ch1 : shmap[1]=" << shmap.get("one") << std::endl;
    shmap.insert("two", 2);
    return 0;
  }
  ::waitpid(fork1, nullptr, 0);
  std::cout << "shmap[1]=" << shmap.get("one") << std::endl;
  shmap.update("one", 111);
  std::cout << "shmap[1]=" << shmap.get("one") << std::endl;
  std::cout << "shmap[2]=" << shmap.get("two") << std::endl;
  shmap.destroy();
  return 0;
  }
  catch(shmem::errnoExcept & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.get_extended_message() << std::endl;
    return 1;
  }
  catch(std::exception & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

int test_2(){
  try{
  shmem::SharedMap<std::string, std::string> shmap(128, 128);
  shmap.insert("one", "one one one");
  int fork1 = ::fork();
  if(fork1 == 0){
    std::cout << "ch1 : shmap[1]=" << shmap.get("one") << std::endl;
    shmap.update("one", "one from child");
    std::cout << "ch1 : shmap[1]=" << shmap.get("one") << std::endl;
    shmap.insert("two abcdefghigklmnopqrstuvwxyz abcdefghigklmnopqrstuvwxyz", "two from child");
    return 0;
  }
  ::waitpid(fork1, nullptr, 0);
  std::cout << "shmap[1]=" << shmap.get("one") << std::endl;
  shmap.update("one", "one from parent one from parent one from parent one from parent");
  std::cout << "shmap[1]=" << shmap.get("one") << std::endl;
  std::cout << "shmap[2]=" << shmap.get("two abcdefghigklmnopqrstuvwxyz abcdefghigklmnopqrstuvwxyz") << std::endl;
  shmap.destroy();
  return 0;
  }
  catch(shmem::errnoExcept & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.get_extended_message() << std::endl;
    return 1;
  }
  catch(std::exception & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

int test_3(){
  try{
  shmem::SharedMap<int, std::basic_string<char, std::char_traits<char>, shmem::ShAlloc<char>>> shmap(128, 128);
  shmap.insert(1, "one one one");
  int fork1 = ::fork();
  if(fork1 == 0){
    std::cout << "ch1 : shmap[1]=" << shmap.get(1) << std::endl;
    shmap.update(1, "one from child");
    const char *str = (const char *)"two from child two from child two from child two from child";
    std::cout << "ch1 : shmap[1]=" << shmap.get(1) << std::endl;
    shmap.insert(2, str);
    std::cout << "ch1 : shmap[2]=" << shmap.get(2) << std::endl;
    return 0;
  }
  ::waitpid(fork1, nullptr, 0);
  std::cout << "shmap[1]=" << shmap.get(1) << std::endl;
  shmap.update(1, "one from parent one from parent one from parent one from parent");
  std::cout << "shmap[1]=" << shmap.get(1) << std::endl;
  std::cout << "shmap[2]=" << shmap.get(2) << std::endl;
  shmap.destroy();
  return 0;
  }
  catch(shmem::errnoExcept & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.get_extended_message() << std::endl;
    return 1;
  }
  catch(std::exception & e){
    std::cerr << "Error in test_1" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

int main(){
  test_3();
  return 0;
}

//Добрый вечер! У меня появился вопрос по четвертой домашке, наш словарь должен поддерживать std::string в качестве ключа и значения, а как при этом происходит переназначение аллокатора для строки и происходит ли вообще?
//У меня получается так, что если использовать строку в качестве ключа, то все вроде как работает, а если в качестве значени - вылазят ошибки. Если я правильно понимаю, происходит как раз та ситуация, которую разбирали на лекции.