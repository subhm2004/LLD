#ifndef SPLITWISE_LLD_CORE_GROUP_H
#define SPLITWISE_LLD_CORE_GROUP_H

#include <bits/stdc++.h>

#include "../enums/SplitType.h"
#include "../factories/SplitFactory.h"
#include "../models/Expense.h"
#include "../models/User.h"
#include "../utils/DebtSimplifier.h"

using namespace std;

namespace splitwise_lld {
class Group {
private:
    User *getUserByuserId(const string &userId) {
        for (User *member : members) if (member->userId == userId) return member;
        return nullptr;
    }

    void resetGroupBalances() {
        for (User *member : members) {
            groupBalances[member->userId].clear();
        }
    }

    void applyExpenseToBalances(const Expense *expense) {
        for (const Split &split : expense->splits) {
            if (split.userId != expense->paidByUserId) {
                updateGroupBalance(expense->paidByUserId, split.userId, split.amount);
            }
        }
    }

    void recalculateBalancesFromExpenses() {
        resetGroupBalances();
        for (const auto &entry : groupExpenses) {
            applyExpenseToBalances(entry.second);
        }
    }

public:
    static int nextGroupId;
    string groupId;
    string name;
    vector<User *> members;
    map<string, Expense *> groupExpenses;
    map<string, map<string, double>> groupBalances;

    explicit Group(const string &name) : name(name) { groupId = "group" + to_string(++nextGroupId); }
    ~Group() { for (auto &pair : groupExpenses) delete pair.second; }

    void addMember(User *user) {
        members.push_back(user);
        groupBalances[user->userId] = {};
        cout << user->name << " added to group " << name << endl;
    }

    bool isMember(const string &userId) { return groupBalances.find(userId) != groupBalances.end(); }

    void notifyMembers(const string &message) {
        for (Observer *observer : members) observer->update(message);
    }

    void updateGroupBalance(const string &fromUserId, const string &toUserId, double amount) {
        groupBalances[fromUserId][toUserId] += amount;
        groupBalances[toUserId][fromUserId] -= amount;
        if (abs(groupBalances[fromUserId][toUserId]) < 0.01) groupBalances[fromUserId].erase(toUserId);
        if (abs(groupBalances[toUserId][fromUserId]) < 0.01) groupBalances[toUserId].erase(fromUserId);
    }

    bool canUserLeaveGroup(const string &userId) {
        if (!isMember(userId)) throw runtime_error("user is not a part of this group");
        for (auto &balance : groupBalances[userId]) if (abs(balance.second) > 0.01) return false;
        return true;
    }

    bool removeMember(const string &userId) {
        if (!canUserLeaveGroup(userId)) {
            cout << "\nUser not allowed to leave group without clearing expenses" << endl;
            return false;
        }
        for (User *user : members) {
            if (user->userId == userId) {
                members.erase(remove(members.begin(), members.end(), user), members.end());
                break;
            }
        }
        groupBalances.erase(userId);
        for (auto &memberBalance : groupBalances) memberBalance.second.erase(userId);
        return true;
    }

    string addExpense(string description, double amount, string paidByUserId, vector<string> involvedUsers,
                      SplitType splitType, const vector<double> &splitValues = {}) {
        if (!isMember(paidByUserId)) throw runtime_error("user is not a part of this group");
        for (const string &userId : involvedUsers) if (!isMember(userId)) throw runtime_error("involvedUsers invalid");

        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, involvedUsers, splitValues);

        Expense *expense = new Expense(description, amount, paidByUserId, splits, groupId);
        groupExpenses[expense->expenseId] = expense;

        applyExpenseToBalances(expense);
        string paidByName = getUserByuserId(paidByUserId)->name;
        notifyMembers("New expense added: " + description + " (Rs " + to_string(amount) + ")");
        cout << "Expense added to " << name << ": " << description << " (Rs " << amount << ") paid by " << paidByName << endl;
        return expense->expenseId;
    }

    bool editExpense(const string &expenseId, string description, double amount, string paidByUserId,
                     vector<string> involvedUsers, SplitType splitType, const vector<double> &splitValues = {}) {
        if (!groupExpenses.count(expenseId)) return false;
        if (!isMember(paidByUserId)) return false;
        for (const string &userId : involvedUsers) if (!isMember(userId)) return false;

        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, involvedUsers, splitValues);

        Expense *oldExpense = groupExpenses[expenseId];
        Expense *newExpense = new Expense(description, amount, paidByUserId, splits, groupId);
        newExpense->expenseId = expenseId;
        groupExpenses[expenseId] = newExpense;
        delete oldExpense;

        recalculateBalancesFromExpenses();
        notifyMembers("Expense edited: " + description + " (ID: " + expenseId + ")");
        return true;
    }

    bool deleteExpense(const string &expenseId) {
        auto it = groupExpenses.find(expenseId);
        if (it == groupExpenses.end()) return false;
        delete it->second;
        groupExpenses.erase(it);
        recalculateBalancesFromExpenses();
        notifyMembers("Expense deleted (ID: " + expenseId + ")");
        return true;
    }

    bool settlePayment(string fromUserId, string toUserId, double amount) {
        if (!isMember(fromUserId) || !isMember(toUserId)) return false;
        updateGroupBalance(fromUserId, toUserId, amount);
        string fromName = getUserByuserId(fromUserId)->name;
        string toName = getUserByuserId(toUserId)->name;
        notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " + to_string(amount));
        cout << "Settlement in " << name << ": " << fromName << " settled Rs " << amount << " with " << toName << endl;
        return true;
    }

    void showGroupBalances() {
        cout << "\n=== Group Balances for " << name << " ===" << endl;
        for (const auto &pair : groupBalances) {
            string memberId = pair.first;
            string memberName = getUserByuserId(memberId)->name;
            cout << memberName << "'s balances in group:" << endl;
            if (pair.second.empty()) cout << "  No outstanding balances" << endl;
            for (const auto &ub : pair.second) {
                string otherName = getUserByuserId(ub.first)->name;
                if (ub.second > 0) cout << "  " << otherName << " owes: Rs " << fixed << setprecision(2) << ub.second << endl;
                else cout << "  Owes " << otherName << ": Rs " << fixed << setprecision(2) << abs(ub.second) << endl;
            }
        }
    }

    void simplifyGroupDebts() {
        groupBalances = DebtSimplifier::simplifyDebts(groupBalances);
        cout << "\nDebts have been simplified for group: " << name << endl;
    }
};

inline int Group::nextGroupId = 0;
}

#endif
