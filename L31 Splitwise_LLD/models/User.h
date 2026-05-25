#ifndef SPLITWISE_LLD_MODELS_USER_H
#define SPLITWISE_LLD_MODELS_USER_H

#include <bits/stdc++.h>

#include "../observers/Observer.h"

using namespace std;

namespace splitwise_lld {
class User : public Observer {
public:
    static int nextUserId;
    string userId;
    string name;
    string email;
    map<string, double> balances;

    User(const string &name, const string &email) : name(name), email(email) {
        userId = "user" + to_string(++nextUserId);
    }

    void update(const string &message) override {
        cout << "[NOTIFICATION to " << name << "]: " << message << endl;
    }

    void updateBalance(const string &otherUserId, double amount) {
        balances[otherUserId] += amount;
        if (abs(balances[otherUserId]) < 0.01) balances.erase(otherUserId);
    }

    double getTotalOwed() const {
        double total = 0;
        for (const auto &balance : balances) if (balance.second < 0) total += abs(balance.second);
        return total;
    }
    double getTotalOwing() const {
        double total = 0;
        for (const auto &balance : balances) if (balance.second > 0) total += balance.second;
        return total;
    }
};

inline int User::nextUserId = 0;
}

#endif
