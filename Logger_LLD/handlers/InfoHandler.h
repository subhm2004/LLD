#ifndef INFOHANDLER_H
#define INFOHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

class InfoHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::INFO;
    }
};

#endif // INFOHANDLER_H
