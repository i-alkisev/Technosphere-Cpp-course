#include <iostream>
#include <stdexcept>
#include <LoggerLib.hpp>

namespace log
{
    std::map<Level, std::string> log_heading = {{Level::DEBUG, "DEBUG: "},
                                                {Level::INFO, "INFO: "},
                                                {Level::WARNING, "WARNING: "},
                                                {Level::ERROR, "ERROR: "}};

    BaseLogger::~BaseLogger(){}

    void BaseLogger::log(const std::string & msg, Level level){
        if (level >= level_){
            print(msg);
        }
    }

    void BaseLogger::debug(const std::string & msg){
        log(log_heading[Level::DEBUG] + msg, Level::DEBUG);
    }
    
    void BaseLogger::info(const std::string & msg){
        log(log_heading[Level::INFO] + msg, Level::INFO);
    }

    void BaseLogger::warn(const std::string & msg){
        log(log_heading[Level::WARNING] + msg, Level::WARNING);
    }

    void BaseLogger::error(const std::string & msg){
        log(log_heading[Level::ERROR] + msg, Level::ERROR);
    }

    void BaseLogger::set_level(const Level level){
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

    Logger & Logger::get_instance(){
        static Logger logger;
        return logger;
    }

    void Logger::set_global_logger(std::unique_ptr<BaseLogger> global_logger){
        global_logger_ = std::move(global_logger);
    }

    const std::unique_ptr<BaseLogger> & Logger::get_global_logger() const{
        return global_logger_;
    }

    Logger::Logger(){
        global_logger_ = std::make_unique<StderrLogger>();
    }

    void init_with_stderr_logger(Level lvl){
        Logger::get_instance().set_global_logger(std::make_unique<StderrLogger>());
        set_logger_level(lvl);
    }

    void init_with_stdout_logger(Level lvl){
        Logger::get_instance().set_global_logger(std::make_unique<StdoutLogger>());
        set_logger_level(lvl);
    }

    void init_with_file_logger(const std::string & path, Level lvl){
        Logger::get_instance().set_global_logger(std::make_unique<FileLogger>(path));
        set_logger_level(lvl);
    }

    void set_logger_level(Level lvl){
        Logger::get_instance().get_global_logger()->set_level(lvl);
    }

    void debug(const std::string & msg){
        Logger::get_instance().get_global_logger()->debug(msg);
    }

    void info(const std::string & msg){
        Logger::get_instance().get_global_logger()->info(msg);
    }
    
    void warn(const std::string & msg){
        Logger::get_instance().get_global_logger()->warn(msg);
    }
    
    void error(const std::string & msg){
        Logger::get_instance().get_global_logger()->error(msg);
    }
    
}