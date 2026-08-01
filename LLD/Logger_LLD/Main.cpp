// ============================================================================
//  Main.cpp  —  Logger LLD ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye dikhata hai: (1) logger lo (Singleton), (2) alag-alag levels ke liye
//  alag appenders configure karo, (3) log karo — message apne aap sahi jagah
//  chala jaata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CONFIG: kaun sa level KAHAN jaaye                                     │
//  │     INFO  -> Console                                                      │
//  │     ERROR -> Console + File (do appenders — Observer pattern)             │
//  │                                                                          │
//  │  Phir:                                                                    │
//  │     logger.info(...)  -> InfoHandler match -> Console                     │
//  │     logger.error(...) -> ErrorHandler match -> Console AUR File           │
//  │                                                                          │
//  │  Client ko chain/handlers/formatters ka kuch nahi pata — bas             │
//  │  getInstance() + addAppenderForLevel() + info()/error(). (Facade-ish.)   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Include targeted (bits/stdc++.h nahi) — portable + saaf.
// ============================================================================
#include "Logger.h"
#include "LogHandlerConfiguration.h"
#include "appenders/ConsoleAppender.h"
#include "appenders/FileAppender.h"
#include "formatter/PlainTextFormatter.h"
#include "enums/LogLevel.h"
#include <memory>

using namespace logger_lld;

int main() {
    Logger& logger = Logger::getInstance(); // ⭐ Singleton lo (chain yahin ban jaati)

    // ---- INFO -> Console (plain text formatter) ----------------------------
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::INFO,
        std::make_shared<ConsoleAppender>(std::make_shared<PlainTextFormatter>())
    );

    // ---- ERROR -> Console AUR File (do appenders subscribe = Observer) ------
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::ERROR,
        std::make_shared<ConsoleAppender>(std::make_shared<PlainTextFormatter>())
    );
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::ERROR,
        std::make_shared<FileAppender>(std::make_shared<PlainTextFormatter>(), "logs.txt")
    );

    // ---- Log karo — routing apne aap ---------------------------------------
    logger.info("This is some key information"); // -> InfoHandler -> CONSOLE
    logger.error("Oh no! there's an error");     // -> ErrorHandler -> CONSOLE + FILE

    return 0;
}
