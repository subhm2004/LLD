// ============================================================================
//  handlers/WarnHandler.h  —  Concrete CoR handler: sirf WARN level
// ----------------------------------------------------------------------------
//  Chain me InfoHandler ke baad. Sirf WARN handle karta; baaki ErrorHandler ko forward.
// ============================================================================
#ifndef WARNHANDLER_H
#define WARNHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class WarnHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::WARN; // sirf WARN mera
    }
};

}

#endif // WARNHANDLER_H
