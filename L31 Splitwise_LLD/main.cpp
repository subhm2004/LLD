// ============================================================================
//  main.cpp  —  Splitwise expense-sharing system ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye file poore system ki "kahani" hai. Users banao, group banao, kharche
//  daalo (equal/exact split), balance dekho, edit/delete karo, debts simplify
//  karo, aur aakhir me ek banda group chhod ke jaaye.
//
//  ⭐ CLIENT KITNA KAM JAANTA HAI — ye notice karo:
//     Poore system me 9 classes hain (User, Group, Expense, Split, SplitStrategy
//     × 3, SplitFactory, DebtSimplifier, Observer). Is file me hum kitni classes
//     ko DIRECTLY chhoote hain? Sirf teen naam dikhte hain: Splitwise, User,
//     Group — aur User/Group bhi sirf `->userId` / `->groupId` padhne ke liye.
//     Baaki 6 classes ka naam tak yahan nahi aata.
//
//     Yahi FACADE ka poora point hai. `manager` ek hi darwaza hai; andar Factory
//     strategy bana rahi hai, strategy splits calculate kar rahi hai, Group matrix
//     update kar raha hai, Observers notify ho rahe hain — client ko in me se kisi
//     ka pata bhi nahi chalta.
//
//  ============================================================================
//   IS PROJECT ME 5 DESIGN PATTERNS — ek nazar me
//  ----------------------------------------------------------------------------
//   1. FACADE     -> Splitwise         : poore system ka ek simple front door
//   2. SINGLETON  -> Splitwise         : poore app ka sirf EK data store
//   3. STRATEGY   -> SplitStrategy     : baantne ke 3 tareeke, swappable
//   4. FACTORY    -> SplitFactory      : enum se sahi strategy bana ke deti hai
//   5. OBSERVER   -> Group -> User     : event pe sab members ko notification
//  ============================================================================
#include <bits/stdc++.h>

#include "core/Splitwise.h"

using namespace std;
using namespace splitwise_lld;

int main() {
    // ------------------------------------------------------------------------
    //  SINGLETON — `new Splitwise()` likh hi nahi sakte (constructor private hai).
    //  getInstance() hi ekmatr rasta hai, aur wo hamesha WAHI EK object deta hai.
    //  Kahin bhi, kabhi bhi getInstance() bulao — same object milega, same data.
    // ------------------------------------------------------------------------
    Splitwise *manager = Splitwise::getInstance();

    // ------------------------------------------------------------------------
    //  1) USERS banao — auto ids milti hain: user1, user2, user3, user4
    // ------------------------------------------------------------------------
    User *user1 = manager->createUser("Aditya", "aditya@gmail.com");
    User *user2 = manager->createUser("Rohit", "rohit@gmail.com");
    User *user3 = manager->createUser("Manish", "manish@gmail.com");
    User *user4 = manager->createUser("Saurav", "saurav@gmail.com");

    // ------------------------------------------------------------------------
    //  2) GROUP banao aur chaaro ko add karo
    //  Har addUserToGroup pe console pe "X added to group Y" print hoga, aur
    //  us user ki khaali balance row matrix me ban jaayegi.
    // ------------------------------------------------------------------------
    Group *hostelGroup = manager->createGroup("Hostel Expenses");
    manager->addUserToGroup(user1->userId, hostelGroup->groupId);
    manager->addUserToGroup(user2->userId, hostelGroup->groupId);
    manager->addUserToGroup(user3->userId, hostelGroup->groupId);
    manager->addUserToGroup(user4->userId, hostelGroup->groupId);

    // ------------------------------------------------------------------------
    //  3) EXPENSE #1 — LUNCH: Rs 800, Aditya ne pay kiya, chaaro me EQUAL
    // ------------------------------------------------------------------------
    //  EQUAL strategy: 800 / 4 = har banda 200.
    //  Aditya khud bhi splits me hai (200 ka), par uska split SKIP hota hai —
    //  wo khud ko paisa nahi dega. To baaki teen usko 200-200 denge.
    //
    //  Balance ban jaayega:
    //      Rohit  -> Aditya : 200
    //      Manish -> Aditya : 200
    //      Saurav -> Aditya : 200        (Aditya ka kul lena = 600)
    //
    //  Yahan splitValues bheja hi nahi — EQUAL ko uski zaroorat hi nahi
    //  (SplitStrategy me wo param `= {}` default hai).
    //
    //  Return me expense ki ID milti hai — ise sambhal ke rakho, aage delete
    //  karne ke liye chahiye hogi.
    vector<string> groupMembers = {user1->userId, user2->userId, user3->userId, user4->userId};
    string lunchExpenseId =
        manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0, user1->userId, groupMembers, SplitType::EQUAL);

    // ------------------------------------------------------------------------
    //  4) EXPENSE #2 — DINNER: Rs 700, Manish ne pay kiya, sirf 3 log, EXACT
    // ------------------------------------------------------------------------
    //  ⭐ Dekho: is baar Rohit (user2) shaamil hi NAHI hai — dinner pe wo tha hi
    //  nahi. Involved users ki list alag ho sakti hai group members se.
    //
    //  EXACT strategy: hisse hum khud bata rahe hain, barabar nahi hain —
    //      Aditya : 200   |   Manish : 300   |   Saurav : 200     (= 700 ✓)
    //  (Manish ne mehnga khaya, isi liye uska hissa zyada.)
    //
    //  ⚠ ORDER matter karta hai! dinnerAmounts ka i-th number dinnerMembers ke
    //  i-th bande ka hissa hai. List ka kram badla to hisse galat bandon pe chale
    //  jaayenge — aur code complain bhi nahi karega (ExactSplit me koi validation
    //  nahi hai). Ye is design ki ek asli kamzori hai.
    //
    //  Balance kaise banega — dhyaan se:
    //    Manish payer hai. Aditya usko 250 nahi, 200 dega. Saurav 200 dega.
    //    PAR Aditya ka lunch wala 200 ka lena Manish se PEHLE se pending tha!
    //    To wo dono NET OFF ho jaayenge (200 - 200 = 0) aur entry hi ERASE ho
    //    jaayegi. Ye updateGroupBalance ka kamaal hai — wo `+=` karta hai, aur
    //    zero hone pe entry uda deta hai.
    //
    //  Net picture ab:
    //      Rohit  -> Aditya : 200
    //      Saurav -> Aditya : 200
    //      Saurav -> Manish : 200
    //      (Aditya <-> Manish : barabar, koi entry nahi!)
    vector<string> dinnerMembers = {user1->userId, user3->userId, user4->userId};
    vector<double> dinnerAmounts = {200.0, 300.0, 200.0};
    string dinnerExpenseId = manager->addExpenseToGroup(hostelGroup->groupId, "Dinner", 700.0, user3->userId, dinnerMembers,
                                                        SplitType::EXACT, dinnerAmounts);

    // Pehli baar hisaab dekho — upar wala "net picture" yahan print hoga.
    manager->showGroupBalances(hostelGroup->groupId);

    // ------------------------------------------------------------------------
    //  5) EDIT — dinner 700 se 750 ho gaya (Aditya ka hissa 200 -> 250)
    // ------------------------------------------------------------------------
    //  Andar kya hota hai: purana Expense object DELETE hota hai, naya banta hai
    //  (purani ID ke saath), aur phir POORA matrix zero karke saare bache hue
    //  expenses dobara apply hote hain ("recalculate, don't reverse").
    //  Purane 700 wale hisaab ko "ulta" karne ki koshish nahi hoti — isi liye
    //  yahan bug aane ki gunjaish hi nahi.
    manager->editExpenseInGroup(hostelGroup->groupId, dinnerExpenseId, "Dinner Updated", 750.0, user3->userId,
                                dinnerMembers, SplitType::EXACT, {250.0, 300.0, 200.0});

    // ------------------------------------------------------------------------
    //  6) DELETE — lunch wala expense hata do (galti se add ho gaya tha, maan lo)
    // ------------------------------------------------------------------------
    //  Ab sirf "Dinner Updated" bacha. Matrix dobara zero se banega, aur lunch
    //  ka poora asar apne aap gayab ho jaayega.
    //
    //  Ab bacha hua hisaab (sirf dinner-updated se):
    //      Aditya -> Manish : 250
    //      Saurav -> Manish : 200        (Manish ka kul lena = 450)
    //      Rohit  : bilkul free! (uska lunch wala 200 ka karza uda gaya)
    manager->deleteExpenseInGroup(hostelGroup->groupId, lunchExpenseId);
    manager->showGroupBalances(hostelGroup->groupId);

    // ------------------------------------------------------------------------
    //  7) SIMPLIFY DEBTS — greedy min-transaction algorithm
    // ------------------------------------------------------------------------
    //  Nets: Manish = +450 (creditor), Aditya = -250, Saurav = -200 (debtors),
    //        Rohit = 0 (list se hi drop ho jaata hai — usko kuch karna hi nahi).
    //
    //  Greedy chalega: sabse bada creditor (Manish, 450) vs sabse bada debtor
    //  (Aditya, 250) -> 250 settle. Phir Manish (bacha 200) vs Saurav (200)
    //  -> 200 settle. Done, 2 transactions.
    //
    //  😅 IMANDARI SE: yahan simplify ka koi FARAK nahi padega — natija bilkul
    //  wahi rahega jo pehle tha! Kyunki matrix already minimal thi: sab log ek
    //  hi bande (Manish) ko de rahe the — "star" shape. Isme simplify karne ko
    //  kuch bacha hi nahi.
    //
    //  Simplify ka asli jaadu tab dikhta hai jab CHAIN bane, jaise:
    //      Aditya -> Rohit : 100   aur   Rohit -> Manish : 100
    //  Tab Rohit ka net 0 nikalta hai, wo beech se HAT jaata hai, aur bachta hai
    //  sirf:  Aditya -> Manish : 100.  2 transactions -> 1. 🎯
    //  (Aisa case khud banao aur chala ke dekho — tab algorithm asli me samajh
    //   aayega. DebtSimplifier.h me poora example likha hai.)
    manager->simplifyGroupDebts(hostelGroup->groupId);
    manager->showGroupBalances(hostelGroup->groupId);

    // ------------------------------------------------------------------------
    //  8) INDIVIDUAL EXPENSES — group ke bahar, seedhe do logon ke beech
    // ------------------------------------------------------------------------
    //  ⭐ Ye BILKUL ALAG duniya hai! Ye User::balances ko chhedta hai, group ke
    //  matrix ko nahi. Coffee ka hisaab hostel group me kahin nahi dikhega.
    //
    //  Coffee Rs 40, Rohit ne pay kiya, Saurav ke saath EQUAL -> dono ka 20-20.
    //  Rohit ka apna split skip -> Saurav Rohit ko 20 dega.
    string coffeeExpenseId = manager->addIndividualExpense("Coffee", 40.0, user2->userId, user4->userId, SplitType::EQUAL);

    // Edit: 40 -> 60. Ab Saurav Rohit ko 30 dega (60/2). Andar poora
    // rebuildIndividualBalancesFromExpenses() chalta hai — sab zero, sab dobara.
    manager->editIndividualExpense(coffeeExpenseId, "Coffee Updated", 60.0, user2->userId, user4->userId, SplitType::EQUAL);

    // Delete: ab coffee ka hisaab poora saaf. Saurav-Rohit dono barabar.
    manager->deleteIndividualExpense(coffeeExpenseId);

    // Dobara add — 40 ka. Saurav phir se Rohit ko 20 dega.
    // (Note: naya expense hai to nayi ID milegi — purani `coffeeExpenseId` ab
    //  bekaar hai, isi liye variable ko overwrite kar rahe hain.)
    coffeeExpenseId = manager->addIndividualExpense("Coffee Re-added", 40.0, user2->userId, user4->userId, SplitType::EQUAL);

    // ------------------------------------------------------------------------
    //  9) HAR USER KA INDIVIDUAL BALANCE
    // ------------------------------------------------------------------------
    //  ⚠ CHONKO MAT — Aditya aur Manish dono ka balance Rs 0.00 aayega!
    //  Jabki abhi-abhi group me Aditya ko Manish ko 250 dene hain! To phir 0 kyun?
    //
    //  Kyunki showUserBalance SIRF INDIVIDUAL expenses ka hisaab dikhata hai.
    //  Group ka hisaab Group ke apne matrix me rehta hai (showGroupBalances se
    //  dikhta hai). Dono duniya jaan-boojh ke alag rakhi gayi hain.
    //
    //  Sirf Rohit (+20 lena) aur Saurav (-20 dena) ke numbers dikhenge — kyunki
    //  coffee hi ekmatr individual expense hai.
    //
    //  Ye ek asli DESIGN GAP hai: asli Splitwise dono jod ke ek unified balance
    //  dikhata hai. Ise theek karna ek accha next-step exercise hai.
    manager->showUserBalance(user1->userId);  // Aditya  -> 0.00 / 0.00 (group ka hisaab yahan nahi)
    manager->showUserBalance(user2->userId);  // Rohit   -> others owe you: 20.00
    manager->showUserBalance(user3->userId);  // Manish  -> 0.00 / 0.00 (group ka hisaab yahan nahi)
    manager->showUserBalance(user4->userId);  // Saurav  -> you owe: 20.00

    // ------------------------------------------------------------------------
    //  10) GROUP CHHODNA — "pehle dues clear karo" wala rule
    // ------------------------------------------------------------------------
    //  Rohit nikal payega? HAAN ✅ — kyunki uska GROUP wala hisaab bilkul saaf
    //  hai (lunch delete hone se uska 200 ka karza gayab ho gaya tha).
    //
    //  Uske coffee wale 20 rupaye? Wo INDIVIDUAL hai — group chhodne se uska koi
    //  lena-dena nahi. Isi liye dono duniya alag rakhi thi! Agar dono mila diye
    //  hote, to Rohit ko coffee ke 20 rupaye ke chakkar me hostel group me hi
    //  atka rehna padta — jo bilkul galat hota.
    //
    //  (Try karo: agar Aditya ya Saurav ko remove karne ki koshish karo, to
    //   "User not allowed to leave group without clearing expenses" milega —
    //   kyunki unke group dues abhi bhi pending hain.)
    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

    // Aakhri baar hisaab — ab Rohit matrix me kahin nahi dikhega (uski row bhi
    // gayi, aur doosron ki rows se uska column bhi).
    manager->showGroupBalances(hostelGroup->groupId);

    return 0;
    // Note: yahan koi cleanup nahi kar rahe (Splitwise singleton, users, groups —
    // sab heap pe hain aur delete nahi hote). Program khatam hone pe OS sab memory
    // wapas le leta hai, to practically kuch nahi bigadta. Par technically ye leak
    // hai — production code me proper shutdown / smart pointers hone chahiye.
}
