// factories/SplitFactory.h — FACTORY: SplitType se sahi SplitStrategy (Equal/
// Exact/Percentage) bana ke deta hai. Creation logic ek jagah.
#ifndef SPLITWISE_LLD_FACTORIES_SPLITFACTORY_H
#define SPLITWISE_LLD_FACTORIES_SPLITFACTORY_H

#include <bits/stdc++.h>

#include "../enums/SplitType.h"
#include "../strategies/SplitStrategy.h"

using namespace std;

namespace splitwise_lld {
class SplitFactory {
public:
    static SplitStrategy *getSplitStrategy(SplitType type) {
        switch (type) {
            case SplitType::EQUAL: return new EqualSplit();
            case SplitType::EXACT: return new ExactSplit();
            case SplitType::PERCENTAGE: return new PercentageSplit();
            default: return new EqualSplit();
        }
    }
};
}

#endif
