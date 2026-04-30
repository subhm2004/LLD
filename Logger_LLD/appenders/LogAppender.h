#ifndef LOGAPPENDER_H
#define LOGAPPENDER_H

#include "../model/LogMessage.h"

class LogAppender {
public:
    virtual ~LogAppender() = default;
    virtual void append(const LogMessage& message) = 0;
};

#endif // LOGAPPENDER_H
