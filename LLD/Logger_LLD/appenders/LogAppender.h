// ============================================================================
//  appenders/LogAppender.h  —  STRATEGY interface: log KAHAN jaaye (destination)
// ----------------------------------------------------------------------------
//  Ek log message ko KAHAN likhe — console? file? network? database? Ye bhi ek
//  badalne wala faisla hai, isliye interface ke peeche (Strategy pattern).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO ORTHOGONAL STRATEGIES: FORMAT (kaise) aur DESTINATION (kahan)      │
//  │                                                                          │
//  │     LogFormatter -> "kaise dikhe" (plain text / JSON)                     │
//  │     LogAppender  -> "kahan jaaye" (console / file)                        │
//  │                                                                          │
//  │  Dono ALAG interfaces hain, aur appender ek formatter ko HOLD karta hai   │
//  │  (ConsoleAppender/FileAppender me dekho). Isliye 2 formats x 2 destinations│
//  │  = 4 combinations bina naya code likhe (console+plain, console+json,      │
//  │  file+plain, file+json). Ye "appender formatter ko compose karta" wala    │
//  │  design BRIDGE pattern jaisa hai — do dimensions independently vary karte.│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ VIRTUAL DESTRUCTOR: appenders shared_ptr<LogAppender> me base pointer se
//     rakhe jaate — cleanup ke liye virtual destructor zaroori.
// ============================================================================
#ifndef LOGAPPENDER_H
#define LOGAPPENDER_H

#include "../model/LogMessage.h"

namespace logger_lld {

class LogAppender {
public:
    virtual ~LogAppender() = default;

    // Pure virtual — har concrete appender ko ye implement karna HI padega.
    // Message ko apni destination (console/file/...) pe likh do.
    virtual void append(const LogMessage& message) = 0;
};

}

#endif // LOGAPPENDER_H
