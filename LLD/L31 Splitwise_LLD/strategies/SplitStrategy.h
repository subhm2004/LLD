// ============================================================================
//  strategies/SplitStrategy.h  —  STRATEGY PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Strategy = "ek hi kaam karne ke KAI tareeke, har tareeka apni class me;
//              runtime pe decide karo kaunsa use karna hai."
//
//  Yahan kaam ek hi hai: "amount ko members me baanto".
//  Tareeke teen hain: barabar / exact rupaye / percentage.
//
//  Pattern ke roles is file me:
//    Strategy (interface)  -> SplitStrategy       (sirf contract, koi logic nahi)
//    ConcreteStrategy      -> EqualSplit          (barabar baanto)
//                          -> ExactSplit          (jo bataya wahi baanto)
//                          -> PercentageSplit     (% se calculate karke baanto)
//    Context (user of it)  -> Group / Splitwise   (strategy ko sirf call karte
//                                                  hain, andar jhaankte nahi)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  Strategy KYUN? Bina iske code aisa dikhta:                              │
//  │                                                                          │
//  │      if (type == EQUAL)           { ...20 lines...  }                    │
//  │      else if (type == EXACT)      { ...20 lines...  }                    │
//  │      else if (type == PERCENTAGE) { ...20 lines...  }                    │
//  │                                                                          │
//  │  Aur ye if-else Group::addExpense me, Group::editExpense me, Splitwise   │
//  │  ke addIndividualExpense me — HAR JAGAH copy-paste hota. Naya split type │
//  │  add karne pe saari jagah dhoondh ke edit karna padta (aur ek jagah bhool│
//  │  gaye to bug). Strategy me: ek nayi class banao, factory me ek line, bas.│
//  │  Yahi OPEN/CLOSED PRINCIPLE hai — nayi cheez ke liye code JODO, purane   │
//  │  code ko CHHEDO mat.                                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Sabse important baat — INTERFACE SABKA EK HI HAI:
//     calculateSplit(totalAmount, userIds, values) -> vector<Split>
//  Teeno classes ka signature bilkul same. Isi liye caller ko `SplitStrategy*`
//  pakad ke bas `calculateSplit()` bulana hai — usse pata bhi nahi ki andar
//  kaunsi class chal rahi hai. Ye "polymorphism" hai, aur yahi Strategy ka
//  poora jaadu hai.
// ============================================================================
#ifndef SPLITWISE_LLD_STRATEGIES_SPLITSTRATEGY_H
#define SPLITWISE_LLD_STRATEGIES_SPLITSTRATEGY_H

#include <bits/stdc++.h>

#include "../models/Split.h"

using namespace std;

namespace splitwise_lld {

// ============================================================================
//  1. STRATEGY (interface) — sirf contract, zero implementation
// ============================================================================
class SplitStrategy {
public:
    // `= 0` -> pure virtual. Har concrete strategy ko ye likhna HI padega.
    //
    // Params:
    //   totalAmount -> pura bill (EXACT me ise ignore kiya jaata hai)
    //   userIds     -> kis-kis me baantna hai
    //   values      -> extra input, strategy pe depend karta hai:
    //                    EQUAL      -> chahiye hi nahi (khaali {} bhejo)
    //                    EXACT      -> rupaye ki list  [200, 300, 200]
    //                    PERCENTAGE -> percent ki list [50, 30, 20]
    //                  `= {}` default hai, isi liye EQUAL wale call me ise
    //                  bhejna zaroori nahi (main.cpp me dekh lo).
    //
    // Ye ek "leaky" jagah hai design me: `values` ka MATLAB har strategy me
    // alag hai. Aur saaf design me har strategy ka apna typed input hota, par
    // tab interface ek nahi rehta. Ye practical trade-off hai — interviews me
    // is trade-off ka zikr karna acha impression banata hai.
    virtual vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds,
                                         const vector<double> &values = {}) = 0;

    // Virtual destructor — MUST hai, kyunki caller `unique_ptr<SplitStrategy>`
    // me EqualSplit ka object rakhta hai. Delete BASE pointer se hoga, to base
    // ka destructor virtual na ho to derived ka destructor chalega hi nahi.
    virtual ~SplitStrategy() {}
};

// ============================================================================
//  2. CONCRETE STRATEGY — EQUAL: sabka hissa barabar
// ----------------------------------------------------------------------------
//  800 rupaye, 4 log  ->  har banda 200.
// ============================================================================
class EqualSplit : public SplitStrategy {
public:
    // Teesra param ka NAAM nahi likha (sirf type `const vector<double> &`).
    // Ye jaan-boojh ke hai — EQUAL ko values chahiye hi nahi. Naam na dene se
    // compiler "unused parameter" ki warning nahi deta, aur padhne wale ko turant
    // signal mil jaata hai: "is strategy me ye param use hi nahi hota".
    vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds, const vector<double> &) override {
        vector<Split> splits;

        // Bas ek division. Note: yahan `userIds.size()` 0 nahi hona chahiye,
        // warna divide-by-zero -> inf/nan. Production code me guard lagta,
        // par yahan caller (Group) pehle hi members validate kar chuka hota hai.
        double amountPerUser = totalAmount / userIds.size();

        for (const string &userId : userIds) splits.push_back(Split(userId, amountPerUser));
        return splits;
    }
    // Rounding ki baat: 100 rupaye / 3 log = 33.333... har ek ko.
    // Jodo to 99.999... -> 1 paisa "gayab". Asli Splitwise ye extra paisa kisi
    // ek banda pe daal deta hai. Yahan hum 0.01 wale epsilon se ignore kar dete
    // hain (User::updateBalance dekho) — LLD ke liye kaafi hai.
};

// ============================================================================
//  3. CONCRETE STRATEGY — EXACT: caller khud batata hai kaun kitna dega
// ----------------------------------------------------------------------------
//  Kab? Jab hisse barabar nahi hote. Dinner me tumne 300 ka pizza khaya, maine
//  200 ka burger -> [200, 300, 200] bhej do.
// ============================================================================
class ExactSplit : public SplitStrategy {
public:
    // Yahan PEHLA param (totalAmount) ka naam nahi hai — kyunki isko iski
    // zaroorat hi nahi! Amounts to caller ne khud values me de diye. Bas unko
    // userIds ke saath jod ke Split bana do.
    vector<Split> calculateSplit(double, const vector<string> &userIds, const vector<double> &values) override {
        vector<Split> splits;

        // i-th user ka hissa = i-th value. Positional mapping hai — yaani
        // userIds aur values ka ORDER match hona chahiye, aur size bhi.
        //
        // WARNING: yahan koi validation nahi hai. Agar values.size() < userIds.size()
        // to `values[i]` out-of-bounds jaayega (undefined behavior). Aur agar
        // values ka sum != totalAmount, to balance sheet galat ban jaayegi —
        // koi complain nahi karega. Interview me ye ek accha "yahan main validation
        // add karunga" wala point hai:
        //     if (values.size() != userIds.size()) throw ...
        //     if (abs(accumulate(values) - totalAmount) > 0.01) throw ...
        for (size_t i = 0; i < userIds.size(); i++) splits.push_back(Split(userIds[i], values[i]));
        return splits;
    }
};

// ============================================================================
//  4. CONCRETE STRATEGY — PERCENTAGE: % ke hisaab se baanto
// ----------------------------------------------------------------------------
//  1000 ka bill, [50, 30, 20] %  ->  [500, 300, 200] rupaye.
//  Kab? Roommates jinke room ka size alag hai -> rent 50/30/20 me baanta.
// ============================================================================
class PercentageSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds, const vector<double> &values) override {
        vector<Split> splits;

        // Formula: hissa = (total × percent) / 100
        // Ye ExactSplit se sirf itna alag hai ki yahan ek chhoti si math hai.
        // Baaki structure bilkul same — aur yahi Strategy ki khoobsurti hai:
        // sirf FORMULA badalta hai, poora flow same rehta hai.
        for (size_t i = 0; i < userIds.size(); i++) splits.push_back(Split(userIds[i], (totalAmount * values[i]) / 100.0));
        return splits;
    }
    // Yahan bhi validation missing hai: percentages ka sum 100 hona chahiye.
    // 50+30+30 = 110 bheja to kisi ko pata nahi chalega, bas sabka hissa badh
    // jaayega aur total 1100 ban jaayega. Real code me check zaroori hai.
};

}

#endif
