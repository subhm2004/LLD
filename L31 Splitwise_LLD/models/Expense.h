// ============================================================================
//  models/Expense.h  —  Ek kharcha (transaction ka record)
// ----------------------------------------------------------------------------
//  Expense me 3 hi sawaalon ka jawab hota hai:
//     1. Kya kharch hua aur kitna?     -> description, totalAmount
//     2. Paisa kisne DIYA (counter pe)? -> paidByUserId
//     3. Wo paisa kis-kis pe BAANTA?    -> splits (vector<Split>)
//
//  Example — Aditya ne 800 ka lunch pay kiya, 4 log the (EQUAL):
//      description   = "Lunch"
//      totalAmount   = 800
//      paidByUserId  = "user1" (Aditya)
//      splits        = [ (user1,200), (user2,200), (user3,200), (user4,200) ]
//      groupId       = "group1"
//
//  Dhyaan do: payer (Aditya) KHUD BHI splits me hai, 200 ke saath. Ye jaan-boojh
//  ke hai — uska bhi to lunch me hissa tha! Lekin balance calculate karte waqt
//  uska apna split SKIP kar diya jaata hai (Group::applyExpenseToBalances me
//  `if (split.userId != expense->paidByUserId)`), kyunki apne aap ko koi paisa
//  nahi deta. Net effect: baaki 3 log Aditya ko 200-200 denge = 600.
//  Aditya ne 800 diye, 200 uska apna tha -> 600 wapas milne chahiye. Sahi hai.
//
//  Ek Expense DO tarah ka ho sakta hai — pehchaan `groupId` se hoti hai:
//      groupId khaali ("")  -> INDIVIDUAL expense (do logon ke beech, 1-to-1).
//                              Splitwise class ke `expenses` map me rehta hai,
//                              aur User::balances ko affect karta hai.
//      groupId bhara hua    -> GROUP expense.
//                              Group ke `groupExpenses` map me rehta hai,
//                              aur Group::groupBalances matrix ko affect karta.
//
//  IMMUTABLE-ish design — edit karne ka tareeka:
//    Is class me koi setter nahi hai. Expense edit karne par purana object
//    DELETE karke naya banaya jaata hai, aur uspe purani expenseId chipka di
//    jaati hai (Group::editExpense dekho). Isse balance history hamesha expense
//    list se DOBARA calculate ki ja sakti hai — "source of truth" ek hi jagah
//    rehta hai (expenses ki list), balances usse DERIVED value hain.
//    Isi liye edit/delete ke baad `recalculateBalancesFromExpenses()` chalta hai
//    — purane balance ko "ulta" karne (reverse karne) ki jhanjhat hi nahi.
// ============================================================================
#ifndef SPLITWISE_LLD_MODELS_EXPENSE_H
#define SPLITWISE_LLD_MODELS_EXPENSE_H

#include <bits/stdc++.h>

#include "Split.h"

using namespace std;

namespace splitwise_lld {

class Expense {
public:
    static int nextExpenseId;  // User::nextUserId jaisa hi — unique id counter

    string expenseId;      // "expense1", "expense2", ...
    string description;    // "Lunch", "Dinner", "Coffee"
    double totalAmount;    // pura bill
    string paidByUserId;   // kisne counter pe paisa diya
    vector<Split> splits;  // kis-kis ka kitna hissa (SplitStrategy ne banaya)
    string groupId;        // khaali = individual expense, warna group expense

    Expense(const string &description, double amount, const string &paidByUserId, vector<Split> &splits,
            const string &groupId = "")  // default "" -> individual expense
        : description(description), totalAmount(amount), paidByUserId(paidByUserId), splits(splits), groupId(groupId) {
        expenseId = "expense" + to_string(++nextExpenseId);
    }
    // Note: edit ke case me caller is auto-generated id ko OVERWRITE kar deta hai
    // purani id se (`newExpense->expenseId = expenseId;`), taaki bahar ke code ke
    // paas jo id pehle se hai wo valid rahe. Counter phir bhi aage badh jaata hai
    // — bas kuch id numbers skip ho jaate hain, koi nuksaan nahi.
};

inline int Expense::nextExpenseId = 0;

}

#endif
