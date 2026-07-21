// ============================================================================
//  handlers/DebugHandler.h  —  Concrete CoR handler: sirf DEBUG level
// ----------------------------------------------------------------------------
//  Chain ka SIR (head) — LogHandlerConfiguration ise pehla banata hai. Sirf DEBUG
//  messages handle karta hai; baaki ko `next` (InfoHandler) ko forward kar deta
//  (base class LogHandler::handle me ye logic hai).
//
//  📌 Notice: ye class kitni CHHOTI hai — sirf `canHandle` override. Poora chain/
//     notify/thread-safety logic base LogHandler me hai (DRY). Naya level add
//     karna = aisi hi ek nayi chhoti class + chain me jodo. (Open/Closed.)
// ============================================================================
#ifndef DEBUGHANDLER_H
#define DEBUGHANDLER_H

#include "LogHandler.h"
#include "../enums/LogLevel.h"

namespace logger_lld {

class DebugHandler : public LogHandler {
protected:
    bool canHandle(LogLevel level) override {
        return level == LogLevel::DEBUG; // sirf DEBUG mera
    }
};

}

#endif // DEBUGHANDLER_H
