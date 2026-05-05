#ifndef SPLITWISE_LLD_UTILS_DEBTSIMPLIFIER_H
#define SPLITWISE_LLD_UTILS_DEBTSIMPLIFIER_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {
class DebtSimplifier {
public:
    static map<string, map<string, double>> simplifyDebts(map<string, map<string, double>> groupBalances) {
        map<string, double> netAmounts;
        for (const auto &ub : groupBalances) netAmounts[ub.first] = 0;

        for (const auto &ub : groupBalances) {
            string creditorId = ub.first;
            for (const auto &b : ub.second) {
                string debtorId = b.first;
                double amount = b.second;
                if (amount > 0) {
                    netAmounts[creditorId] += amount;
                    netAmounts[debtorId] -= amount;
                }
            }
        }

        vector<pair<string, double>> creditors, debtors;
        for (const auto &net : netAmounts) {
            if (net.second > 0.01) creditors.push_back({net.first, net.second});
            else if (net.second < -0.01) debtors.push_back({net.first, -net.second});
        }

        sort(creditors.begin(), creditors.end(), [](const auto &a, const auto &b){ return a.second > b.second; });
        sort(debtors.begin(), debtors.end(), [](const auto &a, const auto &b){ return a.second > b.second; });

        map<string, map<string, double>> simplified;
        for (const auto &ub : groupBalances) simplified[ub.first] = {};

        size_t i = 0, j = 0;
        while (i < creditors.size() && j < debtors.size()) {
            string creditorId = creditors[i].first, debtorId = debtors[j].first;
            double settle = min(creditors[i].second, debtors[j].second);
            simplified[creditorId][debtorId] = settle;
            simplified[debtorId][creditorId] = -settle;
            creditors[i].second -= settle;
            debtors[j].second -= settle;
            if (creditors[i].second < 0.01) i++;
            if (debtors[j].second < 0.01) j++;
        }
        return simplified;
    }
};
}

#endif
