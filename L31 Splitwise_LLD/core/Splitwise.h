// ============================================================================
//  core/Splitwise.h  —  FACADE + SINGLETON: poore system ka ek hi "front door"
// ----------------------------------------------------------------------------
//  Do patterns ek saath is class me:
//
//  1️⃣ FACADE (Structural) — "andar ka jungle chhupa do, ek simple API do"
//     Andar kitna kuch hai: User, Group, Expense, Split, SplitStrategy,
//     SplitFactory, DebtSimplifier, Observer... 8 classes.
//     Client (main.cpp) ko in me se kitni pata honi chahiye? SIRF EK — Splitwise.
//
//         manager->addExpenseToGroup(gid, "Lunch", 800, u1, members, EQUAL);
//
//     Bas. Client ko ye jaanne ki zaroorat NAHI ki andar Factory se strategy
//     bani, strategy ne splits banaye, Group ne matrix update kiya, Observers
//     ko notify kiya. Ek call, kaam khatam. Yahi Facade hai — complexity ki
//     deewar ke aage ek saaf darwaza.
//
//  2️⃣ SINGLETON (Creational) — "poore program me is class ka SIRF EK object"
//     Kyun? Kyunki ye poore app ka data store hai (saare users, saare groups).
//     Do Splitwise objects ban gaye to disaster: ek me user bana, doosre me wo
//     user hai hi nahi. Ek hi "sach" hona chahiye. Isi liye constructor private
//     hai — bahar se `new Splitwise()` likha hi nahi ja sakta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO ALAG-ALAG DUNIYA — ye samajhna zaroori hai:                       │
//  │                                                                          │
//  │   GROUP expenses          |   INDIVIDUAL expenses (1-to-1)               │
//  │   -----------------       |   ---------------------------                │
//  │   Group ke andar rehte    |   Splitwise ke `expenses` map me rehte       │
//  │   Group::groupBalances    |   User::balances ko affect karte              │
//  │   ko affect karte         |   (groupId khaali "" hota hai)               │
//  │                                                                          │
//  │   Splitwise inko sirf FORWARD karta hai:   |  Splitwise inko KHUD handle │
//  │   addExpenseToGroup() -> group->addExpense()|  karta hai (poora logic     │
//  │   (thin wrapper, bas)                       |   yahin likha hai)         │
//  │                                                                          │
//  │  Alag KYUN rakhe? Kyunki group chhodte waqt (removeMember) sirf GROUP ka │
//  │  hisaab check hona chahiye. Tumhare personal coffee ke 40 rupaye tumhe   │
//  │  hostel group chhodne se nahi rok sakte. Alag rakhne se ye apne aap ho   │
//  │  jaata hai — koi extra filtering ki zaroorat hi nahi.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Is Singleton implementation me 2 asli kamiyan (interview me poocha jaata hai):
//     a) THREAD-SAFE NAHI: do threads ek saath getInstance() call karein to dono
//        `instance == nullptr` dekh sakte hain -> DO objects ban jayenge.
//        Fix (C++11+, sabse saaf): function ke andar `static Splitwise inst;`
//        rakh do — C++ guarantee karta hai ki wo exactly ek baar, thread-safe
//        tareeke se banega ("magic static").
//     b) MEMORY LEAK: `new Splitwise()` kabhi delete nahi hota. Practically
//        program-end pe OS sab wapas le leta hai, to nuksaan nahi — par technically
//        leak hai. `static` wala fix isko bhi khatam kar deta hai.
// ============================================================================
#ifndef SPLITWISE_LLD_CORE_SPLITWISE_H
#define SPLITWISE_LLD_CORE_SPLITWISE_H

#include <bits/stdc++.h>

#include "Group.h"

using namespace std;

namespace splitwise_lld {

class Splitwise {
private:
    // Poore app ka data — ye class hi in sab ki MAALIK hai (ownership yahin hai).
    map<string, User *> users;      // saare users (group ke ho ya na ho)
    map<string, Group *> groups;    // saare groups
    map<string, Expense *> expenses;// ⚠ SIRF individual expenses! Group wale
                                    //    apne-apne Group ke andar rehte hain.

    // ---- SINGLETON ka setup -------------------------------------------------
    static Splitwise *instance;  // ek hi object, class-level pe rakha
    Splitwise() {}               // ⭐ constructor PRIVATE — yahi Singleton ka
                                 //    asli taala hai. Bahar se `new Splitwise()`
                                 //    likhoge to compile error milega.

public:
    // Object paane ka EKLAUTA rasta. "Lazy initialization" hai — object tabhi
    // banta hai jab pehli baar maanga jaaye, program shuru hote hi nahi.
    static Splitwise *getInstance() {
        if (instance == nullptr) instance = new Splitwise();
        return instance;
        // (Upar bataya thread-safety wala issue yahin hai — ye `if` do threads
        //  ke beech me toot sakta hai.)
    }

    // ========================================================================
    //  USER / GROUP banana
    // ========================================================================
    User *createUser(string name, string email) {
        User *user = new User(name, email);
        users[user->userId] = user;  // registry me daal do, taaki id se dhoondh sakein
        return user;
    }

    Group *createGroup(const string &name) {
        Group *group = new Group(name);
        groups[group->groupId] = group;
        return group;
    }

    // Safe lookup — id galat hui to crash nahi, `nullptr` milega.
    // `count()` pehle check karta hai ki key hai ya nahi. Seedha `users[userId]`
    // likhte to map me ek NAYI khaali entry ban jaati (operator[] ka side-effect!)
    // — ek classic C++ trap. Isi liye pehle count(), phir hi index karo.
    User *getUser(const string &userId) { return users.count(userId) ? users[userId] : nullptr; }
    Group *getGroup(const string &groupId) { return groups.count(groupId) ? groups[groupId] : nullptr; }

    // ========================================================================
    //  GROUP ke kaam — ye sab THIN WRAPPERS hain (Facade ka classic roop)
    //  Splitwise khud kuch nahi karta: bas id se object dhoondta hai aur kaam
    //  Group ko forward kar deta hai. Iska fayda: client ke paas Group ka pointer
    //  na bhi ho, sirf groupId se kaam chal jaata hai.
    // ========================================================================
    void addUserToGroup(const string &userId, const string &groupId) {
        User *user = getUser(userId);
        Group *group = getGroup(groupId);
        if (user && group) group->addMember(user);  // dono valid ho, tabhi
    }

    bool removeUserFromGroup(const string &userId, const string &groupId) {
        Group *group = getGroup(groupId);
        User *user = getUser(userId);
        if (!group || !user) return false;

        // Asli decision Group leta hai (dues clear hain ya nahi — canUserLeaveGroup).
        // Splitwise sirf natija print karta hai. Business rule wahin rehna chahiye
        // jahan uska data hai — yahi "tell, don't ask" ka sahi roop hai.
        bool removed = group->removeMember(userId);
        if (removed) cout << user->name << " successfully left " << group->name << endl;
        return removed;
    }

    string addExpenseToGroup(string groupId, string description, double amount, string paidByUserId,
                             vector<string> involvedUsers, SplitType splitType, const vector<double> &splitValues = {}) {
        Group *group = getGroup(groupId);
        if (!group) return "";  // khaali id = "fail" ka signal
        return group->addExpense(description, amount, paidByUserId, involvedUsers, splitType, splitValues);
    }

    void settlePaymentInGroup(string groupId, string fromUserId, string toUserId, double amount) {
        Group *group = getGroup(groupId);
        if (!group) return;
        group->settlePayment(fromUserId, toUserId, amount);
    }

    // ========================================================================
    //  INDIVIDUAL (1-to-1) EXPENSES — ye Splitwise KHUD handle karta hai
    //  (koi Group nahi, isi liye logic yahin likha hai)
    // ========================================================================

    // ---- Saare individual balances ZERO se dobara banao ----------------------
    // Wahi "RECALCULATE, DON'T REVERSE" wala idea jo Group me tha — bas yahan
    // User::balances pe. Expenses source of truth, balances derived.
    void rebuildIndividualBalancesFromExpenses() {
        // Step 1: sabki balance sheet poori saaf karo
        for (auto &entry : users) {
            entry.second->balances.clear();
        }

        // Step 2: har individual expense dobara apply karo
        for (const auto &entry : expenses) {
            const Expense *expense = entry.second;

            // ⭐ Group wale expenses SKIP — unka hisaab Group ke matrix me hota hai,
            // User::balances me nahi. (Waise abhi is `expenses` map me group wale
            // aate hi nahi — ye check defensive hai, taaki kal koi galti se group
            // expense yahan daal de to balances kharab na ho.)
            if (!expense->groupId.empty()) continue;

            for (const Split &split : expense->splits) {
                // Payer ka apna split skip — apne aap ko kaun paisa dega?
                if (split.userId == expense->paidByUserId) continue;

                User *paidByUser = getUser(expense->paidByUserId);
                User *toUser = getUser(split.userId);

                if (paidByUser && toUser) {
                    // Dono taraf likho (mirrored) — User::balances ka bhi wahi
                    // sign convention hai jo Group::groupBalances ka tha:
                    //   +ve = mera lena, -ve = mera dena
                    paidByUser->updateBalance(toUser->userId, split.amount);   // payer ka lena badha
                    toUser->updateBalance(paidByUser->userId, -split.amount);  // doosre ka dena badha
                }
            }
        }
    }

    string addIndividualExpense(string description, double amount, string paidByUserId, string toUserId,
                                SplitType splitType, const vector<double> &splitValues = {}) {
        // Wahi Factory + Strategy jodi — group wale flow jaisi bilkul.
        // Bas "members" yahan sirf DO log hain: payer aur doosra banda.
        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);

        // groupId nahi de rahe -> default "" -> ye INDIVIDUAL expense hai.
        Expense *expense = new Expense(description, amount, paidByUserId, splits);
        expenses[expense->expenseId] = expense;

        // Note: yahan bhi poora rebuild kar rahe hain (jabki sirf add hua hai,
        // incremental apply se kaam chal jaata — Group::addExpense ki tarah).
        // Thoda extra kaam hai, par consistent aur galti-proof. Chhote data pe
        // ye bilkul theek trade-off hai.
        rebuildIndividualBalancesFromExpenses();
        return expense->expenseId;
    }

    // ---- Group expense edit/delete — sirf forward kar do --------------------
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

    // ---- Individual expense edit — "purana phenko, naya banao" ---------------
    // Bilkul Group::editExpense jaisa hi pattern, bas User::balances pe.
    bool editIndividualExpense(string expenseId, string description, double amount, string paidByUserId, string toUserId,
                               SplitType splitType, const vector<double> &splitValues = {}) {
        auto it = expenses.find(expenseId);
        if (it == expenses.end()) return false;

        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);

        Expense *oldExpense = it->second;
        Expense *newExpense = new Expense(description, amount, paidByUserId, splits);
        newExpense->expenseId = expenseId;  // purani id chipka do (bahar ka code use kar raha hai)
        expenses[expenseId] = newExpense;
        delete oldExpense;                  // memory free

        rebuildIndividualBalancesFromExpenses();  // sab dobara calculate
        return true;
    }

    bool deleteIndividualExpense(string expenseId) {
        auto it = expenses.find(expenseId);
        if (it == expenses.end()) return false;

        delete it->second;      // memory free
        expenses.erase(it);     // map se hatao

        // Ab bache hue expenses se balances dobara bano -> deleted expense ka
        // asar apne aap gayab. Reverse karne ki zaroorat hi nahi.
        rebuildIndividualBalancesFromExpenses();
        return true;
    }

    // ========================================================================
    //  DISPLAY
    // ========================================================================
    void showUserBalance(string userId) {
        User *user = getUser(userId);
        if (!user) return;

        cout << "\n=========== Balance for " << user->name << " ====================" << endl;

        // Do alag numbers dikhate hain, ek "net" nahi — kyunki asli Splitwise
        // bhi yahi karta hai. Net (+200) se pata hi nahi chalta ki tumhe kisi ko
        // paisa DENA bhi hai. Dono alag dikhana zyada honest hai.
        //
        // `fixed << setprecision(2)` -> hamesha 2 decimal (paise). Iske bina
        // C++ "266.667" ya "1e+03" jaisa kuch bhi print kar sakta hai — paison
        // ke liye bilkul galat.
        cout << "Total you owe: Rs " << fixed << setprecision(2) << user->getTotalOwed() << endl;
        cout << "Total others owe you: Rs " << fixed << setprecision(2) << user->getTotalOwing() << endl;

        // ⚠ Ye sirf INDIVIDUAL expenses ka hisaab hai — group wale isme nahi hain!
        // Asli Splitwise dono jod ke ek total dikhata. Yahan dono duniya alag hain,
        // isi liye group ka hisaab dekhne ke liye showGroupBalances() alag se
        // bulana padta hai. Ise "unified balance" banana ek accha next-step
        // exercise hai.
    }

    // Ye do bhi pure forwarders hain — `if` ke andar hi variable declare kar diya
    // (C++17 ka feature: `if (Group *g = ...)`) — group mila to hi aage badho.
    void showGroupBalances(string groupId) { if (Group *group = getGroup(groupId)) group->showGroupBalances(); }
    void simplifyGroupDebts(string groupId) { if (Group *group = getGroup(groupId)) group->simplifyGroupDebts(); }
};

// static member ki asli definition. `inline` (C++17) isliye taaki ye header
// kai .cpp files me include ho to linker "duplicate symbol" ka rona na roye.
inline Splitwise *Splitwise::instance = nullptr;

}

#endif
