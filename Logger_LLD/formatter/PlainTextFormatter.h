#ifndef PLAINTEXTFORMATTER_H
#define PLAINTEXTFORMATTER_H

#include "LogFormatter.h"
#include "../model/LogMessage.h"
#include "../enums/LogLevel.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace logger_lld {

class PlainTextFormatter : public LogFormatter {
private:
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default:              return "UNKNOWN";
        }
    }

    std::string formatTimestamp(long long epochMillis) const {
        std::time_t seconds = epochMillis / 1000;
        std::tm* tm_info = std::localtime(&seconds);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        return std::string(buf);
    }

public:
    std::string format(const LogMessage& message) override {
        return formatTimestamp(message.getTimestamp())
               + " [" + levelToString(message.getLevel()) + "] - "
               + message.getMessage();
    }
};

}

#endif // PLAINTEXTFORMATTER_H
