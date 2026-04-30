#ifndef LOGGER_H
#define LOGGER_H

#include "enums/LogLevel.h"
#include "handlers/LogHandler.h"
#include "model/LogMessage.h"
#include "LogHandlerConfiguration.h"
#include <memory>
#include <string>
#include <chrono>

class Logger {
private:
    std::shared_ptr<LogHandler> handlerChain;

    // Private constructor - Singleton pattern
    Logger() {
        handlerChain = LogHandlerConfiguration::build();
    }

    // Get current time in milliseconds
    static long long currentTimeMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    // Delete copy constructor and assignment (Singleton)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(LogLevel level, const std::string& message) {
        LogMessage msg(level, message, currentTimeMillis());
        handlerChain->handle(msg);
    }

    void debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
    void info(const std::string& msg)  { log(LogLevel::INFO,  msg); }
    void warn(const std::string& msg)  { log(LogLevel::WARN,  msg); }
    void error(const std::string& msg) { log(LogLevel::ERROR, msg); }
    void fatal(const std::string& msg) { log(LogLevel::FATAL, msg); }
};

#endif // LOGGER_H
