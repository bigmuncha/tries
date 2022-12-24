#include <experimental/source_location>
#include <string_view>
#include <sstream>
#include <iostream>
enum LogLevel
{
    NONE,
    ERROR,
    INFO,
    DEBUG
};
//#define LOG_DEBUG
//#define LOG_INFO
//#define LOG_ERROR
static constexpr LogLevel getLogLevel()
{
    int lev_count = 0;
#ifdef LOG_ERROR
    lev_count = LogLevel::ERROR;
#endif
#ifdef LOG_INFO
    lev_count = LogLevel::INFO;
#endif	
#ifdef LOG_DEBUG
    lev_count = LogLevel::DEBUG;
#endif
	return static_cast<LogLevel>(lev_count);
}

class Logger: public std::stringstream
{
    std::string_view getLogLevelStr(LogLevel lev)
    {
	std::string_view result;
	switch(lev)
	{
	case NONE:
	{
	    result = "";
	    break;
	}
	case ERROR:
	{
	    result = "ERROR";
	    break;
	}
	case INFO:
	{
	    result = "INFO";
	    break;
	}
	case DEBUG:
	{
	    result = "DEBUG";
	    break;
	}
	}

	return result;
    }
public:
    Logger(LogLevel level,
	   const std::experimental::source_location location,
	   std::string tag = ""): level_(level), location_(location), tag_(tag) {}
    ~Logger(){
	std::string tag = (tag_ == "" ? "" : " [" + tag_ + "] ");
	std::cout << getLogLevelStr(level_)
		  << " file: "
		  << location_.file_name() << "("
		  << location_.line() << ":"
		  << location_.column() << ") "
	    //<< location_.function_name() << "`: "
		  << tag_ + str() << '\n';
    }
private:
    LogLevel level_;
    std::experimental::source_location location_;
    std::string tag_;
};
#define LLOG_DEBUG()                                                           \
  if constexpr (getLogLevel() >= LogLevel::DEBUG)                              \
  Logger(DEBUG, std::experimental::source_location::current())




#define LLOG_INFO()                                                             \
  if constexpr (getLogLevel() >= LogLevel::INFO)                               \
  Logger(INFO, std::experimental::source_location::current())

#define LLOG_ERROR()                                                            \
  if constexpr (getLogLevel() >= LogLevel::ERROR)                              \
  Logger(ERROR, std::experimental::source_location::current())

