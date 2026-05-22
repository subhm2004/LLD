#ifndef TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H
#define TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H

#include "ISpamScoringStrategy.h"

namespace truecaller_lld {

class DefaultSpamScoringStrategy : public ISpamScoringStrategy {
public:
    int scoreDeltaFor(ReportReason reason) const override {
        switch (reason) {
            case ReportReason::SCAM:
                return 35;
            case ReportReason::TELEMARKETER:
                return 25;
            case ReportReason::ROBOT_CALL:
                return 20;
            case ReportReason::HARASSMENT:
                return 30;
            case ReportReason::OTHER:
                return 15;
        }
        return 10;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H
