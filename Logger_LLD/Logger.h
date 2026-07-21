// ============================================================================
//  Logger.h  —  SINGLETON: poore app ka ek hi global logger (entry point)
// ----------------------------------------------------------------------------
//  Client ke liye "logger" yahi hai. `Logger::getInstance().info("...")`. Andar
//  handler chain hoti hai; log() message ko chain ke sir se handle karwaata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SINGLETON PATTERN — poore app me EK hi logger instance                │
//  │                                                                          │
//  │  Logging ek "global service" hai — har jagah se same logger chahiye (na   │
//  │  ki har class apna banaye). Singleton ye ensure karta hai: ek hi instance,│
//  │  ek global access point (`getInstance()`).                               │
//  │                                                                          │
//  │  KAISE enforce hota:                                                      │
//  │    - constructor PRIVATE (bahar se `Logger l;` nahi bana sakte)          │
//  │    - copy ctor + assignment DELETE (copy karke doosra instance nahi)     │
//  │    - `getInstance()` ek `static local` instance return karta            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MEYERS SINGLETON — `static Logger instance;` function ke andar        │
//  │                                                                          │
//  │  `static Logger instance;` (getInstance ke andar) — ye C++ ka sabse saaf  │
//  │  aur THREAD-SAFE singleton hai. C++11+ guarantee deta hai: static local   │
//  │  variable ka initialization THREAD-SAFE hota hai (do threads ek saath     │
//  │  getInstance bulayein to bhi instance sirf EK baar, safely banta). Koi    │
//  │  manual mutex/double-checked-locking ki zaroorat nahi. Aur "lazy" bhi —   │
//  │  pehli getInstance() call pe hi banta (program start pe nahi).           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `log()` message banata hai (level + text + abhi ka timestamp) aur chain ke
//     head ko de deta hai. `debug/info/warn/error/fatal` bas convenience wrappers
//     hain (sahi level ke saath log() bulate).
// ============================================================================
#ifndef LOGGER_H
#define LOGGER_H

#include "enums/LogLevel.h"
#include "handlers/LogHandler.h"
#include "model/LogMessage.h"
#include "LogHandlerConfiguration.h"
#include <memory>
#include <string>
#include <chrono>

namespace logger_lld {

class Logger {
private:
    std::shared_ptr<LogHandler> handlerChain; // chain ka SIR (yahin se handle shuru)

    // ⭐ PRIVATE constructor — bahar se banaya nahi ja sakta (Singleton enforce).
    Logger() {
        handlerChain = LogHandlerConfiguration::build(); // idempotent (config note dekho)
    }

    // Abhi ka time epoch MILLISECONDS me (LogMessage ke timestamp ke liye).
    static long long currentTimeMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    // ⭐ Copy BAND — Singleton ki doosri copy nahi ban sakti.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ⭐ Meyers Singleton — thread-safe lazy single instance (upar note).
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Asli log entry point: message banao + chain ko de do.
    void log(LogLevel level, const std::string& message) {
        LogMessage msg(level, message, currentTimeMillis());
        handlerChain->handle(msg); // chain ke sir se routing shuru
    }

    // Convenience wrappers (sahi level ke saath log() bulate).
    void debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
    void info(const std::string& msg)  { log(LogLevel::INFO,  msg); }
    void warn(const std::string& msg)  { log(LogLevel::WARN,  msg); }
    void error(const std::string& msg) { log(LogLevel::ERROR, msg); }
    void fatal(const std::string& msg) { log(LogLevel::FATAL, msg); }
};

}

#endif // LOGGER_H
