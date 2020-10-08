#include <string>
#include <vector>
#include <iostream>

#include "../lib/process/include/process.hpp"

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
    Process p(path, args);
    std::string command = "";
    std::vector<char> buf;
    size_t n;
    while(command != "q"){
        std::cout << "\"r b\"  read b bytes" << std::endl;
        std::cout << "\"re b\" read exatly b bytes" << std::endl;
        std::cout << "\"w b\"  write b bytes" << std::endl;
        std::cout << "\"we b\" write exatly b bytes" << std::endl;
        std::cout << "\"c\"    close STDIN" << std::endl;
        std::cout << "\"q\"    close the process" << std::endl;
        std::getline(std::cin, command);
        switch (command[0])
        {
        case 'q':
            p.close();
            break;
        case 'c':
            p.closeStdin();
            break;
        case 'r':
            if (command.length() >= 3){
                if (command[1] == 'e'){
                    sscanf(command.data(), "re %lu", &n);
                    buf.reserve(n);
                    p.readExact(buf.data(), n);
                }
                else{
                    sscanf(command.data(), "r %lu", &n);
                    buf.reserve(n);
                    p.read(buf.data(), n);
                }
                for (size_t i = 0; i < n; ++i){
                    std::cout << buf[i];
                }
                std::cout << std::endl;
            }
            break;
        case 'w':
            if (command.length() >= 3){
                if (command[1] == 'e'){
                    sscanf(command.data(), "we %lu", &n);
                    buf.reserve(n);
                    for (size_t i = 0; i < n; ++i){
                        buf[i] = getchar();
                    }
                    getchar();
                    p.writeExact(buf.data(), n);
                }
                else{
                    sscanf(command.data(), "w %lu", &n);
                    buf.reserve(n);
                    for (size_t i = 0; i < n; ++i){
                        buf[i] = getchar();
                    }
                    getchar();
                    p.write(buf.data(), n);
                }
            }
            break;
        default:
            std::cout << "Unknown command: \"" << command << "\"" << std::endl;
            break;
        }
    }
    std::cout << "Process \"" << path << "\" was closed" << std::endl;
    return 0;
}