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

void exec_command(std::string & command, Process & p){
    std::vector<char> buf;
    std::string buf_str;
    size_t n;
    if (command.length() == 0){
        std::cout << "Unknown command: \"\"" << std::endl;
        return;
    }
    switch (command[0])
    {
    case 'q':
        p.close();
        break;
    case 'c':
        p.closeStdin();
        break;
    case 'r':
        std::cout << "N=";
        std::cin >> n;
        getchar();
        buf.reserve(n);
        if (command.length() >= 1 && command[1] == 'e'){
            p.readExact(buf.data(), n);
        }
        else{
            n = p.read(buf.data(), n);
            std::cout << n << " bytes were read" << std::endl;
        }
        for (size_t i = 0; i < n; ++i){
            std::cout << buf[i];
        }
        std::cout << std::endl;
        break;
    case 'w':
        std::cout << "Enter the data you want to write" << std::endl;
        getline(std::cin, buf_str);
        if (command.length() >= 1 && command[1] == 'e'){
            p.writeExact(buf_str.data(), buf_str.length());
        }
        else{
            n = p.write(buf_str.data(), buf_str.length());
            std::cout << n << " bytes were written" << std::endl;
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
        Process p(path, args);
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
