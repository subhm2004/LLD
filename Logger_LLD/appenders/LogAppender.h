#ifndef LOGAPPENDER_H
#define LOGAPPENDER_H

#include "../model/LogMessage.h"

namespace logger_lld {

class LogAppender {
public:
    virtual ~LogAppender() = default;
    virtual void append(const LogMessage& message) = 0;
};

}

#endif // LOGAPPENDER_H
