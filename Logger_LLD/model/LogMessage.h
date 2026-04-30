#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <string>
#include "../enums/LogLevel.h"

class LogMessage {
private:
    LogLevel level;
    std::string message;
    long long timestamp;

public:
    LogMessage(LogLevel level, const std::string& message, long long timestamp)
        : level(level), message(message), timestamp(timestamp) {}

    LogLevel getLevel() const { return level; }
    const std::string& getMessage() const { return message; }
    long long getTimestamp() const { return timestamp; }
};

#endif // LOGMESSAGE_H
