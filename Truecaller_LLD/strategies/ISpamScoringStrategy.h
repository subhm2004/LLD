#ifndef TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H
#define TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H

#include "../enums/ReportReason.h"

namespace truecaller_lld {

class ISpamScoringStrategy {
public:
    virtual ~ISpamScoringStrategy() = default;
    virtual int scoreDeltaFor(ReportReason reason) const = 0;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H
