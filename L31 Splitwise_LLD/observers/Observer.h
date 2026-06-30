// observers/Observer.h — OBSERVER interface: group members ko expense/settlement
// events par notify karne ke liye. Group subject hai, users observers.
#ifndef SPLITWISE_LLD_OBSERVERS_OBSERVER_H
#define SPLITWISE_LLD_OBSERVERS_OBSERVER_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {
class Observer {
public:
    virtual void update(const string &message) = 0;
    virtual ~Observer() {}
};
}

#endif
