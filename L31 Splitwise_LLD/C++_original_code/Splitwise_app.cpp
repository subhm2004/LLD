// ============================================================================
//  Splitwise_app.cpp  —  POORA Splitwise system EK HI FILE me (original version)
// ----------------------------------------------------------------------------
//  Ye wahi Splitwise hai jo modular version (../core, ../models, ../strategies
//  waghairah) me alag-alag files me bata hua hai — par yahan sab kuch ek hi
//  file me hai. Padhne ke liye ye aasan hai (upar se neeche ek flow me), par
//  asli project me classes alag files me rakhna behtar hota hai.
//
//  ============================================================================
//   IS FILE ME 5 DESIGN PATTERNS — ek nazar me
//  ----------------------------------------------------------------------------
//   1. SINGLETON  -> Splitwise      : poore app ka sirf EK instance (data store)
//   2. FACADE     -> Splitwise      : client ke liye ek simple darwaza; andar ki
//                                     complexity (Group/User/Split) chhupa deta
//   3. STRATEGY   -> SplitStrategy  : baantne ke 3 tareeke (Equal/Exact/%)
//   4. FACTORY    -> SplitFactory   : SplitType se sahi strategy bana ke deta
//   5. OBSERVER   -> Observer/User  : expense/settlement pe members ko notify
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SIGN CONVENTION — is poore file ka sabse zaroori niyam                │
//  │                                                                          │
//  │  balances[X] > 0  ->  X ko MUJHE dena hai   (X owes me)   -> mera LENA    │
//  │  balances[X] < 0  ->  Mujhe X ko dena hai   (I owe X)     -> mera DENA    │
//  │  balances[X] == 0 ->  hisaab barabar, entry hata do                       │
//  │                                                                          │
//  │  Har karza DO jagah likha jaata hai (mirrored, ulta):                    │
//  │     A.balances[B] = +200   (B ko A ko 200 dene hain)                     │
//  │     B.balances[A] = -200   (wahi baat, B ki taraf se)                    │
//  │                                                                          │
//  │  0.01 wala epsilon: double me 800/3 = 266.66... exact 0 kabhi nahi banta,│
//  │  isi liye "1 paise se kam bacha" = "settled maan lo, entry uda do".      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

// Forward declarations — ye classes neeche define hoti hain, par kuch jagah unka
// naam pehle chahiye. "Ye class aage aayegi" batane ke liye forward declaration.
// (Note: ExpenseManager kabhi define hi nahi hota — asli class ka naam Splitwise
//  hai. Ye ek bacha-khucha declaration hai, koi nuksaan nahi karta.)
class User;
class Group;
class ExpenseManager;

// ============================================================================
//  SplitType (enum) — expense kaise baantna hai, uska "type tag"
// ----------------------------------------------------------------------------
//  Ye client ki zubaan hai. Client bolta hai "mujhe EQUAL split chahiye" — usse
//  ye jaanne ki zaroorat nahi ki andar `EqualSplit` naam ki class hai. Wo bas
//  enum bhejta hai, aur SplitFactory usse sahi strategy bana deti hai.
//
//    EQUAL      : total ko members ki ginti se barabar baant do (800/4 = 200)
//    EXACT      : caller khud batata hai kaun kitna dega, rupayo me [200,300,200]
//    PERCENTAGE : caller % batata hai, amount hum calculate karte hain [50,30,20]%
//
//  ⭐ `enum class` (scoped enum) use hua hai, plain `enum` nahi — aur ye sahi
//  hai. Isse `SplitType::EQUAL` likhna padta hai (naam bahar leak nahi hota),
//  aur ye chupke se `int` me convert nahi hota -> type safety free me.
//  (Is file ke doosre enums ki tarah nahi jo plain hote.)
enum class SplitType { EQUAL, EXACT, PERCENTAGE };

// ============================================================================
//  Split — expense ke EK hisse ka record: "IS user ka hissa ITNA hai"
// ----------------------------------------------------------------------------
//  Bas do field: kaun (userId) aur kitna (amount). Aur kuch nahi.
//
//  Example — 800 ka lunch, 4 log, EQUAL: 4 Split objects banenge, har ek
//  (userId, 200) ke saath. Ek Expense ke andar splits ka vector hota hai.
//
//  Dhyaan: Split "kaun kitna" batata hai, "kisko dena" NAHI — wo Expense se
//  pata chalta hai (`paidByUserId`). Ye ek "value object" / DTO hai — sirf data
//  carry karta hai, koi logic nahi. Sara calculation SplitStrategy karti hai.
class Split {
public:
  string userId; // Kis user ka split hai
  double amount; // Us user ka kitna amount banta hai

  // Initializer-list use karta hai (`: userId(userId), ...`) — members directly
  // construct hote hain (body me assign karne se ek extra step bach jaata hai).
  Split(const string &userId, double amount) : userId(userId), amount(amount) {}
};

// ============================================================================
//  OBSERVER PATTERN (Behavioral) — notification ka interface
// ----------------------------------------------------------------------------
//  Observer = "publisher-subscriber". SUBJECT (Group) ke paas ghatnaayein hoti
//  rehti hain, aur OBSERVERS (Users) ko unki khabar chahiye. Group ko ye jaanne
//  ki zaroorat nahi ki notification KAISE deliver hogi — bas `update()` bolta
//  hai. Aaj console print hai; kal email/SMS/push — Group ka code nahi badlega.
//
//    Subject  -> Group   (expense add/settlement hua)
//    Observer -> User    (har member ko notification milti hai)
class Observer {
public:
  // Pure virtual (`= 0`): koi apna implementation nahi. Jo inherit karega
  // (User) usko `update()` likhna PADEGA — compiler majboor karta hai.
  virtual void update(const string &message) = 0;

  // Virtual destructor — User is interface se inherit karta hai. Agar kabhi
  // `Observer*` se User delete kiya jaaye, to base ka destructor virtual na hone
  // se User ka destructor chalta hi nahi -> undefined behavior. 📌 Rule: ek bhi
  // virtual function hai to destructor bhi virtual hona chahiye.
  virtual ~Observer() {}
};

// ============================================================================
//  STRATEGY PATTERN (Behavioral) — "ek kaam ke kai tareeke, runtime pe chuno"
// ----------------------------------------------------------------------------
//  Kaam ek hi hai: "amount ko members me baanto". Tareeke teen: barabar / exact
//  rupaye / percentage. Har tareeka apni class me.
//
//  ⭐ Bina Strategy ke, ye `if (type == EQUAL) ... else if (EXACT) ...` har us
//  jagah copy-paste hota jahan expense banta hai (Group me, individual me).
//  Naya split type add karo -> har jagah dhoondh ke badlo. Strategy me: ek nayi
//  class + factory me ek line, bas. Yahi OPEN/CLOSED PRINCIPLE hai.
//
//  ⭐ SABKA INTERFACE EK HI: calculateSplit(amount, userIds, values) -> vector<Split>
//  Isi liye caller ko `SplitStrategy*` pakad ke bas `calculateSplit()` bulana
//  hai — andar kaunsi class chal rahi, usse matlab nahi (polymorphism).
class SplitStrategy {
public:
  // `values` ka MATLAB har strategy me alag hai:
  //   EQUAL      -> chahiye hi nahi (khaali {} bhejo)
  //   EXACT      -> rupaye ki list  [200, 300, 200]
  //   PERCENTAGE -> percent ki list [50, 30, 20]
  // Ye ek "leaky" jagah hai design me (ek param ka arth strategy pe depend
  // karta hai). Interview me is trade-off ka zikr karna acha impression banata.
  virtual vector<Split> calculateSplit(double totalAmount,
                                       const vector<string> &userIds,
                                       const vector<double> &values = {}) = 0;

  // Virtual destructor — SplitFactory `SplitStrategy*` return karta hai, aur
  // caller use base pointer se `delete` karta hai. Iske bina derived (EqualSplit
  // waghairah) ka destructor chalta hi nahi -> undefined behavior.
  virtual ~SplitStrategy() {}
};

// ---- EQUAL: sabka hissa barabar (800 rupaye, 4 log -> har ek 200) -----------
class EqualSplit : public SplitStrategy {
public:
  // Teesra param (values) ka NAAM nahi diya — EQUAL ko uski zaroorat hi nahi.
  // Naam na dene se compiler "unused parameter" warning bhi nahi deta, aur
  // padhne wale ko turant signal milta hai ki ye param yahan use nahi hota.
  vector<Split> calculateSplit(double totalAmount,
                               const vector<string> &userIds,
                               const vector<double> & = {}) override {
    vector<Split> splits;
    // Bas ek division. (userIds.size() 0 hua to divide-by-zero -> inf/nan.
    // Yahan safe hai kyunki caller pehle members validate kar chuka hota hai.)
    double amountPerUser = totalAmount / userIds.size();

    for (const string &userId : userIds) {
      splits.push_back(Split(userId, amountPerUser));
    }
    return splits;
  }
};

// ---- EXACT: caller khud batata hai kaun kitna dega --------------------------
//  Kab? Jab hisse barabar nahi. Tumne 300 ka pizza khaya, maine 200 ka burger.
class ExactSplit : public SplitStrategy {
public:
  // Pehla param (totalAmount) ka naam nahi — EXACT me amounts to caller ne khud
  // values me de diye, total ki zaroorat hi nahi.
  vector<Split> calculateSplit(double, const vector<string> &userIds,
                               const vector<double> &values = {}) override {
    vector<Split> splits;

    // i-th user ka hissa = i-th value. POSITIONAL mapping — userIds aur values
    // ka order aur size match hona chahiye. (`size_t` loop -> signed/unsigned
    // compare warning bhi nahi.)
    //
    // 💡 Robust banana ho to: pehle `values.size() == userIds.size()` aur
    // `sum(values) == totalAmount` validate karo, warna out-of-bounds ya galat
    // balance ban sakta hai. Yahan simple rakha hai.
    for (size_t i = 0; i < userIds.size(); i++) {
      splits.push_back(Split(userIds[i], values[i]));
    }
    return splits;
  }
};

// ---- PERCENTAGE: % ke hisaab se (1000 ka bill, [50,30,20]% -> [500,300,200]) --
class PercentageSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount,
                               const vector<string> &userIds,
                               const vector<double> &values = {}) override {
    vector<Split> splits;

    for (size_t i = 0; i < userIds.size(); i++) {
      // Formula: hissa = (total * percent) / 100
      // Ye ExactSplit se sirf itna alag hai ki yahan ek chhoti math hai — baaki
      // structure bilkul same. Yahi Strategy ki khoobsurti: sirf FORMULA badla.
      double amount = (totalAmount * values[i]) / 100.0;
      splits.push_back(Split(userIds[i], amount));
    }
    return splits;
    // 💡 Robust banana ho to: percentages ka sum 100 hona chahiye — validate
    // kar lena. Yahan simple rakha hai.
  }
};

// ============================================================================
//  FACTORY PATTERN (Creational) — SplitType se sahi strategy bana ke deta hai
// ----------------------------------------------------------------------------
//  Strategy ka natural jodidar. Bina factory ke har caller ko ye if-else likhna
//  padta ("EQUAL hai to new EqualSplit(), EXACT hai to..."). Factory se wo logic
//  EK JAGAH. Naya type aaye to sirf yahan ek `case` add karo.
//
//  ⭐ Return type `SplitStrategy*` (BASE pointer) hai — caller ko concrete class
//  (EqualSplit) ka naam pata hi nahi chalta. Yahi to point hai.
//
//  ⭐ MEMORY KA ZIMMA: ye function `new` karta hai, par `delete` NAHI karta —
//  wo CALLER ki zimmedari hai. Isi liye dono call sites (Group::addExpense aur
//  Splitwise::addIndividualExpense) strategy ko use karne ke baad khud `delete`
//  karte hain. Aur SplitStrategy ka destructor virtual hai (upar), isi liye base
//  pointer se delete karna safe hai.
//  (Aur bhi safe tareeka: `unique_ptr<SplitStrategy>` return karna — tab caller
//   ko delete yaad hi na rakhna pade. Yahan raw pointer + manual delete rakha
//   hai taaki ownership saaf dikhe.)
class SplitFactory {
public:
  // `static` -> object banane ki zaroorat nahi; factory ka apna koi state nahi.
  static SplitStrategy *getSplitStrategy(SplitType type) {
    switch (type) {
    case SplitType::EQUAL:
      return new EqualSplit();
    case SplitType::EXACT:
      return new ExactSplit();
    case SplitType::PERCENTAGE:
      return new PercentageSplit();
    default:
      // Defensive fallback — enum me kal naya value aaya aur case bhool gaye to
      // crash ke bajaye EQUAL. (Kuch log yahan `throw` pasand karte — galti
      // chhupe nahi. Dono theek, soch-samajh ke chuno.)
      return new EqualSplit();
    }
  }
};

// ============================================================================
//  User — ek banda: id, naam, email, aur uski personal balance sheet
// ----------------------------------------------------------------------------
//  User do kaam karta hai:
//    1. DATA rakhta hai (userId, name, email, balances)
//    2. OBSERVER banta hai (`: public Observer`) — group notifications le sakta
//
//  ⚠ IMPORTANT — ye `balances` map SIRF INDIVIDUAL (1-to-1) expenses ka hai!
//  Group wale expenses ka hisaab Group ke andar alag `groupBalances` matrix me
//  rehta hai. Dono jaan-boojh ke alag — group chhodte waqt sirf GROUP ka hisaab
//  matter karta hai (tumhare personal coffee ke 40 rupaye group chhodne se nahi
//  rokte). Isi liye main() me showUserBalance sirf coffee dikhata hai, group ka
//  Rs 400 nahi.
class User : public Observer {
public:
  // `static` = poori class ka ek counter (har object ka apna nahi). Har naye
  // user ko unique id: user1, user2... (asli system me DB auto-increment hota).
  static int nextUserId;
  string userId;         // "user1", "user2"... auto-generated, unique
  string name;           // User ka naam
  string email;          // User ki email id

  // ┌── SIGN CONVENTION (upar file-header me detail) ──────────────────────────┐
  // │  key = doosre user ki id, value = signed amount                          │
  // │    > 0  ->  wo MUJHE dega  (mera lena)                                    │
  // │    < 0  ->  main USE dunga (mera dena)                                    │
  // └──────────────────────────────────────────────────────────────────────────┘
  map<string, double> balances;

  User(const string &name, const string &email) {
    // `++nextUserId` PEHLE badhata phir use karta -> pehla user "user1" (0 nahi)
    this->userId = "user" + to_string(++nextUserId);
    this->name = name;
    this->email = email;
  }

  // OBSERVER ka contract: Group event pe ye call hota hai. Har user ise console
  // pe print karta hai. `override` -> compiler check karega signature match ho.
  void update(const string &message) override {
    cout << "[NOTIFICATION to " << name << "]: " << message << endl;
  }

  // Sirf MERI sheet ek taraf update karta hai. Doosre bande ki mirror-entry
  // caller ko khud karni padti hai — isi liye hamesha JODI me call hota hai:
  //   paidByUser->updateBalance(toUser, +amount);   // mera lena badha
  //   toUser->updateBalance(paidByUser, -amount);    // uska dena badha
  void updateBalance(const string &otherUserId, double amount) {
    // `+=` (=` nahi) -> purane hisaab ke UPAR jodo. (map me key na ho to C++
    // khud 0.0 se shuru karta hai, isi liye pehli baar bhi `+=` safe.)
    balances[otherUserId] += amount;

    // Hisaab barabar (1 paise se kam bacha) -> entry uda do, taaki sheet saaf
    // rahe aur floating-point ka kachra na bache.
    if (abs(balances[otherUserId]) < 0.01) {
      balances.erase(otherUserId);
    }
  }

  // "Total mujhe kitna DENA hai" -> sirf NEGATIVE entries jodo, abs() laga ke
  // positive dikhao ("you owe Rs 500" — "-500" dikhana confusing hoga).
  double getTotalOwed() {
    double total = 0;
    for (const auto &balance : balances) {
      if (balance.second < 0) {
        total += abs(balance.second);
      }
    }
    return total;
  }

  // "Total mera kitna LENA hai" -> sirf POSITIVE entries jodo.
  // Note: owe aur owing alag-alag rakhe hain (ek "net" number ke bajaye) kyunki
  // asli Splitwise bhi "you owe X" aur "you are owed Y" alag dikhata hai. Net
  // (+200) dikhana chhupa deta ki tumhe kisi ko dena bhi hai.
  double getTotalOwing() {
    double total = 0;
    for (const auto &balance : balances) {
      if (balance.second > 0) {
        total += balance.second;
      }
    }
    return total;
  }
};
// static member ki asli definition (declaration class ke andar tha). Ye single
// .cpp file hai, isi liye `inline` ki zaroorat nahi (modular headers me thi).
int User::nextUserId = 0;

// ============================================================================
//  Expense — ek kharcha (transaction ka record)
// ----------------------------------------------------------------------------
//  3 sawaalon ka jawab: kya+kitna (description, totalAmount), kisne DIYA
//  (paidByUserId), kis-kis pe BAANTA (splits).
//
//  ⭐ Payer KHUD BHI splits me hota hai (usne bhi to khaya). Par balance banate
//  waqt uska apna split SKIP hota hai (Group::addExpense me `if split.userId !=
//  paidByUserId`) — apne aap ko koi paisa nahi deta.
//
//  Do tarah ka: groupId khaali "" -> individual expense; bhara -> group expense.
class Expense {
public:
  static int nextExpenseId; // User::nextUserId jaisa hi unique-id counter
  string expenseId;         // "expense1", "expense2"...
  string description; // "Lunch", "Dinner", "Coffee"
  double totalAmount; // pura bill
  string paidByUserId;  // kisne counter pe paisa diya
  vector<Split> splits; // kis-kis ka kitna hissa (SplitStrategy ne banaya)
  string groupId; // khaali = individual, bhara = group expense

  Expense(const string &desc, double amount, const string &paidBy,
          vector<Split> &splits, const string group = "") {
    this->expenseId = "expense" + std::to_string(++nextExpenseId);
    this->description = desc;
    this->totalAmount = amount;
    this->paidByUserId = paidBy;
    this->splits = splits;
    this->groupId = group;
  }
};
int Expense::nextExpenseId = 0;

// ============================================================================
//  DebtSimplifier — Splitwise ka HEADLINE feature: "Simplify Debts"
// ----------------------------------------------------------------------------
//  IDEA (ek line): kisi ko farak nahi ki paisa KISKO dena hai — bas uska NET
//  (kul lena/dena) matter karta hai. To saare pairwise karze bhool ke sirf har
//  bande ka NET nikaalo, phir naye sire se sabse kam transfers bana do.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IS PROGRAM KA ASLI EXAMPLE (main() ka output isse match karta hai):  │
//  │                                                                          │
//  │  SIMPLIFY SE PEHLE (4 transactions):                                     │
//  │     Rohit  -> Shubham : 200   |   Saurav -> Shubham : 200                │
//  │     Saurav -> Manish  : 200   |   (Manish ko Saurav se 200 lena)         │
//  │                                                                          │
//  │  NET nikaalo:                                                            │
//  │     Shubham = +400  (creditor)   Manish = +200 (creditor)               │
//  │     Rohit   = -200  (debtor)     Saurav = -400 (debtor)                 │
//  │                                                                          │
//  │  Greedy match (bada creditor vs bada debtor):                           │
//  │     Shubham(400) <-> Saurav(400) : settle 400                           │
//  │     Manish(200)  <-> Rohit(200)  : settle 200                           │
//  │                                                                          │
//  │  SIMPLIFY KE BAAD (still 2, par RESHUFFLED):                            │
//  │     Saurav -> Shubham : 400   |   Rohit -> Manish : 200                  │
//  │                                                                          │
//  │  ⭐ Dhyaan do: Rohit ab SHUBHAM ko nahi, MANISH ko de raha hai! Net sab  │
//  │  ka same hai, par ab har banda kam logon se lena-dena karta hai.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Kyun GREEDY (bada vs bada)? Har transaction me kam se kam EK banda poori tarah
//  settle ho jaata hai (net 0 -> list se bahar). To n logon ke liye zyada se
//  zyada n-1 transactions.
//
//  ⚠ Kya ye HAMESHA absolute minimum deta hai? Nahi — "min transactions" NP-hard
//  hai (subset-sum). Ye greedy ek HEURISTIC hai: hamesha valid, hamesha <= n-1,
//  aur practically optimal ke aas-paas. Interview me exactly yahi bolna.
class DebtSimplifier {
public:
  // Pure function: matrix andar, naya (simplified) matrix bahar. Koi state,
  // koi side-effect nahi -> isi liye `static`, aur test karna aasan.
  // (Param BY VALUE hai -> copy banti hai; original chhedta nahi.)
  static map<string, map<string, double>>
  simplifyDebts(map<string, map<string, double>> groupBalances) {
    // ---- STEP 1: har bande ka NET nikaalo -----------------------------------
    // net = (kul lena) - (kul dena).  > 0 creditor, < 0 debtor, 0 free.
    map<string, double> netAmounts;

    // Sabko pehle 0 se initialize (taaki jinka koi lena-dena nahi, wo bhi map me
    // aayein — net = 0).
    for (const auto &userBalance : groupBalances) {
      netAmounts[userBalance.first] = 0;
    }

    // Matrix ghumo aur nets bharo.
    for (const auto &userBalance : groupBalances) {
      string creditorId =
          userBalance.first; // matrix ki row = ek banda
      for (const auto &balance : userBalance.second) {
        string debtorId =
            balance.first;
        double amount = balance.second;

        // ⚠ SIRF POSITIVE entries dekho — ye chhota `if` bahut zaroori hai.
        // Matrix MIRRORED hai (har karza DO baar likha hai: A[B]=+200, B[A]=-200).
        // Dono padh lete to har amount DOUBLE count hota. Sirf positive side
        // padhne se har karza THEEK EK BAAR ginta hai.
        if (amount > 0) {
          netAmounts[creditorId] += amount; // iska lena badha
          netAmounts[debtorId] -= amount;   // uska dena badha
        }
      }
    }

    // ---- STEP 2: do camps me baanto (creditors vs debtors) ------------------
    vector<pair<string, double>>
        creditors; // net-positive (paise lenge)
    vector<pair<string, double>>
        debtors; // net-negative (paise denge)

    for (const auto &net : netAmounts) {
      if (net.second > 0.01) {
        creditors.push_back({net.first, net.second});
      } else if (net.second < -0.01) {
        // Debtor ka amount POSITIVE bana ke store (`-net.second`) -> aage min()
        // aur subtract ka logic dono lists pe SAME chalta hai, sign ka dhyaan
        // baar-baar nahi rakhna padta. (Aur exact-0 wale apne aap chhut jaate —
        // unhe kuch karna hi nahi.)
        debtors.push_back({net.first, -net.second});
      }
    }

    // ---- STEP 3: bade se chhota sort karo (greedy ka dil) -------------------
    // Lambda `a.second > b.second` -> descending. Bade amount pehle aate hain.
    sort(creditors.begin(), creditors.end(),
         [](const pair<string, double> &a, const pair<string, double> &b) {
           return a.second > b.second;
         });
    sort(debtors.begin(), debtors.end(),
         [](const pair<string, double> &a, const pair<string, double> &b) {
           return a.second > b.second;
         });

    // Naya khaali matrix. Purana poora BHOOL jaate hain — ab sirf nets se fresh
    // transfers banayenge. Har member ki khaali row daal do (taaki settled log
    // bhi matrix me dikhein — "No outstanding balances").
    map<string, map<string, double>> simplifiedBalances;
    for (const auto &userBalance : groupBalances) {
      simplifiedBalances[userBalance.first] = map<string, double>();
    }

    // ---- STEP 4: GREEDY two-pointer matching --------------------------------
    size_t i = 0, j = 0; // i -> creditors me, j -> debtors me (size_t -> signed/
                         // unsigned compare warning bhi nahi)
    while (i < creditors.size() && j < debtors.size()) {
      string creditorId = creditors[i].first;
      string debtorId = debtors[j].first;
      double creditorAmount = creditors[i].second;
      double debtorAmount = debtors[j].second;

      // Jitna ho sake utna settle karo — dono me se JO CHHOTA hai. Kyun min()?
      //   creditor se zyada nahi de sakte (usko utna lena hi nahi)
      //   debtor se zyada nahi le sakte (uske paas utna dena hi nahi)
      // Aur min() lene se DONO me se kam-se-kam EK ka amount 0 ho jaata -> list
      // se nikal jaata -> loop guaranteed aage badhta (infinite loop nahi).
      double settleAmount = min(creditorAmount, debtorAmount);

      // Naya transfer likho, dono taraf (mirrored, same convention):
      simplifiedBalances[creditorId][debtorId] = settleAmount;
      simplifiedBalances[debtorId][creditorId] = -settleAmount;

      // Bacha hua amount kam karo.
      creditors[i].second -= settleAmount;
      debtors[j].second -= settleAmount;

      // Jiska khatam ho gaya (1 paise se kam bacha) use chhod ke aage badho.
      // Note: dono ek saath bhi khatam ho sakte (barabar the) -> dono i++, j++.
      // Isi liye ye do ALAG `if` hain, `if-else` nahi. Classic bug ki jagah.
      if (creditors[i].second < 0.01) {
        i++;
      }
      if (debtors[j].second < 0.01) {
        j++;
      }
    }
    // Loop khatam = ek list poori. Aur kyunki saare nets ka sum 0 hota hai, ek
    // list khatam = DONO khatam. Kuch "bacha hua" nahi rehta.

    return simplifiedBalances;
  }
};

// ============================================================================
//  Group — ek group ("Hostel Expenses"): members + expenses + balance matrix
// ----------------------------------------------------------------------------
//  Group OBSERVER pattern ka SUBJECT hai. Ye 3 cheezein sambhalta hai:
//    1. MEMBERS       -> kaun-kaun group me hai (vector<User*> = observers)
//    2. EXPENSES      -> group ke saare kharche (map<id, Expense*>)
//    3. BALANCE MATRIX-> kaun kisko kitna deta (map<string, map<string,double>>)
//
//  ┌── BALANCE MATRIX — is class ka dil ─────────────────────────────────────┐
//  │  groupBalances[A][B] = +200  ->  "B ko A ko 200 dene hain"              │
//  │  groupBalances[B][A] = -200  ->  wahi baat, B ki taraf se               │
//  │  Har karza DO jagah (mirrored). Fayda: ek row padho, poora hisaab dikhe.│
//  │  Cost: update DO jagah karna padta (updateGroupBalance khud sambhalta). │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Note: is single-file version me expense EDIT/DELETE nahi hai (modular
//  version me tha). Yahan expense add hone ke baad matrix incremental update
//  hota hai. Isi liye "recalculate from scratch" wala pattern yahan nahi dikhta.
class Group {
private:
  // userId se member dhoondho. Jaise hi match mile, turant lauta do (poori list
  // ghumne ki zaroorat nahi — IDs unique hain).
  User *getUserByuserId(string userId) {
    for (User *member : members) {
      if (member->userId == userId) {
        return member;
      }
    }
    return nullptr;
  }

public:
  static int nextGroupId; // Auto-incrementing group ID counter
  string groupId;         // Unique Group ID
  string name;            // Group name (jaise "Trip to Goa")
  vector<User *> members; // Observers list (jo notifications receive karenge)
  map<string, Expense *> groupExpenses; // expenseId -> Expense* (SOURCE OF TRUTH)
  map<string, map<string, double>>
      groupBalances; // memberId -> {otherMemberId -> balance} (DERIVED)

  Group(const string &name) {
    this->groupId = "group" + std::to_string(++nextGroupId);
    this->name = name;
  }

  ~Group() {
    // Group apne expenses ka MAALIK hai -> unhe delete karta hai.
    // ⚠ Note: `members` (Users) ko delete NAHI karta — unke maalik Splitwise
    // class hai. Group unhe sirf POINT karta hai. "Jo banata hai wahi delete."
    for (auto &pair : groupExpenses) {
      delete pair.second;
    }
  }

  void addMember(User *user) {
    members.push_back(user);
    // Naye member ki khaali balance row banao (ab isMember() true dega).
    groupBalances[user->userId] = map<string, double>();
    cout << user->name << " added to group " << name << endl;
  }

  bool removeMember(const string &userId) {
    // RULE: dues clear kiye bina group nahi chhod sakte (asli Splitwise jaisa).
    // Warna karza kahan jaayega? (main() me dikhta hai — Rohit pehle nahi ja
    // paata, phir 200 settle karke jaata hai.)
    if (!canUserLeaveGroup(userId)) {
      cout << "\nUser not allowed to leave group without clearing expenses"
           << endl;
      return false;
    }

    // Members vector se hatao — "erase-remove idiom" (remove peeche dhakelta,
    // erase asal me hataata).
    for (User *user : members) {
      if (user->userId == userId) {
        members.erase(remove(members.begin(), members.end(), user),
                      members.end());
        break;
      }
    }

    // Matrix se DONO taraf se nikalo:
    groupBalances.erase(userId);                    // (1) uski apni row
    for (auto &memberBalance : groupBalances) {     // (2) baakiyon ki rows me
      memberBalance.second.erase(userId);           //     uska column
    }
    // (2) bhool jaate to doosron ki sheet me ek "bhoot" entry reh jaati, aur
    // showGroupBalances usko print karte waqt getUserByuserId se nullptr paata
    // aur ->name pe CRASH kar jaata. Isi liye cleanup poora karna zaroori.
    return true;
  }

  // OBSERVER ka dil — sabko khabar. Loop `Observer*` pe (not `User*`) — batata
  // hai Group ko User se matlab nahi, bas "koi jo update() ka jawab de". Kal
  // EmailNotifier add karo, ye function nahi badlega.
  void notifyMembers(const string &message) {
    for (Observer *observer : members) {
      observer->update(message);
    }
  }

  // Member hai? Matrix ki keys se check (map lookup O(log n), members vector
  // me linear search se faster).
  bool isMember(const string &userId) {
    return groupBalances.find(userId) != groupBalances.end();
  }

  // Matrix update — HAMESHA isi se karo, manually nahi. Dono taraf likhta hai
  // (mirrored invariant), aur settled entries uda deta hai.
  void updateGroupBalance(const string &fromUserId, const string &toUserId,
                          double amount) {
    groupBalances[fromUserId][toUserId] += amount; // from ka lena badha
    groupBalances[toUserId][fromUserId] -= amount; // to ka dena badha

    // Barabar (1 paise se kam) -> entry uda do. Ye sirf saaf-safai nahi —
    // canUserLeaveGroup ISI PE depend karta hai (0.0000001 pada reh jaata to
    // banda kabhi group chhod hi na paata).
    if (abs(groupBalances[fromUserId][toUserId]) < 0.01) {
      groupBalances[fromUserId].erase(toUserId);
    }
    if (abs(groupBalances[toUserId][fromUserId]) < 0.01) {
      groupBalances[toUserId].erase(fromUserId);
    }
  }

  // Group chhod sakta hai? Sirf tab jab uski row me koi non-zero entry na bache.
  bool canUserLeaveGroup(const string &userId) {
    if (!isMember(userId)) {
      throw runtime_error("user is not a part of this group");
    };

    // `const auto&` -> reference se kaam chalate hain, poora map copy nahi karte.
    const auto &userBalanceSheet = groupBalances[userId];
    for (const auto &balance : userBalanceSheet) {
      if (abs(balance.second) > 0.01) {
        return false; // abhi bhi kuch outstanding hai
      }
    }
    return true;
  }

  // User ki is group me balances (bahar ke code ke liye read-only view).
  map<string, double> getUserGroupBalances(const string &userId) {
    if (!isMember(userId)) {
      throw runtime_error("user is not a part of this group");
    };
    return groupBalances[userId];
  }

  // ==========================================================================
  //  addExpense — yahan SAARE PATTERNS ek saath kaam karte dikhte hain
  // ==========================================================================
  bool addExpense(string &description, double amount, string &paidByUserId,
                  vector<string> &involvedUsers, SplitType splitType,
                  const vector<double> &splitValues = {}) {
    // 1. VALIDATION pehle — bahar ka koi banda group expense me nahi ghus sakta.
    if (!isMember(paidByUserId)) {
      throw runtime_error("user is not a part of this group");
    }

    for (const string &userId : involvedUsers) {
      if (!isMember(userId)) {
        throw runtime_error("involvedUsers are not a part of this group");
      }
    }

    // 2. ⭐ FACTORY + STRATEGY: factory enum dekh ke sahi strategy banati hai,
    //    strategy amount ko baant ke Split list deti hai. Group ko pata hi nahi
    //    ki andar EqualSplit chal raha ya PercentageSplit.
    //    Factory ne `new` kiya tha -> use karne ke baad yahin `delete` karte hain
    //    (SplitStrategy ka destructor virtual hai, isi liye base pointer se safe).
    SplitStrategy *strategy = SplitFactory::getSplitStrategy(splitType);
    vector<Split> splits =
        strategy->calculateSplit(amount, involvedUsers, splitValues);
    delete strategy;

    // 3. Expense banao aur group ki book me daalo (SOURCE OF TRUTH).
    Expense *expense =
        new Expense(description, amount, paidByUserId, splits, groupId);
    groupExpenses[expense->expenseId] = expense;

    // 4. Balances update — payer ka apna split SKIP.
    for (Split &split : splits) {
      if (split.userId != paidByUserId) {
        // Payer ka lena badha, doosre ka dena badha (updateGroupBalance dono
        // mirror-entries khud sambhal leta hai).
        updateGroupBalance(paidByUserId, split.userId, split.amount);
      }
    }

    // 5. ⭐ OBSERVER — sabko khabar kar do.
    cout << endl
         << "=========== Sending Notifications ====================" << endl;
    string paidByName = getUserByuserId(paidByUserId)->name;
    notifyMembers("New expense added: " + description + " (Rs " +
                  to_string(amount) + ")");

    // Console printing — sirf tracking/demo ke liye (asli logic nahi).
    cout << endl << "=========== Expense Message ====================" << endl;
    cout << "Expense added to " << name << ": " << description << " (Rs "
         << amount << ") paid by " << paidByName
         << " and involved people are : " << endl;
    // splitValues diye the (EXACT/PERCENTAGE) to naam ke saath value dikhao,
    // warna (EQUAL) bas "Will be Paid Equally".
    if (!splitValues.empty()) {
      for (size_t i = 0; i < splitValues.size(); i++) {
        cout << getUserByuserId(involvedUsers[i])->name << " : "
             << splitValues[i] << endl;
      }
    } else {
      for (string user : involvedUsers) {
        cout << getUserByuserId(user)->name << ", ";
      }
      cout << endl << "Will be Paid Equally" << endl;
    }

    return true;
  }

  // ---- SETTLEMENT — "maine tumhe cash de diya, hisaab kam karo" ------------
  bool settlePayment(string &fromUserId, string &toUserId, double amount) {
    if (!isMember(fromUserId) || !isMember(toUserId)) {
      cout << "user is not a part of this group" << endl;
      return false;
    }

    // Bas matrix update — koi Expense object nahi banta (settlement ek record
    // ke bajaye seedha balance kam kar deta hai). Is version me ye bilkul theek
    // chalta hai — main() me Rohit isi se apna 200 clear karke group chhod paata.
    updateGroupBalance(fromUserId, toUserId, amount);

    string fromName = getUserByuserId(fromUserId)->name;
    string toName = getUserByuserId(toUserId)->name;

    notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " +
                  to_string(amount));

    cout << "Settlement in " << name << ": " << fromName << " settled Rs "
         << amount << " with " << toName << endl;

    return true;
  }

  // Poore group ka hisaab print karo.
  void showGroupBalances() {
    cout << "\n=== Group Balances for " << name << " ===" << endl;

    for (const auto &pair : groupBalances) {
      string memberId = pair.first;
      string memberName = getUserByuserId(memberId)->name;

      cout << memberName << "'s balances in group:" << endl;

      auto userBalances = pair.second;
      if (userBalances.empty()) {
        cout << "  No outstanding balances" << endl; // khaali row = free
      } else {
        for (const auto &userBalance : userBalances) {
          string otherMemberUserId = userBalance.first;
          string otherName = getUserByuserId(otherMemberUserId)->name;

          double balance = userBalance.second;
          // Sign convention: +ve -> wo mujhe dega; -ve -> main use dunga.
          // abs() se user ko "-250" jaisa negative na dikhe.
          if (balance > 0) {
            cout << "  " << otherName << " owes: Rs " << fixed
                 << setprecision(2) << balance << endl;
          } else {
            cout << "  Owes " << otherName << ": Rs " << fixed
                 << setprecision(2) << abs(balance) << endl;
          }
        }
      }
    }
  }

  // Debt simplification chalao. Poora kaam DebtSimplifier ko de diya — Group
  // sirf apna matrix bhejta hai aur naya (simplified) matrix wapas rakh leta hai.
  // Ye SINGLE RESPONSIBILITY ka accha example: greedy algorithm Group ka kaam
  // nahi, uski apni class me rehta hai.
  void simplifyGroupDebts() {
    map<string, map<string, double>> simplifiedBalances =
        DebtSimplifier::simplifyDebts(groupBalances);
    groupBalances = simplifiedBalances;

    cout << "\nDebts have been simplified for group: " << name << endl;
  }
};
int Group::nextGroupId = 0;

// ============================================================================
//  Splitwise — SINGLETON + FACADE: poore system ka ek "front door"
// ----------------------------------------------------------------------------
//  FACADE : andar 6+ classes (User, Group, Expense, Split, SplitStrategy,
//           DebtSimplifier) hain. Client (main) ko sirf EK jaanna hota hai —
//           Splitwise. Ek call, kaam ho gaya; complexity deewar ke peeche.
//
//  SINGLETON : poore app ka data store — saare users, groups. Do instance =
//           disaster (ek me user bana, doosre me hai hi nahi). Isi liye
//           constructor PRIVATE hai — bahar se `new Splitwise()` likha hi nahi
//           ja sakta.
//
//  ⭐ DO ALAG DUNIYA:
//     GROUP expenses     -> Group ke andar, groupBalances ko affect karte
//     INDIVIDUAL expenses -> yahan `expenses` map me, User::balances ko affect
//  Alag kyun? Group chhodte waqt sirf GROUP ka hisaab check hona chahiye
//  (personal coffee ke 40 rupaye group chhodne se nahi rokte).
//
//  ⭐ THREAD-SAFE Singleton — "MAGIC STATIC" se (getInstance ke andar dekho):
//    C++11 guarantee karta hai ki function ke andar ka `static` object exactly
//    EK BAAR, thread-safe tareeke se banega, aur program-end pe apne aap destroy
//    bhi ho jaayega. (Purana `new Splitwise()` + null-check wala tareeka thread-
//    safe nahi tha — do threads ek saath aa jaate to 2 objects ban jaate — aur
//    delete kabhi nahi hota tha, yaani leak. Magic static dono hal kar deta hai.)
class Splitwise {
private:
  map<string, User *> users;   // saare users (userId -> User*)
  map<string, Group *> groups; // saare groups (groupId -> Group*)
  map<string, Expense *>
      expenses; // SIRF individual expenses (group wale Group ke andar rehte)

  Splitwise() {
  } // ⭐ PRIVATE constructor — yahi Singleton ka taala. Bahar se banane band.

public:
  // Object paane ka EKLAUTA rasta.
  static Splitwise *getInstance() {
    // "magic static" — pehli call pe hi banta hai (lazy), thread-safe, aur
    // program-end pe khud destroy. Isi liye koi `static ... *instance` member,
    // manual null-check, ya `new` ki zaroorat hi nahi.
    static Splitwise instance;
    return &instance;
  }

  // Destructor — Splitwise poore app ka data store ka MAALIK hai (users, groups,
  // aur individual expenses). Program-end pe magic-static instance destroy hote
  // waqt ye sab clean kar deta hai. (Purana `new Splitwise()` wala tareeka kabhi
  // destroy hi nahi hota tha, isi liye ye leaks chhupe rehte the.)
  //
  // ⭐ Double-free se bachne ka dhyaan:
  //   - groups delete karne pe har Group ka destructor apne groupExpenses khud
  //     delete karta hai (group wale expenses ka maalik Group hai).
  //   - `expenses` map me SIRF individual expenses hain (group wale nahi) ->
  //     koi overlap nahi.
  //   - Users kisi bhi Group ke members me sirf POINTER hain; Group unhe delete
  //     nahi karta -> unka ekmatr maalik yahi (`users`) hai.
  ~Splitwise() {
    for (auto &p : users) delete p.second;
    for (auto &p : groups) delete p.second;    // Group::~Group group expenses hataata
    for (auto &p : expenses) delete p.second;  // individual expenses
  }

  // Naya User create karna.
  User *createUser(string name, string email) {
    User *user = new User(name, email);
    users[user->userId] = user;
    cout << "User created: " << name << " (ID: " << user->userId << ")" << endl;
    return user;
  }

  User *getUser(const string &userId) {
    auto it = users.find(userId);
    return (it != users.end()) ? it->second : nullptr;
  }

  // Naya Group create karna.
  Group *createGroup(const string name) {
    Group *group = new Group(name);
    groups[group->groupId] = group;
    cout << "Group created: " << name << " (ID: " << group->groupId << ")"
         << endl;
    return group;
  }

  Group *getGroup(const string &groupId) {
    auto it = groups.find(groupId);
    return (it != groups.end()) ? it->second : nullptr;
  }

  // ---- FACADE ke thin wrappers -------------------------------------------
  // Neeche ke kai functions bas id se object dhoondh ke kaam Group ko FORWARD
  // karte hain. Splitwise khud kuch nahi karta. Fayda: client ke paas Group ka
  // pointer na bhi ho, sirf groupId se kaam chal jaata hai.

  // Kisi user ko group me add karne ka portal.
  void addUserToGroup(const string &userId, const string &groupId) {
    User *user = getUser(userId);
    Group *group = getGroup(groupId);

    if (user && group) { // dono valid ho tabhi
      group->addMember(user);
    }
  }

  // User ko group se leave karana.
  bool removeUserFromGroup(const string &userId, const string &groupId) {
    Group *group = getGroup(groupId);

    if (!group) {
      cout << "Group not found!" << endl;
      return false;
    }

    User *user = getUser(userId);
    if (!user) {
      cout << "User not found!" << endl;
      return false;
    }

    bool userRemoved = group->removeMember(userId);

    if (userRemoved) {
      cout << user->name << " successfully left " << group->name << endl;
    }
    return userRemoved;
  }

  // Group Expense add karna.
  void addExpenseToGroup(string &groupId, string description, double amount,
                         string &paidByUserId, vector<string> &involvedUsers,
                         SplitType splitType,
                         const vector<double> &splitValues = {}) {

    Group *group = getGroup(groupId);
    if (!group) {
      cout << "Group not found!" << endl;
      return;
    }

    group->addExpense(description, amount, paidByUserId, involvedUsers,
                      splitType, splitValues);
  }

  // Group payment settle karna.
  void settlePaymentInGroup(string &groupId, string &fromUserId,
                            string &toUserId, double amount) {

    Group *group = getGroup(groupId);
    if (!group) {
      cout << "Group not found!" << endl;
      return;
    }

    group->settlePayment(fromUserId, toUserId, amount);
  }

  // Bina group ke individual payment settle karna (User::balances pe, mirrored).
  void settleIndividualPayment(string &fromUserId, string &toUserId,
                               double amount) {
    User *fromUser = getUser(fromUserId);
    User *toUser = getUser(toUserId);

    if (fromUser && toUser) {
      fromUser->updateBalance(toUserId, amount);
      toUser->updateBalance(fromUserId, -amount);

      cout << fromUser->name << " settled Rs" << amount << " with "
           << toUser->name << endl;
    }
  }

  // ---- INDIVIDUAL (1-to-1) EXPENSE — Splitwise KHUD handle karta hai --------
  void addIndividualExpense(string description, double amount,
                            string paidByUserId, string toUserId,
                            SplitType splitType,
                            const vector<double> &splitValues = {}) {

    User *paidByUser = getUser(paidByUserId);
    User *toUser = getUser(toUserId);

    // Pehle users validate karo — galat id par crash na ho (Group wale flow me
    // ye check tha, yahan bhi hona chahiye).
    if (!paidByUser || !toUser) {
      cout << "User not found! Individual expense not added." << endl;
      return;
    }

    // Wahi Factory + Strategy jodi. Yahan "members" sirf DO: payer aur doosra.
    // Factory ne `new` kiya -> use karne ke baad `delete` (destructor virtual hai).
    SplitStrategy *strategy = SplitFactory::getSplitStrategy(splitType);
    vector<Split> splits =
        strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);
    delete strategy;

    // groupId nahi diya -> default "" -> INDIVIDUAL expense.
    Expense *expense = new Expense(description, amount, paidByUserId, splits);
    expenses[expense->expenseId] = expense;

    // Balance dono taraf (mirrored) — sign convention: payer ka lena +, doosre
    // ka dena -. Note: yahan `amount` (poora) use hua, `split.amount` nahi —
    // 1-to-1 me "toUser poora amount owe karta hai payer ko". EQUAL split ka
    // asar yahan nahi (is 40 ke coffee me toUser poore 40 owe karta, 20 nahi).
    paidByUser->updateBalance(toUserId, amount);
    toUser->updateBalance(paidByUserId, -amount);

    cout << "Individual expense added: " << description << " (Rs " << amount
         << ") paid by " << paidByUser->name << " for " << toUser->name << endl;
  }

  // User ki overall balance details print karna.
  // Note: ye JAAN-BOOJH ke sirf individual expenses dikhata hai (User::balances).
  // Group ka hisaab alag rakha gaya hai (Group::groupBalances) — isi liye main()
  // me Shubham/Manish ka yahan "Total: 0.00" aata hai, jabki group me unka
  // lena-dena hai. Dono duniya alag hone ki wajah upar Splitwise class me likhi
  // hai (group chhodne wala rule isi separation pe kaam karta hai).
  void showUserBalance(string &userId) {
    User *user = getUser(userId);
    if (!user)
      return;

    cout << endl
         << "=========== Balance for " << user->name
         << " ====================" << endl;
    // `fixed << setprecision(2)` -> hamesha 2 decimal (paise). Iske bina paison
    // ke liye "266.667" ya "1e+03" jaisa galat output aa sakta.
    cout << "Total you owe: Rs " << fixed << setprecision(2)
         << user->getTotalOwed() << endl;
    cout << "Total others owe you: Rs " << fixed << setprecision(2)
         << user->getTotalOwing() << endl;

    cout << "Detailed balances:" << endl;
    for (auto &balance : user->balances) {
      User *otherUser = getUser(balance.first);
      if (otherUser) {
        if (balance.second > 0) {
          cout << "  " << otherUser->name << " owes you: Rs" << balance.second
               << endl;
        } else {
          cout << "  You owe " << otherUser->name << ": Rs"
               << abs(balance.second) << endl;
        }
      }
    }
  }

  // Group ki pure members ki balance print karna.
  void showGroupBalances(string &groupId) {
    Group *group = getGroup(groupId);
    if (!group)
      return;

    group->showGroupBalances();
  }

  // Group ke debts simplify karne ke liye request forward karna.
  void simplifyGroupDebts(string &groupId) {
    Group *group = getGroup(groupId);
    if (!group)
      return;

    // Use group's balance data for debt simplification
    group->simplifyGroupDebts();
  }
};

// ============================================================================
//  main() — poore system ki "kahani" (CLIENT)
// ----------------------------------------------------------------------------
//  ⭐ Notice: client sirf `manager` (Splitwise) se baat karta hai. Andar Factory
//  strategy bana rahi, strategy splits calculate kar rahi, Group matrix update
//  kar raha, Observers notify ho rahe — client ko in me se kisi ka pata bhi
//  nahi. Yahi FACADE ka poora point hai.
//
//  Is demo ka flow (output isse match karta hai):
//    4 users -> group -> Lunch (800, EQUAL) + Dinner (700, EXACT) ->
//    balances dekho -> simplify (Rohit ab Shubham ke bajaye Manish ko dega) ->
//    individual Coffee (40) -> user balances -> Rohit ko group se nikaalne ki
//    koshish (fail, dues bache) -> settlement -> phir se koshish (safal).
// ============================================================================
int main() {
  Splitwise *manager = Splitwise::getInstance();

  cout << endl << "=========== Creating Users ====================" << endl;
  User *user1 = manager->createUser("Shubham", "Shubham@gmail.com");
  User *user2 = manager->createUser("Rohit", "rohit@gmail.com");
  User *user3 = manager->createUser("Manish", "manish@gmail.com");
  User *user4 = manager->createUser("Saurav", "saurav@gmail.com");

  cout << endl
       << "=========== Creating Group and Adding Members ===================="
       << endl;
  Group *hostelGroup = manager->createGroup("Hostel Expenses");
  manager->addUserToGroup(user1->userId, hostelGroup->groupId);
  manager->addUserToGroup(user2->userId, hostelGroup->groupId);
  manager->addUserToGroup(user3->userId, hostelGroup->groupId);
  manager->addUserToGroup(user4->userId, hostelGroup->groupId);

  cout << endl
       << "=========== Adding Expenses in group ====================" << endl;
  // LUNCH: 800, Shubham ne pay kiya, chaaro me EQUAL (har ek 200). Shubham ka
  // apna split skip -> baaki 3 usko 200-200 denge.
  vector<string> groupMembers = {user1->userId, user2->userId, user3->userId,
                                 user4->userId};
  manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0,
                             user1->userId, groupMembers, SplitType::EQUAL);

  // DINNER: 700, Manish ne pay kiya, sirf 3 log (Rohit nahi!), EXACT split.
  // ⭐ Involved list group members se ALAG ho sakti hai. Amounts khud diye:
  // Shubham 200, Manish 300, Saurav 200 (sum = 700).
  vector<string> dinnerMembers = {user1->userId, user3->userId, user4->userId};
  vector<double> dinnerAmounts = {200.0, 300.0, 200.0};
  manager->addExpenseToGroup(hostelGroup->groupId, "Dinner", 700.0,
                             user3->userId, dinnerMembers, SplitType::EXACT,
                             dinnerAmounts);

  cout << endl
       << "=========== printing Group-Specific Balances ===================="
       << endl;
  manager->showGroupBalances(hostelGroup->groupId);

  cout << endl
       << "=========== Debt Simplification ====================" << endl;
  manager->simplifyGroupDebts(hostelGroup->groupId);

  cout << endl
       << "=========== printing Group-Specific Balances ===================="
       << endl;
  manager->showGroupBalances(hostelGroup->groupId);

  cout << endl
       << "=========== Adding Individual Expense ====================" << endl;
  manager->addIndividualExpense("Coffee", 40.0, user2->userId, user4->userId,
                                SplitType::EQUAL);

  cout << endl
       << "=========== printing User Balances ====================" << endl;
  manager->showUserBalance(user1->userId);
  manager->showUserBalance(user2->userId);
  manager->showUserBalance(user3->userId);
  manager->showUserBalance(user4->userId);

  // ⭐ Rohit ko nikaalne ki koshish — FAIL hoga! Simplify ke baad Rohit ko
  // Manish ko 200 dene hain (dues bache hain). canUserLeaveGroup false dega.
  cout << endl
       << "==========Attempting to remove Rohit from group==========" << endl;
  manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

  // Rohit apna 200 ka karza Manish ko chuka deta hai -> uska balance 0.
  cout << endl
       << "======== Making Settlement to Clear Rohit's Debt ==========" << endl;
  manager->settlePaymentInGroup(hostelGroup->groupId, user2->userId,
                                user3->userId, 200.0);

  // ⭐ Ab dobara koshish — SAFAL! Rohit ke dues clear ho gaye, ab ja sakta hai.
  // (Balance clear hone ka rule kaam kar raha hai — yahi is demo ka asli point.)
  cout << endl
       << "======== Attempting to Remove Rohit Again ==========" << endl;
  manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

  cout << endl
       << "=========== Updated Group Balances ====================" << endl;
  manager->showGroupBalances(hostelGroup->groupId);

  return 0;
}