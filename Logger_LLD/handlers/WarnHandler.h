#ifndef WARNHANDLER_H
#define WARNHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

class WarnHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::WARN;
    }
};

#endif // WARNHANDLER_H
