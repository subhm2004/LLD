// strategies/SplitStrategy.h — STRATEGY interface: amount ko members me kaise
// baatna. Concrete: Equal / Exact / Percentage. Expense flow same rehta hai,
// sirf math swap hoti hai.
#ifndef SPLITWISE_LLD_STRATEGIES_SPLITSTRATEGY_H
#define SPLITWISE_LLD_STRATEGIES_SPLITSTRATEGY_H

#include <bits/stdc++.h>

#include "../models/Split.h"

using namespace std;

namespace splitwise_lld {
class SplitStrategy {
public:
    virtual vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds,
                                         const vector<double> &values = {}) = 0;
    virtual ~SplitStrategy() {}
};

class EqualSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds, const vector<double> &) override {
        vector<Split> splits;
        double amountPerUser = totalAmount / userIds.size();
        for (const string &userId : userIds) splits.push_back(Split(userId, amountPerUser));
        return splits;
    }
};

class ExactSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double, const vector<string> &userIds, const vector<double> &values) override {
        vector<Split> splits;
        for (size_t i = 0; i < userIds.size(); i++) splits.push_back(Split(userIds[i], values[i]));
        return splits;
    }
};

class PercentageSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds, const vector<double> &values) override {
        vector<Split> splits;
        for (size_t i = 0; i < userIds.size(); i++) splits.push_back(Split(userIds[i], (totalAmount * values[i]) / 100.0));
        return splits;
    }
};
}

#endif
