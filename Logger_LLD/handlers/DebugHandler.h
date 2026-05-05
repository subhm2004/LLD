#ifndef DEBUGHANDLER_H
#define DEBUGHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class DebugHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::DEBUG;
    }
};

}

#endif // DEBUGHANDLER_H
