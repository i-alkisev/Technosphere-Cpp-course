#ifndef LOGGERLIB_HPP
#define LOGGERLIB_HPP

#include <string>
#include <fstream>

namespace log
    {

    enum Level{DEBUG, INFO, WARNING, ERROR};

    class BaseLogger
    {
        Level level_;
    public:
        virtual ~BaseLogger();
        void debug(const std::string & msg);
        void info(const std::string & msg);
        void warn(const std::string & msg);
        void error(const std::string & msg);
        void set_level(const Level level);
        Level level() const;
        virtual void print(const std::string & msg);
        virtual void flush();
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
        ~StderrLogger();
    };

    struct FileLogger: BaseLogger
    {
        explicit FileLogger(const std::string & path);
        ~FileLogger();
        void print(const std::string & msg);
        void flush();
    private:
        FileLogger(const FileLogger & filelogger);
        std::ofstream file_;
    };
}

#endif