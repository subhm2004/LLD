#ifndef SPLITWISE_LLD_MODELS_EXPENSE_H
#define SPLITWISE_LLD_MODELS_EXPENSE_H

#include <bits/stdc++.h>

#include "Split.h"

using namespace std;

namespace splitwise_lld {
class Expense {
public:
    static int nextExpenseId;
    string expenseId;
    string description;
    double totalAmount;
    string paidByUserId;
    vector<Split> splits;
    string groupId;

    Expense(const string &description, double amount, const string &paidByUserId, vector<Split> &splits,
            const string &groupId = "")
        : description(description), totalAmount(amount), paidByUserId(paidByUserId), splits(splits), groupId(groupId) {
        expenseId = "expense" + to_string(++nextExpenseId);
    }
};

inline int Expense::nextExpenseId = 0;
}

#endif
