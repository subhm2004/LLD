#ifndef FATALHANDLER_H
#define FATALHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

class FatalHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::FATAL;
    }
};

#endif // FATALHANDLER_H
