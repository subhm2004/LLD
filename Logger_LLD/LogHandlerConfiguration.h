#ifndef LOGHANDLERCONFIGURATION_H
#define LOGHANDLERCONFIGURATION_H

#include "handlers/LogHandler.h"
#include "handlers/DebugHandler.h"
#include "handlers/InfoHandler.h"
#include "handlers/WarnHandler.h"
#include "handlers/ErrorHandler.h"
#include "handlers/FatalHandler.h"
#include "appenders/LogAppender.h"
#include "enums/LogLevel.h"
#include <memory>

class LogHandlerConfiguration {
private:
    static std::shared_ptr<LogHandler> debug;
    static std::shared_ptr<LogHandler> info;
    static std::shared_ptr<LogHandler> warn;
    static std::shared_ptr<LogHandler> error;
    static std::shared_ptr<LogHandler> fatal;

public:
    static std::shared_ptr<LogHandler> build() {
        debug = std::make_shared<DebugHandler>();
        info  = std::make_shared<InfoHandler>();
        warn  = std::make_shared<WarnHandler>();
        error = std::make_shared<ErrorHandler>();
        fatal = std::make_shared<FatalHandler>();

        // Chain of responsibility setup
        debug->setNext(info);
        info->setNext(warn);
        warn->setNext(error);
        error->setNext(fatal);

        return debug;
    }

    static void addAppenderForLevel(LogLevel level, std::shared_ptr<LogAppender> appender) {
        switch (level) {
            case LogLevel::DEBUG: debug->subscribe(appender); break;
            case LogLevel::INFO:  info->subscribe(appender);  break;
            case LogLevel::WARN:  warn->subscribe(appender);  break;
            case LogLevel::ERROR: error->subscribe(appender); break;
            case LogLevel::FATAL: fatal->subscribe(appender); break;
            default: break;
        }
    }
};

// Static member definitions
std::shared_ptr<LogHandler> LogHandlerConfiguration::debug = nullptr;
std::shared_ptr<LogHandler> LogHandlerConfiguration::info  = nullptr;
std::shared_ptr<LogHandler> LogHandlerConfiguration::warn  = nullptr;
std::shared_ptr<LogHandler> LogHandlerConfiguration::error = nullptr;
std::shared_ptr<LogHandler> LogHandlerConfiguration::fatal = nullptr;

#endif // LOGHANDLERCONFIGURATION_H
