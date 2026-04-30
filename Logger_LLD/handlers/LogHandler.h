#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "../model/LogMessage.h"
#include "../enums/LogLevel.h"
#include "../appenders/LogAppender.h"
#include <vector>
#include <memory>
#include <mutex>

class LogHandler {
protected:
    std::shared_ptr<LogHandler> next;
    // CopyOnWriteArrayList equivalent - using mutex for thread safety
    std::vector<std::shared_ptr<LogAppender>> appenders;
    std::mutex appenderMtx;

public:
    virtual ~LogHandler() = default;

    void setNext(std::shared_ptr<LogHandler> nextHandler) {
        next = std::move(nextHandler);
    }

    void subscribe(std::shared_ptr<LogAppender> appender) {
        std::lock_guard<std::mutex> lock(appenderMtx);
        appenders.push_back(std::move(appender));
    }

    void notifyObservers(const LogMessage& message) {
        std::lock_guard<std::mutex> lock(appenderMtx);
        for (auto& appender : appenders) {
            appender->append(message);
        }
    }

    void handle(const LogMessage& message) {
        if (canHandle(message.getLevel())) {
            notifyObservers(message);
        } else if (next != nullptr) {
            next->handle(message);
        }
    }

protected:
    virtual bool canHandle(LogLevel level) = 0;
};

#endif // LOGHANDLER_H
