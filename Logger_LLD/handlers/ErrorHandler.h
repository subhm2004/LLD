#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

class ErrorHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::ERROR;
    }
};

#endif // ERRORHANDLER_H
