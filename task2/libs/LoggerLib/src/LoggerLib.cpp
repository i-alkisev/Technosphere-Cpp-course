#include <iostream>
#include <stdexcept>
#include <LoggerLib.hpp>

namespace log
{
    BaseLogger::~BaseLogger(){}

    void BaseLogger::debug(const std::string & msg){
        if(level_ == DEBUG){
            print("DEBUG: " + msg);
        }
    }
    
    void BaseLogger::info(const std::string & msg){
        if(level_ <= INFO){
            print("INFO: " + msg);
        }
    }

    void BaseLogger::warn(const std::string & msg){
        if(level_ <= WARNING){
            print("WARNING: " + msg);
        }
    }

    void BaseLogger::error(const std::string & msg){
        print("ERROR: " + msg);
    }

    void BaseLogger::set_level(const Level level){
        if (level > ERROR || level < DEBUG){
            throw std::runtime_error("invalid level");
        }
        level_ = level;
    }

    Level BaseLogger::level() const {
        return level_;
    }

    void BaseLogger::print(const std::string & msg){}
    
    void BaseLogger::flush(){}

    void StdoutLogger::print(const std::string & msg){
        std::cout << msg << std::endl;
    }

    void StdoutLogger::flush(){
        std::cout << std::flush;
    }

    StdoutLogger::~StdoutLogger(){
        flush();
    }

    void StderrLogger::print(const std::string & msg){
        std::cerr << msg << std::endl;
    }

    void StderrLogger::flush(){}

    StderrLogger::~StderrLogger(){}

    FileLogger::FileLogger(const std::string & path){
        file_.open(path, std::ios::app);
        if(!file_.is_open()){
            throw std::runtime_error("file was not opened");
        }
    }

    FileLogger::~FileLogger(){
        flush();
        file_.close();
    }

    void FileLogger::print(const std::string & msg){
        file_ << msg << std::endl;
    }

    void FileLogger::flush(){
        file_ << std::flush;
    }

    FileLogger::FileLogger(const FileLogger & filelogger){};
}