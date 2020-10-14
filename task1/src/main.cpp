#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "process.hpp"

void print_menu(){
    std::cout << "\"r\"  read N bytes" << std::endl;
    std::cout << "\"re\" read exactly N bytes" << std::endl;
    std::cout << "\"w\"  write N bytes" << std::endl;
    std::cout << "\"we\" write exactly N bytes" << std::endl;
    std::cout << "\"c\"  close STDIN" << std::endl;
    std::cout << "\"q\"  close the process" << std::endl;
}

void exec_command(std::string & command, proc::Process & p){
    std::string buf_str;
    std::vector<char> buf_vec;
    size_t count_bytes;
    if (command.length() == 0){
        std::cout << "Unknown command: \"\"" << std::endl;
        return;
    }
    switch (command[0])
    {
    // закрываем, если command == "q..."
    case 'q':
        p.close(); 
        break;
    // закрываем STDIN, если command == "c..."
    case 'c':
        p.closeStdin();
        break;
    case 'r':
        std::cout << "N=";
        std::cin >> count_bytes;
        getchar();
        buf_vec.reserve(count_bytes);
        // вызываем readExact(), если command == "re..."
        if (command.length() >= 1 && command[1] == 'e'){
            p.readExact(buf_vec.data(), count_bytes);
        }
        else{
            count_bytes = p.read(buf_vec.data(), count_bytes);
            std::cout << count_bytes << " bytes were read" << std::endl;
        }
        for(size_t i = 0; i < count_bytes; ++i){
            std::cout << buf_vec[i];
        }
        std::cout << std::endl;
        break;
    case 'w':
        std::cout << "Enter the data you want to write" << std::endl;
        getline(std::cin, buf_str);
        // вызываем writeExact(), если command == "we..."
        if (command.length() >= 1 && command[1] == 'e'){
            p.writeExact(buf_str.data(), buf_str.length());
        }
        else{
            count_bytes = p.write(buf_str.data(), buf_str.length());
            std::cout << count_bytes << " bytes were written" << std::endl;
        }
        break;
    default:
        std::cout << "Unknown command11: \"" << command << "\"" << std::endl;
        break;
    }
}

int main(int argc, char **argv){
    if (argc < 3){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    std::vector<char *> args(argc - 1);
    for (int i = 0; i < argc - 2; ++i){
        args[i] = argv[i + 2];
    }
    args[argc - 2] = nullptr;
    std::string path = argv[1];
    try{
        proc::Process p(path, args);
        std::string command = "";
        while(command != "q"){
            print_menu();
            getline(std::cin, command);
            exec_command(command, p);
        }
        std::cout << "Process \"" << path << "\" was closed" << std::endl;
    } catch(const std::runtime_error & e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}
