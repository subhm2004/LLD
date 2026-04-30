#ifndef CONSOLEAPPENDER_H
#define CONSOLEAPPENDER_H

#include "LogAppender.h"
#include "../formatter/LogFormatter.h"
#include "../model/LogMessage.h"
#include <memory>
#include <iostream>

class ConsoleAppender : public LogAppender {
private:
    std::shared_ptr<LogFormatter> formatter;

public:
    explicit ConsoleAppender(std::shared_ptr<LogFormatter> formatter)
        : formatter(std::move(formatter)) {}

    void append(const LogMessage& message) override {
        std::cout << formatter->format(message) << std::endl;
    }
};

#endif // CONSOLEAPPENDER_H
