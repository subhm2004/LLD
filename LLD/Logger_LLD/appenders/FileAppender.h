// ============================================================================
//  appenders/FileAppender.h  —  CONCRETE APPENDER: log ko FILE me likho
// ----------------------------------------------------------------------------
//  ConsoleAppender jaisa hi (formatter hold karta hai — Bridge), par output
//  console ke bajaye ek file me jaata hai (append mode). Aur ye THREAD-SAFE hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ THREAD-SAFETY: mutex kyun zaroori hai (Console me kyun nahi)          │
//  │                                                                          │
//  │  Do threads ek saath SAME file me likhein to output aapas me mix/corrupt  │
//  │  ho sakta (aadhi line ek ki, aadhi doosre ki). `mtx` (mutex) ensure karta │
//  │  ki ek waqt me ek hi thread file me likhe (line-by-line atomic).          │
//  │                                                                          │
//  │  (std::cout thoda-bahut apne aap synchronized hota individual inserts pe, │
//  │   par file stream nahi — isliye FileAppender me explicit mutex.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ RAII: file constructor me KHULTI hai, destructor me BAND (close()). Isse
//     file handle leak nahi hota — object khatam = file band. Aur `ios::app`
//     (append mode) se purane logs mit-te nahi, naye neeche add hote hain.
//
//  ⚠ Fail-fast: agar file khul hi nahi paayi (permission/path issue), constructor
//     turant exception phenkta hai — ek "invalid FileAppender" object banta hi nahi.
// ============================================================================
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

namespace logger_lld {

class FileAppender : public LogAppender {
private:
    std::shared_ptr<LogFormatter> formatter; // injected format strategy (Bridge)
    std::ofstream fileStream;                 // khuli hui log file
    std::mutex mtx;                           // ⭐ ek waqt me ek thread hi likhe

public:
    FileAppender(std::shared_ptr<LogFormatter> formatter, const std::string& fileName)
        : formatter(std::move(formatter)) {
        fileStream.open(fileName, std::ios::app); // append mode (purane logs safe)
        if (!fileStream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + fileName); // fail-fast
        }
    }

    // Destructor -> file band (RAII: object khatam = handle release).
    ~FileAppender() {
        close();
    }

    // Format karo -> file me likho (mutex ke andar = thread-safe).
    void append(const LogMessage& message) override {
        std::lock_guard<std::mutex> lock(mtx); // ek waqt me ek writer
        if (fileStream.is_open()) {
            fileStream << formatter->format(message) << std::endl;
        }
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
};

}

#endif // FILEAPPENDER_H
