// ============================================================================
//  handlers/FatalHandler.h  —  Concrete CoR handler: sirf FATAL level (chain ka END)
// ----------------------------------------------------------------------------
//  Chain ka AAKHRI handler — iska `next` null hota hai. Sirf FATAL handle karta.
//  Agar message yahan tak pahuncha aur FATAL nahi hai (na kisi ne handle kiya),
//  to wo silently drop ho jaata (chain khatam).
// ============================================================================
#ifndef FATALHANDLER_H
#define FATALHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class FatalHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::FATAL; // sirf FATAL mera (chain ka end)
    }
};

}

#endif // FATALHANDLER_H
