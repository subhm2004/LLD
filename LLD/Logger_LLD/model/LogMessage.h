// ============================================================================
//  model/LogMessage.h  —  Ek log entry ka data (immutable)
// ----------------------------------------------------------------------------
//  Jab bhi koi logger.info("...") bulata hai, ek LogMessage banta hai. Isme 3
//  cheezein: level (kitni serious), message (asli text), aur timestamp (kab).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IMMUTABLE — sirf getters, koi setter                                  │
//  │                                                                          │
//  │  Ek baar log message ban gaya, uska data badalna nahi chahiye — wo ek     │
//  │  "record" hai jo handlers/formatters/appenders ke beech ghoomta hai. Agar │
//  │  koi beech me use badal de to alag appenders ko alag data dikhta (bug).   │
//  │  Immutable rakhne se ye poore pipeline me safe travel karta hai — har     │
//  │  jagah same message. (Multi-threaded me bhi immutable data race-free hai.)│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `timestamp` epoch MILLISECONDS me hai (Logger `currentTimeMillis()` se
//     bharta hai). Formatter isse readable date-time me badalta hai.
// ============================================================================
#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <string>
#include "../enums/LogLevel.h"

namespace logger_lld {

class LogMessage {
private:
    LogLevel level;        // kitni serious (TRACE..FATAL)
    std::string message;   // asli log text
    long long timestamp;   // kab bana (epoch millis)

public:
    LogMessage(LogLevel level, const std::string& message, long long timestamp)
        : level(level), message(message), timestamp(timestamp) {}

    // Sirf getters (immutable) — data ban ne ke baad badalta nahi.
    LogLevel getLevel() const { return level; }
    const std::string& getMessage() const { return message; }
    long long getTimestamp() const { return timestamp; }
};

}

#endif // LOGMESSAGE_H
