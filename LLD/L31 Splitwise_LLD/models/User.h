// ============================================================================
//  models/User.h  —  Ek banda: id, naam, email, aur uski personal balance sheet
// ----------------------------------------------------------------------------
//  User do kaam karta hai:
//    1. DATA rakhta hai   -> userId, name, email, balances
//    2. OBSERVER banta hai -> `: public Observer` — yaani group notifications
//                             receive kar sakta hai (update() implement karke)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  SIGN CONVENTION — is poore project ka sabse important rule. Ise samajh   │
//  │  liya to Group.h aur DebtSimplifier.h dono aasan lagenge.                │
//  ├──────────────────────────────────────────────────────────────────────────┤
//  │  balances[X] ka matlab: "mere aur X ke beech ka net hisaab"              │
//  │                                                                          │
//  │     balances[X] > 0   ->  X mujhe DENA hai   (X owes me)   -> mera LENA  │
//  │     balances[X] < 0   ->  Mujhe X ko DENA hai (I owe X)    -> mera DENA  │
//  │     balances[X] == 0  ->  hisaab barabar, entry hata do                  │
//  │                                                                          │
//  │  Ye hamesha MIRRORED hota hai (do jagah likha jaata hai):                │
//  │     Aditya.balances["rohit"]  = +200   (Rohit ko 200 dene hain Aditya ko)│
//  │     Rohit.balances["aditya"]  = -200   (Rohit ke upar 200 ka karza hai)  │
//  │  Dono hamesha ek dusre ke ulta (negative) rehte hain.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IMPORTANT — ye `balances` map SIRF INDIVIDUAL (1-to-1) expenses ka hai!
//    Group wale expenses ka hisaab Group class ke andar alag `groupBalances`
//    matrix me rehta hai. Dono jaan-boojh ke alag rakhe gaye hain, kyunki group
//    chhodte waqt (removeMember) sirf GROUP ka hisaab check karna hota hai —
//    tumhare personal coffee ke 40 rupaye group chhodne se nahi rokte.
//
//  0.01 ka epsilon baar-baar kyun dikhta hai?
//    `double` me floating-point error hoti hai. 800/3 = 266.666...
//    Teeno ke hisse jodo to exact 800 nahi, 799.9999999 ya 800.0000001 aata hai.
//    To balance kabhi bhi theek 0 nahi banta — 0.0000001 bach jaata hai.
//    Isi liye "1 paise se kam bacha hai" = "settled maan lo" — entry hata do.
//    Warna balance sheet kachre se bhar jaayegi jisme Rs 0.00 likha hoga.
// ============================================================================
#ifndef SPLITWISE_LLD_MODELS_USER_H
#define SPLITWISE_LLD_MODELS_USER_H

#include <bits/stdc++.h>

#include "../observers/Observer.h"

using namespace std;

namespace splitwise_lld {

// `: public Observer` -> User ek Observer HAI. Isi liye Group apne members ko
// `vector<User*>` me rakh ke bhi `Observer*` ki tarah treat kar sakta hai
// (Group::notifyMembers dekho). Ye polymorphism ka classic use hai.
class User : public Observer {
public:
    // `static` = ye variable poori class ka hai, har object ka apna nahi.
    // Sabhi User objects isi EK counter ko share karte hain -> har naye user ko
    // unique id milti hai. (Real system me ye DB ka auto-increment hota.)
    static int nextUserId;

    string userId;  // "user1", "user2", ... — auto-generated, unique
    string name;
    string email;

    // Kisko kitna dena/lena — sirf INDIVIDUAL expenses ka (upar ka note padho).
    // key = doosre user ki id, value = signed amount (+ = wo mujhe dega,
    //                                                 - = main usko dunga)
    map<string, double> balances;

    User(const string &name, const string &email) : name(name), email(email) {
        // `++nextUserId` PEHLE badhata hai phir use karta hai -> pehla user
        // "user1" banega (0 nahi). Har naye object pe counter aage badhta hai.
        userId = "user" + to_string(++nextUserId);
    }

    // ---- OBSERVER ka contract ----------------------------------------------
    // Group jab bhi kuch hota hai (expense add/edit/delete, settlement), sabhi
    // members ka ye function call karta hai. `override` keyword compiler se
    // check karwata hai ki signature base class se exactly match kar rahi hai —
    // typo hui (jaise `Update` ya galat param) to compile-time error mil jayega,
    // silently ek naya function banne ke bajaye.
    void update(const string &message) override {
        cout << "[NOTIFICATION to " << name << "]: " << message << endl;
    }

    // ---- Balance ka ek taraf ka update -------------------------------------
    // Sirf MERI sheet update karta hai. Dusre banda ki sheet (mirror entry)
    // caller ko khud update karni padegi — Splitwise.h me dekho, wahan hamesha
    // JODI me call hota hai:
    //     paidByUser->updateBalance(toUser,  +amount);   // mera lena badha
    //     toUser->updateBalance(paidByUser,  -amount);   // uska dena badha
    void updateBalance(const string &otherUserId, double amount) {
        // `+=` kyun, `=` kyun nahi? Kyunki purane hisaab ke UPAR jodna hai.
        // Pehle se 100 ka lena tha, 50 aur ka expense hua -> ab 150 ka lena.
        // (map me key na ho to C++ khud 0.0 se shuru karta hai — isi liye
        //  pehli baar bhi `+=` safe hai, koi "key exists?" check nahi chahiye.)
        balances[otherUserId] += amount;

        // Hisaab barabar ho gaya (1 paise se kam bacha) -> entry hi uda do,
        // taaki balance sheet saaf rahe aur canUserLeaveGroup jaise checks
        // floating-point ke kachre pe atak na jaayein.
        if (abs(balances[otherUserId]) < 0.01) balances.erase(otherUserId);
    }

    // ---- "Total mujhe kitna DENA hai" (my debt) -----------------------------
    // Sirf NEGATIVE entries jodo (negative = main doosron ka karzdaar hu),
    // aur abs() laga ke positive number me dikhao (user ko "-500 owe" dikhana
    // confusing hoga, "you owe Rs 500" clean hai).
    double getTotalOwed() const {
        double total = 0;
        for (const auto &balance : balances) if (balance.second < 0) total += abs(balance.second);
        return total;
    }

    // ---- "Total mera kitna LENA hai" (others' debt to me) -------------------
    // Ulta case: sirf POSITIVE entries jodo.
    //
    // Note: ye dono alag-alag rakhe gaye hain (ek single "net" number ke bajaye)
    // kyunki asli Splitwise bhi yahi dikhata hai — "you owe Rs 300" aur "you are
    // owed Rs 500" alag lines me. Net (+200) dikhana information chhupa deta hai:
    // usse pata hi nahi chalta ki tumhe kisi ko paisa dena bhi hai.
    double getTotalOwing() const {
        double total = 0;
        for (const auto &balance : balances) if (balance.second > 0) total += balance.second;
        return total;
    }
};

// static member ki asli definition (declaration class ke andar tha, upar).
// `inline` isliye lagaya hai taaki ye header agar 2 alag .cpp me include ho to
// linker "duplicate symbol" ka error na de. (C++17 se ye trick allowed hai —
// warna alag .cpp file me `int User::nextUserId = 0;` likhna padta.)
inline int User::nextUserId = 0;

}

#endif
