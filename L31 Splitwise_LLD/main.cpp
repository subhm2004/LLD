// ============================================================================
//  main.cpp  —  Splitwise expense-sharing system ka demo driver
// ----------------------------------------------------------------------------
//  Users + group banao, expenses add karo (equal/exact/percentage split),
//  balances dekho, settle karo, aur debts simplify karo (min transactions).
//  Patterns: Facade + Singleton + Strategy + Factory + Observer.
// ============================================================================
#include <bits/stdc++.h>

#include "core/Splitwise.h"

using namespace std;
using namespace splitwise_lld;

int main() {
    Splitwise *manager = Splitwise::getInstance();

    User *user1 = manager->createUser("Aditya", "aditya@gmail.com");
    User *user2 = manager->createUser("Rohit", "rohit@gmail.com");
    User *user3 = manager->createUser("Manish", "manish@gmail.com");
    User *user4 = manager->createUser("Saurav", "saurav@gmail.com");

    Group *hostelGroup = manager->createGroup("Hostel Expenses");
    manager->addUserToGroup(user1->userId, hostelGroup->groupId);
    manager->addUserToGroup(user2->userId, hostelGroup->groupId);
    manager->addUserToGroup(user3->userId, hostelGroup->groupId);
    manager->addUserToGroup(user4->userId, hostelGroup->groupId);

    vector<string> groupMembers = {user1->userId, user2->userId, user3->userId, user4->userId};
    string lunchExpenseId =
        manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0, user1->userId, groupMembers, SplitType::EQUAL);

    vector<string> dinnerMembers = {user1->userId, user3->userId, user4->userId};
    vector<double> dinnerAmounts = {200.0, 300.0, 200.0};
    string dinnerExpenseId = manager->addExpenseToGroup(hostelGroup->groupId, "Dinner", 700.0, user3->userId, dinnerMembers,
                                                        SplitType::EXACT, dinnerAmounts);

    manager->showGroupBalances(hostelGroup->groupId);
    manager->editExpenseInGroup(hostelGroup->groupId, dinnerExpenseId, "Dinner Updated", 750.0, user3->userId,
                                dinnerMembers, SplitType::EXACT, {250.0, 300.0, 200.0});
    manager->deleteExpenseInGroup(hostelGroup->groupId, lunchExpenseId);
    manager->showGroupBalances(hostelGroup->groupId);
    manager->simplifyGroupDebts(hostelGroup->groupId);
    manager->showGroupBalances(hostelGroup->groupId);

    string coffeeExpenseId = manager->addIndividualExpense("Coffee", 40.0, user2->userId, user4->userId, SplitType::EQUAL);
    manager->editIndividualExpense(coffeeExpenseId, "Coffee Updated", 60.0, user2->userId, user4->userId, SplitType::EQUAL);
    manager->deleteIndividualExpense(coffeeExpenseId);
    coffeeExpenseId = manager->addIndividualExpense("Coffee Re-added", 40.0, user2->userId, user4->userId, SplitType::EQUAL);
    manager->showUserBalance(user1->userId);
    manager->showUserBalance(user2->userId);
    manager->showUserBalance(user3->userId);
    manager->showUserBalance(user4->userId);

    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);
    manager->showGroupBalances(hostelGroup->groupId);
    return 0;
}
