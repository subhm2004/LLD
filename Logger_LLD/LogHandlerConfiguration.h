// ============================================================================
//  LogHandlerConfiguration.h  —  Chain banao + har level ke appenders configure
// ----------------------------------------------------------------------------
//  Ye class do kaam karti hai:
//    1. build()               -> handlers ki CHAIN wire karo (Debug->...->Fatal)
//    2. addAppenderForLevel()  -> ek level ke handler pe appender subscribe karo
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CHAIN SETUP (Chain of Responsibility ka wiring)                       │
//  │                                                                          │
//  │     debug -> info -> warn -> error -> fatal                               │
//  │     setNext() se linked, aur build() chain ka SIR (debug) return karta.   │
//  │     Logger isi head se `handle()` shuru karta hai.                        │
//  │                                                                          │
//  │  ⭐ Handlers `inline static` (shared, ek hi copy) — kyunki chain aur uske │
//  │     appender-subscriptions POORE app me EK hone chahiye (Logger bhi isi   │
//  │     chain ko use karta, main.cpp bhi isi pe subscribe karta). Static se   │
//  │     dono same instances share karte hain.                                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 IDEMPOTENT build() FIX — double-build se subscriptions bachao        │
//  │                                                                          │
//  │  PROBLEM: build() har baar SAARE handlers ko NAYE sire se banata tha      │
//  │  (make_shared). Do jagah se call hota hai:                               │
//  │     - Logger ka constructor -> build()                                    │
//  │     - addAppenderForLevel -> ensureInitialized() -> build() (agar null)   │
//  │  Agar koi appenders configure karne ke BAAD `Logger::getInstance()`       │
//  │  pehli baar bulaye, to Logger ka build() handlers ko DOBARA bana deta ->  │
//  │  pehle subscribe kiye appenders KHO jaate (naye handlers khaali hote). 😱 │
//  │                                                                          │
//  │  ✅ FIX: build() ab IDEMPOTENT hai — agar chain pehle se bani hai (debug  │
//  │  != nullptr), to naya banane ke bajaye MAUJOODA head return karta. Ab     │
//  │  chahe koi bhi pehle call kare, chain EK hi baar banti, subscriptions safe.│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LOGHANDLERCONFIGURATION_H
#define LOGHANDLERCONFIGURATION_H

#include "handlers/LogHandler.h"
#include "handlers/DebugHandler.h"
#include "handlers/InfoHandler.h"
#include "handlers/WarnHandler.h"
#include "handlers/ErrorHandler.h"
#include "handlers/FatalHandler.h"
#include "appenders/LogAppender.h"
#include "enums/LogLevel.h"
#include <memory>

namespace logger_lld {

class LogHandlerConfiguration {
private:
    // Shared static handlers — poore app me ek hi chain (upar note).
    inline static std::shared_ptr<LogHandler> debug = nullptr;
    inline static std::shared_ptr<LogHandler> info = nullptr;
    inline static std::shared_ptr<LogHandler> warn = nullptr;
    inline static std::shared_ptr<LogHandler> error = nullptr;
    inline static std::shared_ptr<LogHandler> fatal = nullptr;

    static void ensureInitialized() {
        if (debug == nullptr) {
            build();
        }
    }

public:
    // ⭐ Chain banao (idempotent — pehle se bani hai to wahi lauta do).
    static std::shared_ptr<LogHandler> build() {
        if (debug != nullptr) {
            return debug; // 🐛 FIX: dobara mat banao — subscriptions safe rahein
        }

        debug = std::make_shared<DebugHandler>();
        info  = std::make_shared<InfoHandler>();
        warn  = std::make_shared<WarnHandler>();
        error = std::make_shared<ErrorHandler>();
        fatal = std::make_shared<FatalHandler>();

        // Chain of Responsibility wiring: debug -> info -> warn -> error -> fatal
        debug->setNext(info);
        info->setNext(warn);
        warn->setNext(error);
        error->setNext(fatal);

        return debug; // chain ka SIR (yahin se handle() shuru hota)
    }

    // Ek level ke handler pe ek appender subscribe karo (Observer register).
    // Ek level pe kai appenders ho sakte (jaise ERROR -> console + file).
    static void addAppenderForLevel(LogLevel level, std::shared_ptr<LogAppender> appender) {
        ensureInitialized(); // chain zaroor bani ho (nahi to bana lo)
        switch (level) {
            case LogLevel::DEBUG: debug->subscribe(appender); break;
            case LogLevel::INFO:  info->subscribe(appender);  break;
            case LogLevel::WARN:  warn->subscribe(appender);  break;
            case LogLevel::ERROR: error->subscribe(appender); break;
            case LogLevel::FATAL: fatal->subscribe(appender); break;
            default: break; // TRACE ka koi handler nahi (drop)
        }
    }
};

}

#endif // LOGHANDLERCONFIGURATION_H
