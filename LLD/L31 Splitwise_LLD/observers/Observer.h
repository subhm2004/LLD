// ============================================================================
//  observers/Observer.h  —  OBSERVER PATTERN (Behavioral) ka interface
// ----------------------------------------------------------------------------
//  Observer = "publisher-subscriber". Ek SUBJECT hota hai jiske paas kuch hota
//  rehta hai, aur kai OBSERVERS hote hain jinko har ghatna (event) ki khabar
//  chahiye. Subject ko ye nahi pata hota ki observers kaun hain — bas unki list
//  rakhta hai aur event pe sabko `update()` bol deta hai.
//
//  Is project me mapping:
//    Subject  -> Group   (expense add/edit/delete hua, settlement hua)
//    Observer -> User    (har group member ko notification milti hai)
//
//  Flow ek line me:
//    Group::addExpense() -> notifyMembers("New expense added: Lunch")
//                        -> har member ka User::update() chalta hai
//                        -> console pe "[NOTIFICATION to Aditya]: ..." print
//
//  Isse fayda kya?
//    Group ko ye jaanne ki zaroorat NAHI ki notification kaise deliver hogi.
//    Aaj console print hai. Kal email bhejna ho, ya push notification, ya SMS —
//    Group ka code ek line bhi nahi badlega. Bas ek naya class banao jo Observer
//    ko implement kare (EmailNotifier : public Observer) aur group me add kar do.
//    Yahi "loose coupling" hai — Group sirf INTERFACE se baat karta hai, kisi
//    concrete class se nahi.
//
//  Ye "pure abstract class" (Java ka interface) kaise bani?
//    - `= 0` lagane se function PURE VIRTUAL ban jaata hai. Matlab: is class ka
//      apna koi implementation nahi hai, aur is class ka object DIRECTLY banaya
//      hi nahi ja sakta. Jo bhi inherit karega, usko `update()` likhna PADEGA.
// ============================================================================
#ifndef SPLITWISE_LLD_OBSERVERS_OBSERVER_H
#define SPLITWISE_LLD_OBSERVERS_OBSERVER_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {

class Observer {
public:
    // Subject (Group) event hone par ye call karta hai. Message string me event
    // ki detail hoti hai. Har concrete observer apne hisaab se handle karega —
    // User ise console pe print karta hai.
    virtual void update(const string &message) = 0;

    // Virtual destructor — base-class pointer (Observer*) se derived object
    // (User*) delete karne par ye ZAROORI hai. Warna sirf Observer ka
    // destructor chalega, User ka nahi -> User ke members (name, balances map)
    // leak ho jayenge. Rule of thumb: agar class me ek bhi virtual function hai,
    // to destructor bhi virtual hona chahiye.
    virtual ~Observer() {}
};

}

#endif
