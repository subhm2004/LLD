// ============================================================================
//  enums/LogLevel.h  —  Log ki severity (kitni serious baat hai)
// ----------------------------------------------------------------------------
//  Kam se zyada serious ki taraf:
//     TRACE -> sabse detailed (step-by-step tracing) — sabse kam serious
//     DEBUG -> debugging info (developer ke liye)
//     INFO  -> normal information ("server started")
//     WARN  -> warning (galat nahi, par dhyaan do)
//     ERROR -> kuch fail hua (par app chal rahi hai)
//     FATAL -> app hi mar gayi — sabse serious
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ORDER = SEVERITY. Enum values ka order matter karta hai (TRACE=0 ...  │
//  │     FATAL=5). Real logging me isi order se "threshold" set hote hain      │
//  │     (jaise "WARN se upar hi dikhao"). Isliye ye order kabhi mat badalna.  │
//  │                                                                          │
//  │  📌 Is LLD me routing EXACT LEVEL se hoti hai (har level ka apna handler  │
//  │     + apne appenders) — "≥ threshold" wala classic model nahi. Yaani ek  │
//  │     ERROR message SIRF ErrorHandler ke appenders ko jaata hai (LogHandler│
//  │     ke `canHandle` me `==` dekho). TRACE ka koi handler nahi -> wo silently│
//  │     drop ho jaata (configure nahi hai).                                  │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LOGLEVEL_H
#define LOGLEVEL_H

namespace logger_lld {
enum class LogLevel {
    TRACE, // sabse detailed (is LLD me koi handler nahi — drop)
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL  // sabse serious
};
}

#endif // LOGLEVEL_H
