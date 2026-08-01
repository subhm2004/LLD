// ============================================================================
//  core/Group.h  —  Ek group (jaise "Hostel Expenses"): members + expenses +
//                   balance matrix. Ye OBSERVER pattern ka SUBJECT bhi hai.
// ----------------------------------------------------------------------------
//  Group 3 cheezein sambhalta hai:
//    1. MEMBERS       -> kaun-kaun is group me hai (vector<User*>)
//    2. EXPENSES      -> is group ke saare kharche (map<id, Expense*>)
//    3. BALANCE MATRIX-> kaun kisko kitna deta hai (map<string, map<string,double>>)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  BALANCE MATRIX ko samjho — ye is class ka dil hai                       │
//  │                                                                          │
//  │    groupBalances[A][B] = +200   ->  "B ko A ko 200 dene hain"           │
//  │    groupBalances[B][A] = -200   ->  wahi baat, B ki taraf se             │
//  │                                                                          │
//  │  Har karza DO JAGAH likha jaata hai (mirrored). Isse har banda ki apni   │
//  │  row padh ke turant uska poora hisaab dikh jaata hai — dhoondhna nahi    │
//  │  padta. Cost: har update DO jagah karna padta hai (updateGroupBalance    │
//  │  ye khud sambhal leta hai, isi liye kabhi manually mat chhedna).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SABSE IMPORTANT DESIGN DECISION — "RECALCULATE, DON'T REVERSE":
//    Expense edit/delete hone par hum purane balance ko "ulta" (reverse) karke
//    hatane ki koshish NAHI karte. Uske bajaye:
//        matrix poora ZERO karo, phir saare BACHE HUE expenses dobara apply karo
//    (dekho: recalculateBalancesFromExpenses).
//
//    Ye slow lagta hai (har edit pe O(expenses × splits)), par bahut zyada SAFE
//    hai. Kyun? Kyunki reverse karna bug ka ghar hai — purana amount kya tha,
//    kaun involved tha, kya wo epsilon se erase ho chuka tha... ek bhi cheez
//    galat yaad rakhi to balance sheet hamesha ke liye kharab. Recalculate me
//    aisa ho hi nahi sakta: EXPENSES source of truth hain, BALANCES unse banaya
//    hua derived data. Source theek hai, to derived hamesha theek banega.
//
//    Trade-off yaad rakho: correctness > speed, jab tak speed asli problem na
//    ban jaye. 10,000 expenses ho jaayein to incremental reverse pe sochenge.
//
//  ⚠ Ek asli BUG jo yahan chhupa hai (padhne wale, ise pakdo!):
//    simplifyGroupDebts() matrix ko simplified transfers se REPLACE kar deta
//    hai. Par expenses list waisi ki waisi rehti hai. Ab agar iske BAAD koi
//    expense edit/delete hua, to recalculateBalancesFromExpenses() chalega aur
//    matrix ko expenses se DOBARA banayega — yaani simplification GAYAB ho
//    jaayegi, purane pairwise debts wapas aa jayenge. main.cpp me ye dikhta bhi
//    nahi kyunki wahan simplify ke baad koi group-expense edit nahi hota.
//    Asli fix: simplification ko "settlement expenses" ki tarah store karo, ya
//    ise sirf ek DISPLAY-time calculation banao (matrix me likho hi mat).
// ============================================================================
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
    // ---- id se User object dhoondho -----------------------------------------
    // Linear search hai (O(n)) — chhote groups ke liye bilkul theek. Bade scale
    // pe `map<string, User*>` rakhna better hota, par tab members ka ORDER kho
    // jaata (aur notifications ka order bhi). Chhoti list pe vector hi behtar hai.
    User *getUserByuserId(const string &userId) {
        for (User *member : members) if (member->userId == userId) return member;
        return nullptr;
    }

    // ---- Poora matrix zero kar do -------------------------------------------
    // Dhyaan: `groupBalances.clear()` NAHI kiya — sirf har member ki ROW clear
    // ki hai. Kyunki keys (members) bache rehne chahiye. Agar poora map clear kar
    // dete, to members matrix se hi gayab ho jaate aur isMember() jhooth bolne
    // lagta (wo groupBalances me hi to check karta hai!).
    void resetGroupBalances() {
        for (User *member : members) {
            groupBalances[member->userId].clear();
        }
    }

    // ---- Ek expense ka asar matrix pe daalo ---------------------------------
    // Har split ke liye: "is banda ko, payer ko, itna dena hai".
    void applyExpenseToBalances(const Expense *expense) {
        for (const Split &split : expense->splits) {
            // ⭐ Payer ka apna split SKIP — apne aap ko kaun paisa dega?
            // 800 ka lunch, 4 log: Aditya ne pay kiya. Uska bhi 200 ka split hai
            // (usne bhi khaya!), par wo khud ko 200 nahi dega. Sirf baaki 3 se
            // 200-200 lega = 600 wapas. Aur 800 me se 200 uska apna tha. Perfect.
            if (split.userId != expense->paidByUserId) {
                updateGroupBalance(expense->paidByUserId, split.userId, split.amount);
            }
        }
    }

    // ---- Matrix ko ZERO se dobara bana do (upar wala "recalculate" wala idea) --
    void recalculateBalancesFromExpenses() {
        resetGroupBalances();                                    // sab saaf
        for (const auto &entry : groupExpenses) {                // har bache hue expense ko
            applyExpenseToBalances(entry.second);                // dobara apply karo
        }
    }

public:
    static int nextGroupId;

    string groupId;
    string name;
    vector<User *> members;                        // group ke log (Observers bhi)
    map<string, Expense *> groupExpenses;          // id -> expense (SOURCE OF TRUTH)
    map<string, map<string, double>> groupBalances;// kaun-kisko-kitna (DERIVED)

    // `explicit` -> accidental implicit conversion rokta hai. Iske bina koi
    // `Group g = "Hostel";` likh sakta tha (string chupke se Group ban jaata).
    // `explicit` se ab `Group g("Hostel");` hi likhna padega. Single-argument
    // constructor pe `explicit` lagana ek acchi default aadat hai.
    explicit Group(const string &name) : name(name) { groupId = "group" + to_string(++nextGroupId); }

    // Destructor: group ke apne expenses ka memory free karo.
    // Note: `members` ko delete NAHI karte! Users ke maalik Splitwise class hai
    // (uske `users` map me hain) — group unhe sirf POINT karta hai. Yahan delete
    // kar dete to Splitwise ke paas dangling pointers reh jaate.
    // Rule: jo BANATA hai wahi DELETE karta hai (ownership clear rakho).
    ~Group() { for (auto &pair : groupExpenses) delete pair.second; }

    void addMember(User *user) {
        members.push_back(user);
        groupBalances[user->userId] = {};  // khaali row bana do (ab isMember() true dega)
        cout << user->name << " added to group " << name << endl;
    }

    // Member hai ya nahi — matrix ki keys se check karte hain (members vector me
    // linear search karne se faster: map lookup O(log n)).
    bool isMember(const string &userId) { return groupBalances.find(userId) != groupBalances.end(); }

    // ---- OBSERVER PATTERN ka dil --------------------------------------------
    // Group (subject) sabhi members (observers) ko event ki khabar deta hai.
    // Dhyaan se dekho: loop `Observer*` pe chal raha hai, `User*` pe nahi —
    // jabki members vector<User*> hai! Ye jaan-boojh ke hai: ye batata hai ki
    // Group ko User ki KOI zaroorat nahi, use bas "koi bhi cheez jo update() ka
    // jawab de" chahiye. Kal EmailNotifier / SmsNotifier add karo — ye function
    // ek line bhi nahi badlega.
    void notifyMembers(const string &message) {
        for (Observer *observer : members) observer->update(message);
    }

    // ---- Matrix update — HAMESHA isi se karo, manually kabhi nahi -------------
    void updateGroupBalance(const string &fromUserId, const string &toUserId, double amount) {
        // Dono taraf likho (mirrored) — ye invariant kabhi todna nahi hai.
        groupBalances[fromUserId][toUserId] += amount;   // from ka lena badha
        groupBalances[toUserId][fromUserId] -= amount;   // to ka dena badha

        // Hisaab barabar (1 paise se kam bacha) -> entry hi uda do.
        // Ye sirf "saaf dikhne" ke liye nahi hai — canUserLeaveGroup() ISI PE
        // depend karta hai. Agar 0.0000001 ki entry padi reh jaati, to banda
        // group chhod hi nahi paata ("clear your dues" bolta rehta, jabki kuch
        // due hai hi nahi). Floating-point ka classic dard.
        if (abs(groupBalances[fromUserId][toUserId]) < 0.01) groupBalances[fromUserId].erase(toUserId);
        if (abs(groupBalances[toUserId][fromUserId]) < 0.01) groupBalances[toUserId].erase(fromUserId);
    }

    // ---- Group chhodne ka RULE: pehle saara hisaab clear karo ----------------
    // Real Splitwise me bhi yahi rule hai. Agar bina settle kiye nikal gaye, to
    // uska karza kahan jaayega? Kisi ko to bhugatna padega. Isi liye block.
    bool canUserLeaveGroup(const string &userId) {
        if (!isMember(userId)) throw runtime_error("user is not a part of this group");

        // Uski row me koi bhi non-zero entry bachi? To nahi ja sakta.
        // (Yaad rakho: settled entries to updateGroupBalance khud erase kar deta
        //  hai. To normally ye row khaali hi hoti. Ye check double-safety hai.)
        for (auto &balance : groupBalances[userId]) if (abs(balance.second) > 0.01) return false;
        return true;
    }

    bool removeMember(const string &userId) {
        if (!canUserLeaveGroup(userId)) {
            cout << "\nUser not allowed to leave group without clearing expenses" << endl;
            return false;
        }

        // Members vector se hatao.
        // `remove()` asal me kuch DELETE nahi karta — wo bas matching elements ko
        // peeche dhakel deta hai aur ek iterator return karta hai jahan se "kachra"
        // shuru hota hai. Asli hatana `erase()` karta hai. Isi liye dono saath me
        // — ise "erase-remove idiom" kehte hain, C++ ka classic muhavara.
        for (User *user : members) {
            if (user->userId == userId) {
                members.erase(remove(members.begin(), members.end(), user), members.end());
                break;
            }
        }

        // Matrix se bhi poori tarah nikalo — DONO taraf se:
        groupBalances.erase(userId);                                          // (1) uski apni row
        for (auto &memberBalance : groupBalances) memberBalance.second.erase(userId);  // (2) baakiyon ki
                                                                              //     rows me uska column
        // Agar (2) bhool jaate, to doosron ki balance sheet me ek "bhoot" entry
        // reh jaati — ek aisa banda jo group me hai hi nahi. Aur showGroupBalances
        // usko print karne ki koshish me getUserByuserId(...) se nullptr paata
        // aur ->name pe CRASH kar jaata. Isi liye cleanup poora karna zaroori hai.
        return true;
    }

    // ========================================================================
    //  EXPENSE ADD — yahan saare patterns ek saath kaam karte dikhte hain
    // ========================================================================
    string addExpense(string description, double amount, string paidByUserId, vector<string> involvedUsers,
                      SplitType splitType, const vector<double> &splitValues = {}) {
        // VALIDATION pehle — bahar ka koi banda group ke expense me nahi ghus sakta.
        if (!isMember(paidByUserId)) throw runtime_error("user is not a part of this group");
        for (const string &userId : involvedUsers) if (!isMember(userId)) throw runtime_error("involvedUsers invalid");

        // ⭐ FACTORY + STRATEGY, do line me:
        //   Factory  -> enum dekh ke sahi strategy ka object bana ke deti hai
        //   Strategy -> wo object amount ko baant ke Split list bana deta hai
        // Group ko ye pata hi NAHI ki andar EqualSplit chal raha hai ya
        // PercentageSplit. Bas `calculateSplit()` bulaya, kaam ho gaya.
        //
        // `unique_ptr` -> factory ne `new` kiya tha; ye scope khatam hote hi khud
        // `delete` kar dega. Manually delete karne ki zaroorat nahi, aur exception
        // aa jaaye tab bhi leak nahi hoga (RAII ka fayda).
        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, involvedUsers, splitValues);

        // Expense banao aur group ki list me daal do (ye SOURCE OF TRUTH hai)
        Expense *expense = new Expense(description, amount, paidByUserId, splits, groupId);
        groupExpenses[expense->expenseId] = expense;

        // Matrix pe asar daalo. Note: yahan poora recalculate NAHI kiya — kyunki
        // ADD me purana kuch badla hi nahi, sirf naya jud raha hai. To incremental
        // apply safe hai aur sasta bhi. (Edit/delete me recalculate karna PADTA
        // hai, kyunki wahan purana hisaab galat ho chuka hota hai.)
        applyExpenseToBalances(expense);

        string paidByName = getUserByuserId(paidByUserId)->name;

        // ⭐ OBSERVER — sabko khabar kar do
        notifyMembers("New expense added: " + description + " (Rs " + to_string(amount) + ")");

        cout << "Expense added to " << name << ": " << description << " (Rs " << amount << ") paid by " << paidByName << endl;
        return expense->expenseId;  // caller ko id do, taaki baad me edit/delete kar sake
    }

    // ========================================================================
    //  EXPENSE EDIT — "purana phenko, naya banao, sab dobara calculate karo"
    // ========================================================================
    bool editExpense(const string &expenseId, string description, double amount, string paidByUserId,
                     vector<string> involvedUsers, SplitType splitType, const vector<double> &splitValues = {}) {
        // Validation — par yahan `throw` ke bajaye `return false`. Ye addExpense
        // se INCONSISTENT hai (wahan throw hota hai). Ideally dono ek jaisa hona
        // chahiye — ye is code ki ek asli kami hai, notice karne layak.
        if (!groupExpenses.count(expenseId)) return false;
        if (!isMember(paidByUserId)) return false;
        for (const string &userId : involvedUsers) if (!isMember(userId)) return false;

        // Naye hisaab se splits banwao (split type bhi badla ja sakta hai!)
        unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(splitType));
        vector<Split> splits = strategy->calculateSplit(amount, involvedUsers, splitValues);

        // Purane object ko modify karne ke bajaye NAYA banao aur SWAP kar do.
        Expense *oldExpense = groupExpenses[expenseId];
        Expense *newExpense = new Expense(description, amount, paidByUserId, splits, groupId);

        // ⭐ Purani ID chipka do — bahar ke code ke paas jo expenseId hai wo valid
        // rehni chahiye. (Constructor ne ek nayi id di thi, use overwrite kar rahe.)
        newExpense->expenseId = expenseId;

        groupExpenses[expenseId] = newExpense;
        delete oldExpense;  // ab purana object kisi ke kaam ka nahi -> memory free

        // ⭐ Yahan RECALCULATE zaroori hai (incremental apply se kaam nahi chalega):
        // purana expense matrix pe apna asar chhod chuka tha, aur ab wo asar galat
        // hai. Use "ulta" karne ke bajaye — sab zero karo, saare bache expenses
        // dobara apply karo. Simple, aur galti ki gunjaish hi nahi.
        recalculateBalancesFromExpenses();

        notifyMembers("Expense edited: " + description + " (ID: " + expenseId + ")");
        return true;
    }

    // ========================================================================
    //  EXPENSE DELETE — hatao, phir sab dobara calculate karo
    // ========================================================================
    bool deleteExpense(const string &expenseId) {
        auto it = groupExpenses.find(expenseId);
        if (it == groupExpenses.end()) return false;  // hai hi nahi

        delete it->second;          // memory free (Expense ka object)
        groupExpenses.erase(it);    // map se entry hatao

        // Ab bache hue expenses se matrix dobara bana do. Deleted expense ka asar
        // apne aap gayab ho jaayega — kyunki wo ab list me hai hi nahi.
        // Yahi hai "recalculate, don't reverse" ki khoobsurti.
        recalculateBalancesFromExpenses();

        notifyMembers("Expense deleted (ID: " + expenseId + ")");
        return true;
    }

    // ========================================================================
    //  SETTLEMENT — "maine tumhe cash de diya, hisaab kam karo"
    // ========================================================================
    bool settlePayment(string fromUserId, string toUserId, double amount) {
        if (!isMember(fromUserId) || !isMember(toUserId)) return false;

        // Bas matrix update — koi Expense object nahi banta.
        //
        // ⚠ Ye ek DESIGN GAP hai: settlements expense list me record nahi hote.
        // Matlab agle recalculateBalancesFromExpenses() (yaani agla koi bhi
        // edit/delete) pe ye settlement GAYAB ho jaayega, aur karza wapas aa
        // jaayega! Sahi fix: settlement ko bhi ek expense/transaction ki tarah
        // store karo, taaki wo bhi "source of truth" ka hissa ban jaye. Interview
        // me ye pakadna bada plus point hai.
        updateGroupBalance(fromUserId, toUserId, amount);

        string fromName = getUserByuserId(fromUserId)->name;
        string toName = getUserByuserId(toUserId)->name;
        notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " + to_string(amount));
        cout << "Settlement in " << name << ": " << fromName << " settled Rs " << amount << " with " << toName << endl;
        return true;
    }

    // ---- Poore group ka hisaab print karo -----------------------------------
    void showGroupBalances() {
        cout << "\n=== Group Balances for " << name << " ===" << endl;

        for (const auto &pair : groupBalances) {
            string memberId = pair.first;
            string memberName = getUserByuserId(memberId)->name;
            cout << memberName << "'s balances in group:" << endl;

            // Khaali row = is bande ka koi lena-dena nahi
            if (pair.second.empty()) cout << "  No outstanding balances" << endl;

            for (const auto &ub : pair.second) {
                string otherName = getUserByuserId(ub.first)->name;

                // Sign convention wapas yaad karo:
                //   +ve -> doosre ne MUJHE dena hai
                //   -ve -> maine DOOSRE ko dena hai
                // abs() isliye taaki user ko "-250" jaisa negative number na dikhe
                // — "Owes Rohit: Rs 250" padhne me natural lagta hai.
                if (ub.second > 0) cout << "  " << otherName << " owes: Rs " << fixed << setprecision(2) << ub.second << endl;
                else cout << "  Owes " << otherName << ": Rs " << fixed << setprecision(2) << abs(ub.second) << endl;
            }
        }
    }

    // ---- Debt simplification chalao ----------------------------------------
    // Poora kaam DebtSimplifier ko de diya — Group sirf apna matrix bhejta hai
    // aur naya (simplified) matrix wapas rakh leta hai.
    //
    // Ye SINGLE RESPONSIBILITY PRINCIPLE ka accha example hai: greedy matching ka
    // algorithm Group ka kaam nahi. Group ka kaam members/expenses/balances
    // sambhalna hai. Algorithm apni alag class me rehta hai — alag se test karo,
    // alag se optimize karo, alag se badlo. Group ko farak hi nahi padta.
    void simplifyGroupDebts() {
        groupBalances = DebtSimplifier::simplifyDebts(groupBalances);
        cout << "\nDebts have been simplified for group: " << name << endl;
    }
};

inline int Group::nextGroupId = 0;

}

#endif
