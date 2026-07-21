// ============================================================================
//  formatter/LogFormatter.h  —  STRATEGY PATTERN ka interface (kaise dikhe)
// ----------------------------------------------------------------------------
//  Ek log message ko STRING me kaise badle — ye ek BADALNE WALA faisla hai.
//  Plain text? JSON? XML? Har format alag. Isliye ise ek interface ke peeche
//  rakh diya (Strategy pattern).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — formatting algorithm pluggable banao               │
//  │                                                                          │
//  │  Appender (Console/File) ek `LogFormatter*` rakhta hai — usse pata nahi   │
//  │  andar plain text ban raha ya JSON. Wo bas `format(message)` bulata hai.  │
//  │  Naya format chahiye (XmlFormatter)? Bas nayi class banao jo ye interface │
//  │  implement kare — appender code untouched. (Open/Closed Principle.)       │
//  │                                                                          │
//  │  Aur "kaise dikhe" (format) "kahan jaaye" (appender) se ALAG ho gaya —    │
//  │  same message console pe plain, file me JSON ja sakta hai. (Bridge idea.) │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: appender `shared_ptr<LogFormatter>` me derived
//     object (PlainText/Json) rakhta hai base pointer ke through. Bina virtual
//     destructor ke cleanup adhoora reh jaata (UB).
// ============================================================================
#ifndef LOGFORMATTER_H
#define LOGFORMATTER_H

#include <string>
#include "../model/LogMessage.h"

namespace logger_lld {

class LogFormatter {
public:
    virtual ~LogFormatter() = default;

    // Pure virtual — har concrete formatter ko ye implement karna HI padega.
    // Ek LogMessage lo, uski string representation lauta do.
    virtual std::string format(const LogMessage& message) = 0;
};

}

#endif // LOGFORMATTER_H
