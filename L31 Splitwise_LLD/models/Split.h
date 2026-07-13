// ============================================================================
//  models/Split.h  —  Ek expense ke EK hisse ka record
// ----------------------------------------------------------------------------
//  Split ka matlab: "is expense me, IS user ka hissa ITNA hai."
//  Bas do field: kaun (userId) aur kitna (amount). Aur kuch nahi.
//
//  Example — 800 ka lunch, 4 log, EQUAL split. Splits bante hain:
//      Split("user1", 200)   Split("user2", 200)
//      Split("user3", 200)   Split("user4", 200)
//  Yaani ek Expense ke andar splits ka vector hota hai, aur unke amounts ka
//  total = expense ka totalAmount (EQUAL/PERCENTAGE me hamesha; EXACT me tab,
//  jab caller ne sahi values di ho).
//
//  Dhyaan rakhne wali baat:
//    Split sirf "hissa" batata hai, "kisko dena hai" NAHI batata. Wo Expense se
//    pata chalta hai — Expense me `paidByUserId` hota hai. Matlab:
//        "split.userId ko, expense.paidByUserId ko, split.amount dena hai"
//    Isi liye jab balance update hota hai (Group::applyExpenseToBalances) to
//    payer khud ka split SKIP kar diya jaata hai — apne aap ko kaun paisa dega?
//
//  Ye class itni chhoti kyun hai?
//    Kyunki iska kaam sirf DATA carry karna hai (ek "value object" / DTO).
//    Sara calculation SplitStrategy karti hai — wo Split objects "banati" hai.
//    Sara balance ka kaam Group/User karte hain — wo Split objects "padhte" hain.
//    Split khud kuch decide nahi karta. Chhoti classes bilkul theek hain, har
//    class me logic ghusaana zaroori nahi.
// ============================================================================
#ifndef SPLITWISE_LLD_MODELS_SPLIT_H
#define SPLITWISE_LLD_MODELS_SPLIT_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {

class Split {
public:
    string userId;  // kis user ka hissa
    double amount;  // us user ka hissa kitne rupaye ka

    // Constructor initializer-list use karta hai (`: userId(userId), ...`).
    // Body ke andar assign karne se better hai — yahan members DIRECTLY construct
    // hote hain, jabki body me pehle default-construct hote aur phir assign hote
    // (ek extra step). String jaise heavy types ke liye ye fark padta hai.
    Split(const string &userId, double amount) : userId(userId), amount(amount) {}
};

}

#endif
