// ============================================================================
//  formatter/JsonFormatter.h  —  CONCRETE STRATEGY: machine-readable JSON
// ----------------------------------------------------------------------------
//  Same log ko JSON object me badalta hai:
//     {"timestamp": "2026-05-25 14:30:00", "level": "ERROR", "message": "..."}
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY ka fayda LIVE: SAME LogMessage, ALAG output                 │
//  │                                                                          │
//  │  PlainTextFormatter aur JsonFormatter dono SAME `LogFormatter` interface  │
//  │  implement karte hain. Appender ko sirf `format()` chahiye — kaunsa       │
//  │  formatter laga hai, farak nahi. Console pe plain text, file me JSON —    │
//  │  ek line ka change (formatter swap), baaki system waisa hi.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 JSON kyun? Log-aggregation tools (ELK/Splunk/CloudWatch) JSON logs ko
//     seedha parse karke fields pe search/filter kar sakte hain ("level=ERROR
//     wale sab dikhao"). Plain text me ye mushkil (regex parsing). Production me
//     structured (JSON) logging common hai.
//
//  ⚠ Ye ek SIMPLE JSON builder hai — agar message me `"` ya `\` ho to output
//     invalid JSON ho sakta (escaping nahi hai). Real formatter special chars
//     escape karta. Interview-scope me ye simplification theek, par bolna accha.
// ============================================================================
#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include "LogFormatter.h"
#include "../model/LogMessage.h"
#include "../enums/LogLevel.h"
#include <string>
#include <ctime>

namespace logger_lld {

class JsonFormatter : public LogFormatter {
private:
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

    std::string formatTimestamp(long long epochMillis) const {
        std::time_t seconds = epochMillis / 1000;
        std::tm* tm_info = std::localtime(&seconds);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        return std::string(buf);
    }

public:
    // Strategy method: message ko JSON object string me badlo.
    std::string format(const LogMessage& message) override {
        return "{\"timestamp\": \"" + formatTimestamp(message.getTimestamp()) +
               "\", \"level\": \"" + levelToString(message.getLevel()) +
               "\", \"message\": \"" + message.getMessage() + "\"}"; // ⚠ no escaping (upar note)
    }
};

}

#endif // JSONFORMATTER_H
