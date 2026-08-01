// ============================================================================
//  handlers/InfoHandler.h  —  Concrete CoR handler: sirf INFO level
// ----------------------------------------------------------------------------
//  Chain me DebugHandler ke baad. Sirf INFO handle karta; baaki WarnHandler ko
//  forward. (Logic base LogHandler me — yahan sirf level batana.)
// ============================================================================
#ifndef INFOHANDLER_H
#define INFOHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class InfoHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::INFO; // sirf INFO mera
    }
};

}

#endif // INFOHANDLER_H
