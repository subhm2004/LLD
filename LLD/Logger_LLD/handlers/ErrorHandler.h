// ============================================================================
//  handlers/ErrorHandler.h  —  Concrete CoR handler: sirf ERROR level
// ----------------------------------------------------------------------------
//  Chain me WarnHandler ke baad. Sirf ERROR handle karta; baaki FatalHandler ko forward.
//  (Demo me ERROR -> console + file dono jaata hai — do appenders subscribed.)
// ============================================================================
#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class ErrorHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::ERROR; // sirf ERROR mera
    }
};

}

#endif // ERRORHANDLER_H
