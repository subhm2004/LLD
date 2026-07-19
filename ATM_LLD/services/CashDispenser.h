#ifndef ATM_LLD_SERVICES_CASHDISPENSER_H
#define ATM_LLD_SERVICES_CASHDISPENSER_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

/**
 * @class CashDispenser
 * @brief ATM cash vault aur notes distribution (dispensing) algorithm ko manage karti hai.
 * 
 * greedy design: high denominations (500) se start karke low notes (100) tak ka use karke 
 * requested amount fulfill karne ki koshish ki jati hai.
 */
class CashDispenser {
public:
    // Constructor jo ATM notes vault initialize karta hai default counts ke sath (e.g. 500, 200, 100 notes inventory).
    CashDispenser() {
        notes_[500] = 20; // 500 ke 20 notes = 10,000 INR
        notes_[200] = 30; // 200 ke 30 notes = 6,000 INR
        notes_[100] = 50; // 100 ke 50 notes = 5,000 INR
    }

    // Check karta hai ki kya requested amount notes split up karke distribute kiya ja sakta hai ya nahi.
    bool canDispense(int amount) const {
        // ATM normal constraints check: Amount negative ya 100 multiples se bahar nahi hona chahiye.
        if (amount <= 0 || amount % 100 != 0) {
            return false;
        }

        int remaining = amount;
        // Temporary notes map reference loop checking ke liye create kiya.
        map<int, int, greater<int>> temp = notes_;
        for (auto &entry : temp) {
            const int denom = entry.first;     // Denomination like 500, 200
            const int available = entry.second; // Quantity of notes
            const int use = min(remaining / denom, available); // required notes limit selection
            remaining -= use * denom;
        }
        return remaining == 0; // complete check matching notes count checks success.
    }

    // Physical transaction notes count deduct karke dispensed sheet return karta hai.
    map<int, int> dispense(int amount) {
        // Safe validation check.
        if (!canDispense(amount)) {
            throw runtime_error("ATM canDispense validation failed. Requested amount balance distribution combinations not possible.");
        }

        int remaining = amount;
        map<int, int> dispensed;
        // High to Low notes iteration matching note deductions.
        for (auto &entry : notes_) {
            const int denom = entry.first;
            int &available = entry.second;
            const int use = min(remaining / denom, available);
            if (use > 0) {
                dispensed[denom] = use;
                available -= use;        // Stored quantity count logic update
                remaining -= use * denom; // Remaining value balance update
            }
        }
        return dispensed;
    }

private:
    // Notes inventory store: high value denomination sorting order apply (greater<int> comparison rule).
    map<int, int, greater<int>> notes_;
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_CASHDISPENSER_H
