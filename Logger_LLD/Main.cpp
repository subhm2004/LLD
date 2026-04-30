#include "Logger.h"
#include "LogHandlerConfiguration.h"
#include "appenders/ConsoleAppender.h"
#include "appenders/FileAppender.h"
#include "formatter/PlainTextFormatter.h"
#include "enums/LogLevel.h"
#include <memory>

int main() {
    Logger& logger = Logger::getInstance();

    // INFO -> Console
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::INFO,
        std::make_shared<ConsoleAppender>(std::make_shared<PlainTextFormatter>())
    );

    // ERROR -> Console
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::ERROR,
        std::make_shared<ConsoleAppender>(std::make_shared<PlainTextFormatter>())
    );

    // ERROR -> File
    LogHandlerConfiguration::addAppenderForLevel(
        LogLevel::ERROR,
        std::make_shared<FileAppender>(std::make_shared<PlainTextFormatter>(), "logs.txt")
    );

    // Usage
    logger.info("This is some key information"); // CONSOLE
    logger.error("Oh no! there's an error");     // CONSOLE + FILE

    return 0;
}
