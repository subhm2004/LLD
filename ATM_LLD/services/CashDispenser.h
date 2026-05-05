#ifndef ATM_LLD_SERVICES_CASHDISPENSER_H
#define ATM_LLD_SERVICES_CASHDISPENSER_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

class CashDispenser {
public:
    CashDispenser() {
        notes_[500] = 20;
        notes_[200] = 30;
        notes_[100] = 50;
    }

    bool canDispense(int amount) const {
        if (amount <= 0 || amount % 100 != 0) {
            return false;
        }

        int remaining = amount;
        map<int, int, greater<int>> temp = notes_;
        for (auto &entry : temp) {
            const int denom = entry.first;
            const int available = entry.second;
            const int use = min(remaining / denom, available);
            remaining -= use * denom;
        }
        return remaining == 0;
    }

    map<int, int> dispense(int amount) {
        if (!canDispense(amount)) {
            throw runtime_error("ATM cannot dispense requested amount");
        }

        int remaining = amount;
        map<int, int> dispensed;
        for (auto &entry : notes_) {
            const int denom = entry.first;
            int &available = entry.second;
            const int use = min(remaining / denom, available);
            if (use > 0) {
                dispensed[denom] = use;
                available -= use;
                remaining -= use * denom;
            }
        }
        return dispensed;
    }

private:
    map<int, int, greater<int>> notes_;
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_CASHDISPENSER_H
