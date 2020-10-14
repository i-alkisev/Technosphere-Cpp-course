#include <iostream>
#include <string>
#include <stdexcept>

#include <LoggerLib.hpp>

int test_StdoutLogger(){
    log::StdoutLogger logger;
    std::cout << "set level for logger: ";
    int level;
    std::cin >> level;
    logger.set_level(static_cast<log::Level>(level));
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    std::cout << "Test completed. Logger level: " << logger.level() << std::endl;
    return 0;
}

int test_StderrLogger(){
    log::StderrLogger logger;
    std::cout << "set level for logger: ";
    int level;
    std::cin >> level;
    logger.set_level(static_cast<log::Level>(level));
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    std::cout << "Test completed. Logger level: " << logger.level() << std::endl;
    return 0;
}

int test_FileLogger(){
    std::cout << "Enter file path" << std::endl;
    std::string path;
    getline(std::cin, path);
    try{
        log::FileLogger logger(path);
        std::cout << "set level for logger: ";
        int level;
        std::cin >> level;
        logger.set_level(static_cast<log::Level>(level));
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warn("Warning message");
        logger.error("Error message");
        std::cout << "Test completed. Logger level: " << logger.level() << std::endl;
        return 0;
    }
    catch(std::runtime_error & e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char **argv){
    if (argc == 1){
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }
    else if (argc > 2){
        std::cerr << "Too many arguments" << std::endl;
        return 1;
    }
    std::string option = argv[1];
    if (option == "--stdout"){
        return test_StdoutLogger();
    }
    else if (option == "--stderr"){
        return test_StderrLogger();
    }
    else if (option == "--file"){
        return test_FileLogger();
    }
    std::cout << "Invalid option: " << option << std::endl;
    return 1;
}