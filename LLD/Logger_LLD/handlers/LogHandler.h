// ============================================================================
//  handlers/LogHandler.h  —  CHAIN OF RESPONSIBILITY + OBSERVER (do pattern!)
// ----------------------------------------------------------------------------
//  Ye is system ka dil hai. Do pattern ek jagah:
//    1. CHAIN OF RESPONSIBILITY — message chain me aage badhta hai jab tak sahi
//       handler na mile (level match).
//    2. OBSERVER — jab handler match kare, to us level ke saare subscribed
//       appenders (observers) ko notify karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CHAIN OF RESPONSIBILITY — "mera kaam nahi? agle ko de do"            │
//  │                                                                          │
//  │  Handlers ek chain me linked hain (LogHandlerConfiguration me):           │
//  │     Debug -> Info -> Warn -> Error -> Fatal                               │
//  │                                                                          │
//  │  Ek message (jaise ERROR) chain ke SIR (Debug) se shuru hota hai:         │
//  │     Debug: canHandle(ERROR)? (== DEBUG? NAHI) -> agle ko do               │
//  │     Info:  canHandle(ERROR)? (== INFO?  NAHI) -> agle ko do               │
//  │     Warn:  canHandle(ERROR)? (== WARN?  NAHI) -> agle ko do               │
//  │     Error: canHandle(ERROR)? (== ERROR? HAAN) -> notifyObservers ✅       │
//  │                                                                          │
//  │  ⚠ Ye EXACT match (`==`) hai — har handler SIRF apna level handle karta.  │
//  │     (Classic "≥ threshold" model nahi — is design me har level ke apne    │
//  │      alag appenders hote hain, isliye exact routing.) TRACE ka koi handler│
//  │      nahi -> chain ke end tak jaake silently drop.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OBSERVER — ek level ke multiple appenders (subscribers)               │
//  │                                                                          │
//  │  Har handler ke paas `appenders` list hai (observers). `subscribe()` se   │
//  │  add hote hain (jaise ERROR -> console AUR file). `notifyObservers()` un  │
//  │  sabko message bhej deta hai. Ek level pe kitne bhi output ho sakte —     │
//  │  aur naya add karna sirf subscribe() (handler code untouched).            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ THREAD-SAFE notify — "copy-then-notify" pattern (nazuk detail)        │
//  │                                                                          │
//  │  notifyObservers me pehle LOCK leke appenders ki ek LOCAL COPY banate     │
//  │  hain, phir LOCK CHHOD ke us copy pe iterate karte hain (append bulate).  │
//  │                                                                          │
//  │  Kyun? Kyunki `append()` slow ho sakta (file/network I/O). Agar lock      │
//  │  HOLD karke append karte, to koi doosra thread subscribe() pe atak jaata  │
//  │  (aur worse: agar append ke andar dobara subscribe ho to deadlock). Copy  │
//  │  banake lock jaldi chhod dena = kam contention, koi re-entrancy issue     │
//  │  nahi. (Java ka CopyOnWriteArrayList isi soch pe kaam karta.)             │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "../model/LogMessage.h"
#include "../enums/LogLevel.h"
#include "../appenders/LogAppender.h"
#include <vector>
#include <memory>
#include <mutex>

namespace logger_lld {

class LogHandler {
protected:
    std::shared_ptr<LogHandler> next;                      // chain me agla handler (CoR)
    std::vector<std::shared_ptr<LogAppender>> appenders;   // is level ke observers
    std::mutex appenderMtx;                                // appenders list ko guard karta

public:
    virtual ~LogHandler() = default;

    // Chain me agla handler set karo (CoR link banana).
    void setNext(std::shared_ptr<LogHandler> nextHandler) {
        next = std::move(nextHandler);
    }

    // Is level ke liye ek appender subscribe karo (Observer register).
    void subscribe(std::shared_ptr<LogAppender> appender) {
        std::lock_guard<std::mutex> lock(appenderMtx);
        appenders.push_back(std::move(appender));
    }

    // ⭐ Saare subscribed appenders ko message bhejo (copy-then-notify — upar note).
    void notifyObservers(const LogMessage& message) {
        std::vector<std::shared_ptr<LogAppender>> localAppenders;
        {
            std::lock_guard<std::mutex> lock(appenderMtx);
            localAppenders = appenders; // LOCK ke andar sirf COPY
        } // <- lock RELEASE (append LOCK ke bahar hoga)
        for (auto& appender : localAppenders) {
            appender->append(message); // slow I/O — lock hold kiye bina
        }
    }

    // ⭐ CoR ka dil: mera level match? -> handle (notify). Nahi? -> agle ko do.
    void handle(const LogMessage& message) {
        if (canHandle(message.getLevel())) {
            notifyObservers(message);          // mera kaam -> appenders ko bhejo
        } else if (next != nullptr) {
            next->handle(message);             // mera nahi -> chain me aage
        }
        // (na match na next -> message silently drop, jaise TRACE)
    }

protected:
    // Har concrete handler batata hai "main kaunsa level handle karta hoon".
    virtual bool canHandle(LogLevel level) = 0;
};

}

#endif // LOGHANDLER_H
