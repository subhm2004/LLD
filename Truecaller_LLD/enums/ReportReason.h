#ifndef TRUECALLER_LLD_ENUMS_REPORTREASON_H
#define TRUECALLER_LLD_ENUMS_REPORTREASON_H

namespace truecaller_lld {

enum class ReportReason {
    TELEMARKETER,
    SCAM,
    ROBOT_CALL,
    HARASSMENT,
    OTHER
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_ENUMS_REPORTREASON_H
