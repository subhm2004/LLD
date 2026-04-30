#ifndef FILEAPPENDER_H
#define FILEAPPENDER_H

#include "LogAppender.h"
#include "../formatter/LogFormatter.h"
#include "../model/LogMessage.h"
#include <memory>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <iostream>

class FileAppender : public LogAppender {
private:
    std::shared_ptr<LogFormatter> formatter;
    std::ofstream fileStream;
    std::mutex mtx; // for thread safety (like synchronized in Java)

public:
    FileAppender(std::shared_ptr<LogFormatter> formatter, const std::string& fileName)
        : formatter(std::move(formatter)) {
        fileStream.open(fileName, std::ios::app); // append mode
        if (!fileStream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + fileName);
        }
    }

    ~FileAppender() {
        close();
    }

    void append(const LogMessage& message) override {
        std::lock_guard<std::mutex> lock(mtx); // synchronized block
        if (fileStream.is_open()) {
            fileStream << formatter->format(message) << std::endl;
            fileStream.flush();
        }
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
};

#endif // FILEAPPENDER_H
