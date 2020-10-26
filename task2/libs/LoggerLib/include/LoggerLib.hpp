#ifndef LOGGERLIB_HPP
#define LOGGERLIB_HPP

#include <string>
#include <fstream>
#include <memory>

namespace log
    {

    enum class Level{DEBUG, INFO, WARNING, ERROR};

    class BaseLogger
    {
        Level level_;
        void log(const std::string & msg, Level level);
    public:
        virtual ~BaseLogger();
        void debug(const std::string & msg);
        void info(const std::string & msg);
        void warn(const std::string & msg);
        void error(const std::string & msg);
        void set_level(Level level);
        Level level() const;
        virtual void print(const std::string & msg) = 0;
        virtual void flush() = 0;
    };

    struct StdoutLogger: BaseLogger
    {
        void print(const std::string & msg);
        void flush();
        ~StdoutLogger();
    };

    struct StderrLogger: BaseLogger
    {
        void print(const std::string & msg);
        void flush();
    };

    struct FileLogger: BaseLogger
    {
        explicit FileLogger(const std::string & path);
        ~FileLogger();
        void print(const std::string & msg);
        void flush();
    private:
        FileLogger(const FileLogger & filelogger) = delete;
        std::ofstream file_;
    };

    struct Logger
    {
        static Logger & get_instance();
        BaseLogger & get_global_logger() const;
        void set_global_logger(std::unique_ptr<BaseLogger> global_logger);
    private:
        Logger();
        Logger(const Logger & logger) = delete;
        Logger & operator=(const Logger & logger) = delete;
        std::unique_ptr<BaseLogger> global_logger_;
    };

    void set_logger_level(Level lvl);
    void init_with_stderr_logger(Level lvl = Level::DEBUG);
    void init_with_stdout_logger(Level lvl = Level::DEBUG);
    void init_with_file_logger(const std::string & path, Level lvl = Level::DEBUG);

    void debug(const std::string & msg);
    void info(const std::string & msg);
    void warn(const std::string & msg);
    void error(const std::string & msg);

}

#endif