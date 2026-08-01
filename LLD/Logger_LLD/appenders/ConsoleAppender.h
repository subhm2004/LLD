// ============================================================================
//  appenders/ConsoleAppender.h  —  CONCRETE APPENDER: log ko console pe chhaapo
// ----------------------------------------------------------------------------
//  Simplest appender: message ko formatter se string me badlo, aur `std::cout`
//  pe print kar do.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ BRIDGE-like: appender ek FORMATTER ko HOLD karta hai                  │
//  │                                                                          │
//  │  `shared_ptr<LogFormatter> formatter` — construct karte waqt inject hota  │
//  │  hai. Isse ConsoleAppender ko pata nahi (aur farak nahi) ki format plain  │
//  │  text hai ya JSON — wo bas `formatter->format(message)` bulata hai aur    │
//  │  jo string mile use print karta hai.                                     │
//  │                                                                          │
//  │     new ConsoleAppender(make_shared<PlainTextFormatter>())  -> plain      │
//  │     new ConsoleAppender(make_shared<JsonFormatter>())       -> JSON       │
//  │  Same appender, alag formatter = alag output. Destination aur format      │
//  │  independently swappable (Bridge ka poora point).                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `std::move(formatter)` constructor me — shared_ptr ki refcount-bump copy
//     se bachte hain (member me seedha "cheen" lete hain). Chhoti efficiency.
// ============================================================================
#ifndef CONSOLEAPPENDER_H
#define CONSOLEAPPENDER_H

#include "LogAppender.h"
#include "../formatter/LogFormatter.h"
#include "../model/LogMessage.h"
#include <memory>
#include <iostream>

namespace logger_lld {

class ConsoleAppender : public LogAppender {
private:
    std::shared_ptr<LogFormatter> formatter; // ⭐ injected format strategy (Bridge)

public:
    explicit ConsoleAppender(std::shared_ptr<LogFormatter> formatter)
        : formatter(std::move(formatter)) {}

    // Format karo (jo bhi formatter laga hai) -> console pe print.
    void append(const LogMessage& message) override {
        std::cout << formatter->format(message) << std::endl;
    }
};

}

#endif // CONSOLEAPPENDER_H
