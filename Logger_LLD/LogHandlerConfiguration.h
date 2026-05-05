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

namespace logger_lld {

class LogHandlerConfiguration {
private:
    inline static std::shared_ptr<LogHandler> debug = nullptr;
    inline static std::shared_ptr<LogHandler> info = nullptr;
    inline static std::shared_ptr<LogHandler> warn = nullptr;
    inline static std::shared_ptr<LogHandler> error = nullptr;
    inline static std::shared_ptr<LogHandler> fatal = nullptr;
    static void ensureInitialized() {
        if (debug == nullptr) {
            build();
        }
    }

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
        ensureInitialized();
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

}

#endif // LOGHANDLERCONFIGURATION_H
