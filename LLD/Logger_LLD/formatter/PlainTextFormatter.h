// ============================================================================
//  formatter/PlainTextFormatter.h  —  CONCRETE STRATEGY: human-readable line
// ----------------------------------------------------------------------------
//  Log ko ek saaf, aadmi-ke-padhne-layak line me badalta hai:
//     2026-05-25 14:30:00 [ERROR] - Oh no! there's an error
//
//  Format: <timestamp> [<LEVEL>] - <message>
//
//  📌 Ye default formatter hai (console/file logs ke liye sabse aam). JSON
//     wala (JsonFormatter) machine-parsing/log-aggregation ke liye behtar.
//
//  ⚠ `std::localtime` THREAD-SAFE NAHI hai (ek shared internal buffer use karta,
//     aur us `tm*` ko turant strftime me use kar lete hain). Single-threaded ya
//     kam-load me theek, par heavy multi-threaded logging me `localtime_r`
//     (thread-safe variant) use karna chahiye. Interview me ye bolna accha point.
//     (Levels wala thread-safety LogHandler/FileAppender me hai; ye formatter
//      ki chhoti caveat hai.)
// ============================================================================
#ifndef PLAINTEXTFORMATTER_H
#define PLAINTEXTFORMATTER_H

#include "LogFormatter.h"
#include "../model/LogMessage.h"
#include "../enums/LogLevel.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace logger_lld {

class PlainTextFormatter : public LogFormatter {
private:
    // LogLevel enum -> readable string ("ERROR", "INFO"...).
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default:              return "UNKNOWN";
        }
    }

    // epoch millis -> "YYYY-MM-DD HH:MM:SS" (readable).
    std::string formatTimestamp(long long epochMillis) const {
        std::time_t seconds = epochMillis / 1000;      // millis -> seconds
        std::tm* tm_info = std::localtime(&seconds);   // ⚠ localtime not thread-safe (upar note)
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        return std::string(buf);
    }

public:
    // Strategy method: message ko plain-text line me badlo.
    std::string format(const LogMessage& message) override {
        return formatTimestamp(message.getTimestamp())
               + " [" + levelToString(message.getLevel()) + "] - "
               + message.getMessage();
    }
};

}

#endif // PLAINTEXTFORMATTER_H
