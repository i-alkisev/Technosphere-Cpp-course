#include <iostream>
#include <string>
#include <stdexcept>

#include <LoggerLib.hpp>

int test_StdoutLogger(log::Level level){
    log::StdoutLogger logger;
    logger.set_level(level);
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    std::cout << "Test completed. Logger level: " << static_cast<int>(logger.level()) << std::endl;
    return 0;
}

int test_StderrLogger(log::Level level){
    log::StderrLogger logger;
    logger.set_level(level);
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    std::cout << "Test completed. Logger level: " << static_cast<int>(logger.level()) << std::endl;
    return 0;
}

int test_FileLogger(const std::string & path, log::Level level){
    try{
        log::FileLogger logger(path);
        logger.set_level(level);
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warn("Warning message");
        logger.error("Error message");
        std::cout << "Test completed. Logger level: " << static_cast<int>(logger.level()) << std::endl;
        return 0;
    }
    catch(std::runtime_error & e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

int test_Logger_stdout(log::Level level){
    log::init_with_stdout_logger(level);
    log::debug("Debug message");
    log::info("Info message");
    log::warn("Warn message");
    log::error("Error message");
    return 0;
}

int test_Logger_stderr(log::Level level){
    log::init_with_stderr_logger(level);
    log::debug("Debug message");
    log::info("Info message");
    log::warn("Warn message");
    log::error("Error message");
    return 0;
}

int test_Logger_file(const std::string &path, log::Level level){
    try{
        log::init_with_file_logger(path, level);
        log::debug("Debug message");
        log::info("Info message");
        log::warn("Warning message");
        log::error("Error message");
        return 0;
    }
    catch(std::runtime_error & e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char **argv){
    if (argc < 3){
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }
    else if (argc > 5){
        std::cerr << "Too many arguments" << std::endl;
        return 1;
    }
    int lvl = std::stoi(argv[argc - 1]);
    if (lvl > 3 || lvl < 0){
        std::cerr << "Invalid logger level" << std::endl;
        return 1;
    }
    std::string option = argv[1];
    if(option == "--global"){
        if (argc < 4){
            std::cerr << "Not enough arguments" << std::endl;
            return 1;
        }
        option = argv[2];
        if(option == "--stdout"){
            return test_Logger_stdout(static_cast<log::Level>(lvl));
        }
        else if(option == "--stderr"){
            return test_Logger_stderr(static_cast<log::Level>(lvl));
        }
        else if(option == "--file"){
            if (argc < 5){
                std::cerr << "Not enough arguments" << std::endl;
                return 1;
            }
            std::string path = argv[3];
            return test_Logger_file(path, static_cast<log::Level>(lvl));
        }
    }
    if(option == "--stdout"){
        return test_StdoutLogger(static_cast<log::Level>(lvl));
    }
    else if(option == "--stderr"){
        return test_StderrLogger(static_cast<log::Level>(lvl));
    }
    else if(option == "--file"){
        if (argc < 4){
            std::cerr << "Not enough arguments" << std::endl;
            return 1;
        }
        std::string path = argv[2];
        return test_FileLogger(path, static_cast<log::Level>(lvl));
    }
    std::cout << "Invalid option: " << option << std::endl;
    return 1;
}