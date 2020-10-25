#include <iostream>
#include <string>
#include <cstdio>

#include "Connection.hpp"
#include "Server.hpp"
#include "tcp_except.hpp"

#define TIMEOUT 10000
#define BUF_SIZE 1024

enum class Mode{READ, READ_E, WRITE, WRITE_E};

void test_connection_functions(tcp::Connection & connection, Mode mode){
    std::string buf1(BUF_SIZE, '\0');
    std::string buf2;
    size_t count_bytes;
    switch (mode)
    {
    case Mode::READ:
        do{
            count_bytes = connection.read(buf1.data(), buf1.length());
            std::cout << buf1.substr(0, count_bytes);
        }while(count_bytes);
        break;
    case Mode::READ_E:
        do{
            std::cout << "How many bytes to read" << std::endl;
            std::cin >> count_bytes;
            connection.readExact(buf1.data(), count_bytes);
            std::cout << buf1.substr(0, count_bytes) << std::endl;
        }while(count_bytes);
        break;
    case Mode::WRITE:
        do{
            std::getline(std::cin, buf2);
            buf2.push_back('\n');
            connection.write(buf2.data(), buf2.length());
        }while(buf2 != "\n");
        break;
    case Mode::WRITE_E:
        do{
            std::getline(std::cin, buf2);
            buf2.push_back('\n');
            connection.writeExact(buf2.data(), buf2.length());
            std::cout << "gg\n";
        }while(buf2 != "\n");
        break;
    default:
        break;
    }
}

int test_server(std::string addr, int port, Mode mode){
    try{
        tcp::Server server(addr, port);
        server.set_timeout(TIMEOUT);
        tcp::Connection connection = server.accept();
        test_connection_functions(connection, mode);
        connection.close();
        server.close();
        return 0;
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
}

int test_client(std::string addr, int port, Mode mode){
    try{
        tcp::Connection connection(addr, port);
        connection.set_timeout(TIMEOUT);
        test_connection_functions(connection, mode);
        connection.close();
        return 0;
    }
    catch(tcp::errno_except & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char **argv){
    if(argc < 5){
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }
    if(argc > 5){
        std::cerr << "Too many arguments" << std::endl;
        return 1;
    }
    std::string opt = argv[1];
    std::string addr = argv[2];
    int port;
    sscanf(argv[3], "%d", &port);
    int m;
    sscanf(argv[4], "%d", &m);
    if(m < 0 || m > 3){
        std::cerr << "Invalid mode: " << m << std::endl;
        return 1;
    }
    Mode mode = static_cast<Mode>(m);
    if(opt == "--server"){
        return test_server(addr, port, mode);
    }
    else if(opt == "--client"){
        return test_client(addr, port, mode);
    }
    std::cerr << "Invalid option: " << opt << std::endl;
    return 1;
}