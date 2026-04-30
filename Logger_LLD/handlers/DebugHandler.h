#ifndef DEBUGHANDLER_H
#define DEBUGHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

class DebugHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::DEBUG;
    }
};

#endif // DEBUGHANDLER_H
