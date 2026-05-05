#ifndef SPLITWISE_LLD_MODELS_SPLIT_H
#define SPLITWISE_LLD_MODELS_SPLIT_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {
class Split {
public:
    string userId;
    double amount;
    Split(const string &userId, double amount) : userId(userId), amount(amount) {}
};
}

#endif
