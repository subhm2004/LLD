#ifndef SPLITWISE_LLD_CORE_SPLITWISE_H
#define SPLITWISE_LLD_CORE_SPLITWISE_H

#include <bits/stdc++.h>

#include "Group.h"

using namespace std;

namespace splitwise_lld {
class Splitwise {
private:
    map<string, User *> users;
    map<string, Group *> groups;
    map<string, Expense *> expenses;
    static Splitwise *instance;
    Splitwise() {}

public:
    static Splitwise *getInstance() {
        if (instance == nullptr) instance = new Splitwise();
        return instance;
    }

    User *createUser(string name, string email) {
        User *user = new User(name, email);
        users[user->userId] = user;
        return user;
    }
    Group *createGroup(const string &name) {
        Group *group = new Group(name);
        groups[group->groupId] = group;
        return group;
    }
    User *getUser(const string &userId) { return users.count(userId) ? users[userId] : nullptr; }
    Group *getGroup(const string &groupId) { return groups.count(groupId) ? groups[groupId] : nullptr; }

    void addUserToGroup(const string &userId, const string &groupId) {
        User *user = getUser(userId);
        Group *group = getGroup(groupId);
        if (user && group) group->addMember(user);
    }

    bool removeUserFromGroup(const string &userId, const string &groupId) {
        Group *group = getGroup(groupId);
        User *user = getUser(userId);
        if (!group || !user) return false;
        bool removed = group->removeMember(userId);
        if (removed) cout << user->name << " successfully left " << group->name << endl;
        return removed;
    }

    string addExpenseToGroup(string groupId, string description, double amount, string paidByUserId,
                             vector<string> involvedUsers, SplitType splitType, const vector<double> &splitValues = {}) {
        Group *group = getGroup(groupId);
        if (!group) return "";
        return group->addExpense(description, amount, paidByUserId, involvedUsers, splitType, splitValues);
    }

    void settlePaymentInGroup(string groupId, string fromUserId, string toUserId, double amount) {
        Group *group = getGroup(groupId);
        if (!group) return;
        group->settlePayment(fromUserId, toUserId, amount);
    }

    void rebuildIndividualBalancesFromExpenses() {
        for (auto &entry : users) {
            entry.second->balances.clear();
        }
        for (const auto &entry : expenses) {
            const Expense *expense = entry.second;
            if (!expense->groupId.empty()) continue;
            for (const Split &split : expense->splits) {
                if (split.userId == expense->paidByUserId) continue;
                User *paidByUser = getUser(expense->paidByUserId);
                User *toUser = getUser(split.userId);
                if (paidByUser && toUser) {
                    paidByUser->updateBalance(toUser->userId, split.amount);
                    toUser->updateBalance(paidByUser->userId, -split.amount);
                }
            }
        }
    }

    string addIndividualExpense(string description, double amount, string paidByUserId, string toUserId,
                                SplitType splitType, const vector<double> &splitValues = {}) {
        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);
        Expense *expense = new Expense(description, amount, paidByUserId, splits);
        expenses[expense->expenseId] = expense;
        rebuildIndividualBalancesFromExpenses();
        return expense->expenseId;
    }

    bool editExpenseInGroup(string groupId, string expenseId, string description, double amount, string paidByUserId,
                            vector<string> involvedUsers, SplitType splitType, const vector<double> &splitValues = {}) {
        Group *group = getGroup(groupId);
        if (!group) return false;
        return group->editExpense(expenseId, description, amount, paidByUserId, involvedUsers, splitType, splitValues);
    }

    bool deleteExpenseInGroup(string groupId, string expenseId) {
        Group *group = getGroup(groupId);
        if (!group) return false;
        return group->deleteExpense(expenseId);
    }

    bool editIndividualExpense(string expenseId, string description, double amount, string paidByUserId, string toUserId,
                               SplitType splitType, const vector<double> &splitValues = {}) {
        auto it = expenses.find(expenseId);
        if (it == expenses.end()) return false;

        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);

        Expense *oldExpense = it->second;
        Expense *newExpense = new Expense(description, amount, paidByUserId, splits);
        newExpense->expenseId = expenseId;
        expenses[expenseId] = newExpense;
        delete oldExpense;

        rebuildIndividualBalancesFromExpenses();
        return true;
    }

    bool deleteIndividualExpense(string expenseId) {
        auto it = expenses.find(expenseId);
        if (it == expenses.end()) return false;
        delete it->second;
        expenses.erase(it);
        rebuildIndividualBalancesFromExpenses();
        return true;
    }

    void showUserBalance(string userId) {
        User *user = getUser(userId);
        if (!user) return;
        cout << "\n=========== Balance for " << user->name << " ====================" << endl;
        cout << "Total you owe: Rs " << fixed << setprecision(2) << user->getTotalOwed() << endl;
        cout << "Total others owe you: Rs " << fixed << setprecision(2) << user->getTotalOwing() << endl;
    }
    void showGroupBalances(string groupId) { if (Group *group = getGroup(groupId)) group->showGroupBalances(); }
    void simplifyGroupDebts(string groupId) { if (Group *group = getGroup(groupId)) group->simplifyGroupDebts(); }
};

inline Splitwise *Splitwise::instance = nullptr;
}

#endif
