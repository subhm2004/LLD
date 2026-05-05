#ifndef LOGFORMATTER_H
#define LOGFORMATTER_H

#include <string>
#include "../model/LogMessage.h"

namespace logger_lld {

class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogMessage& message) = 0;
};

}

#endif // LOGFORMATTER_H
