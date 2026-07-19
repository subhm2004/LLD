# Splitwise App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L31 Splitwise_LLD/C++_original_code/Splitwise_app.cpp` file me use hone wale sabhi design patterns, core algorithm (Debt Simplification), classes ki responsibilities aur important design decisions ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Yeh application ek classic Low-Level Design (LLD) problem (Splitwise — group expense sharing) ko solve karti hai. Isme system ko scalable, maintainable aur decoupled banane ke liye **5 major Design Patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Splitwise App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Group me expense add/settle hone par sabhi members (observers) ko automatically notification bhejne ke liye. |
| **2. Strategy Pattern** | Behavioral | Split calculate karne ke 3 tareeke (EQUAL / EXACT / PERCENTAGE) ko alag-alag classes me encapsulate karke runtime par switch karne ke liye. |
| **3. Factory Pattern** | Creational | `SplitType` enum ke basis par sahi Strategy object create karke dene ke liye — creation logic ek jagah centralize ho jata hai. |
| **4. Singleton Pattern** | Creational | Poore application me `Splitwise` data-store/controller ka sirf **ek** instance rakhne ke liye (thread-safe "magic static" se). |
| **5. Facade Pattern** | Structural | Subsystems (Group, User, Expense, Strategy, Factory) ki complexity chhupa kar client (`main()`) ko simple methods dene ke liye. |

Iske alawa ek **core algorithm** bhi hai jo GoF pattern nahi hai par app ka dil hai: **Debt Simplification (Greedy)** — [DebtSimplifier](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L405-L518). Neeche detail me explain kiya hai.

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern

#### **Kyu use kiya gaya? (Intent)**
Group me jab bhi koi member expense add karta hai, payment settle karta hai, ya koi member join/leave karta hai — to baaki sabhi members ko iski khabar milni chahiye. Agar ye notification logic `Group` ke har method me haath se likhte, to Group aur User tightly coupled ho jate.

Observer pattern se `Group` (Subject) bas `notifyMembers(msg)` bolta hai, aur members ki poori list ka `update()` apne aap call ho jata hai — **one-to-many dependency, loose coupling ke saath**.

#### **Implementation in Code:**
* **Observer Interface:** [Observer](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L99-L110) — abstract class jisme pure virtual `update(message)` hai. Virtual destructor bhi hai (rule: ek bhi virtual function hai to destructor bhi virtual hona chahiye).
* **Concrete Observer:** [User](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L261) — `Observer` se inherit karta hai aur [update()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L286) me notification print karta hai.
* **Subject (Observable):** [Group](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L538) — `members` (vector<User*>) hi observers ki list hai; [notifyMembers()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L615) broadcast karta hai.

```cpp
// Observer Interface (Line 99-110)
class Observer {
public:
  virtual void update(const string &message) = 0;
  virtual ~Observer() {}
};

// Concrete Observer (Line 261, update at 286)
class User : public Observer {
public:
  void update(const string &message) override {
    cout << "[NOTIFICATION to " << name << "]: " << message << endl;
  }
};

// Subject ke andar broadcast (Line 615)
void notifyMembers(const string &message) {
  for (Observer *observer : members) {
    observer->update(message);
  }
}
```

#### **Kaise extend karein?**
`EmailNotifier` ya `PushNotifier` jaisi nayi Observer subclass banao — `Group` ka code same rahega. (Abhi User hi observer hai; production me notification channel alag observer hote.)

---

### 2. Strategy Design Pattern

#### **Kyu use kiya gaya? (Intent)**
Kaam ek hi hai — "amount ko members me baanto" — par tareeke teen hain:
- **EQUAL**: 800 rupaye, 4 log → har ek 200.
- **EXACT**: caller khud batata hai kaun kitna dega — [200, 300, 200].
- **PERCENTAGE**: caller % batata hai — 1000 ka bill, [50, 30, 20]% → [500, 300, 200].

Bina Strategy ke ye `if (type == EQUAL) ... else if (EXACT) ...` **har us jagah** copy-paste hota jahan expense banta hai (group me bhi, individual me bhi). Naya split type aane par har jagah dhoondh ke badalna padta. Strategy me: **ek nayi class + factory me ek line, bas** — yahi Open/Closed Principle hai.

#### **Implementation in Code:**
* **Strategy Interface:** [SplitStrategy](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L126-L142) — common signature `calculateSplit(totalAmount, userIds, values) -> vector<Split>`.
* **Concrete Strategies:**
  1. [EqualSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L145-L163): `total / userIds.size()` — bas ek division; `values` param use hi nahi hota.
  2. [ExactSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L167-L187): i-th user ka hissa = i-th value (**positional mapping** — order aur size match hona chahiye).
  3. [PercentageSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L190-L208): `(total * percent) / 100` — ExactSplit se sirf formula alag hai.
* **Context:** [Group::addExpense()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L672) `SplitStrategy*` base pointer pakad kar `calculateSplit()` call karta hai — andar kaunsi class chal rahi hai, usse matlab nahi (polymorphism).

```cpp
// Strategy Interface (Line 126-142)
class SplitStrategy {
public:
  virtual vector<Split> calculateSplit(double totalAmount,
                                       const vector<string> &userIds,
                                       const vector<double> &values = {}) = 0;
  virtual ~SplitStrategy() {}
};

// Concrete Strategy: EqualSplit (Line 145-163)
class EqualSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds,
                               const vector<double> & = {}) override {
    vector<Split> splits;
    double amountPerUser = totalAmount / userIds.size();
    for (const string &userId : userIds) {
      splits.push_back(Split(userId, amountPerUser));
    }
    return splits;
  }
};
```

#### **Ek honest trade-off (interview point):**
`values` param ka **matlab har strategy me alag** hai — EQUAL me chahiye hi nahi, EXACT me rupaye, PERCENTAGE me percent. Ye design ki ek "leaky" jagah hai (ek param ka arth strategy par depend karta hai). Interview me is trade-off ka zikr karna acha impression banata hai.

---

### 3. Factory Design Pattern (Simple Factory)

#### **Kyu use kiya gaya? (Intent)**
Strategy ka natural jodidar. Bina factory ke har caller ko `new EqualSplit()` / `new ExactSplit()` wala if-else khud likhna padta. Factory se wo creation logic **ek jagah** centralize hai — naya type aaye to sirf yahan ek `case` add karo.

#### **Implementation in Code:**
* [SplitFactory](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L228-L246) — static method `getSplitStrategy(SplitType)` switch-case se sahi strategy return karta hai.
* Return type `SplitStrategy*` (**base pointer**) hai — caller ko concrete class ka naam pata hi nahi chalta. Yahi to point hai.

```cpp
// Simple Factory (Line 228-246)
class SplitFactory {
public:
  static SplitStrategy *getSplitStrategy(SplitType type) {
    switch (type) {
    case SplitType::EQUAL:      return new EqualSplit();
    case SplitType::EXACT:      return new ExactSplit();
    case SplitType::PERCENTAGE: return new PercentageSplit();
    default:                    return new EqualSplit(); // defensive fallback
    }
  }
};
```

#### **Memory ka zimma (important):**
Factory `new` karta hai par `delete` **nahi** — wo **caller ki zimmedari** hai. Isi liye dono call sites (`Group::addExpense` aur `Splitwise::addIndividualExpense`) strategy use karne ke baad khud `delete` karte hain, aur `SplitStrategy` ka destructor virtual hai taaki base pointer se delete safe rahe. (Aur bhi safe tareeka: `unique_ptr<SplitStrategy>` return karna.)

---

### 4. Singleton Design Pattern

#### **Kyu use kiya gaya? (Intent)**
`Splitwise` poore app ka **central data store** hai (saare users, groups, individual expenses). Do instance hote to data bat jata — kahin user list adhuri, kahin groups. Singleton guarantee karta hai ki poore application me iska sirf **ek** instance rahe aur global access point mile.

#### **Implementation in Code:**
* [Splitwise](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L834) ka constructor **private** hai — yahi Singleton ka taala; bahar se object banana band.
* [getInstance()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L846-L852) **"magic static"** use karta hai — C++11 guarantee deta hai ki function ke andar ka `static` object exactly **ek baar, thread-safe** tareeke se banega, aur program-end par apne aap destroy bhi hoga.

```cpp
// Thread-safe Singleton via magic static (Line 834-852)
class Splitwise {
private:
  map<string, User *> users;
  map<string, Group *> groups;
  map<string, Expense *> expenses; // SIRF individual expenses

  Splitwise() {} // PRIVATE constructor

public:
  static Splitwise *getInstance() {
    static Splitwise instance; // magic static — lazy + thread-safe + auto-destroy
    return &instance;
  }
};
```

#### **Purane null-check wale tareeke se behtar kyu?**
Classic `if (instance == nullptr) instance = new Splitwise();` wala pattern **thread-safe nahi** tha (do threads ek saath aa jayein to 2 objects ban sakte hain) aur `delete` kabhi hota hi nahi tha (leak). Magic static dono problems ek saath hal kar deta hai — aur destroy hote waqt `~Splitwise()` saare users/groups/expenses clean kar deta hai.

---

### 5. Facade Design Pattern

#### **Kyu use kiya gaya? (Intent)**
System ke andar kaafi entities hain (User, Group, Expense, Split, SplitStrategy, SplitFactory, DebtSimplifier). Client (`main()`) ko in sabke relationships manage na karne padein, isliye `Splitwise` ek **unified simple interface** deta hai. Client sirf IDs bhejta hai; objects dhoondhna, validate karna aur kaam sahi subsystem ko forward karna — sab facade ke andar hota hai.

#### **Implementation in Code:**
`Splitwise` ke facade methods zyada tar **thin wrappers** hain — id se object dhoondho aur kaam `Group` ko forward kar do:

* [addUserToGroup()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L905)
* [addExpenseToGroup()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L938) → `group->addExpense(...)`
* [settlePaymentInGroup()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L954) → `group->settlePayment(...)`
* [settleIndividualPayment()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L967), [addIndividualExpense()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L982)
* [simplifyGroupDebts()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L1065) → `group->simplifyGroupDebts()`

```cpp
// Facade method — client ke liye simple, andar delegation (Line 938-952)
void addExpenseToGroup(string &groupId, string description, double amount,
                       string &paidByUserId, vector<string> &involvedUsers,
                       SplitType splitType,
                       const vector<double> &splitValues = {}) {
  Group *group = getGroup(groupId);          // 1. Group dhoondho
  if (!group) return;                        // 2. Validate karo
  group->addExpense(description, amount,     // 3. Kaam Group ko forward
                    paidByUserId, involvedUsers, splitType, splitValues);
}
```

Client ([main()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L1089)) kabhi bhi `SplitFactory`, `SplitStrategy` ya `DebtSimplifier` ko directly touch nahi karta.

---

## Core Algorithm: Debt Simplification (Greedy) — GoF pattern nahi, par app ka dil

[DebtSimplifier::simplifyDebts()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L405-L518) group ke transactions ko kam se kam transfers me reshuffle kar deta hai. Ye ek **pure static function** hai — matrix andar, naya simplified matrix bahar; koi state, koi side-effect nahi (isliye test karna aasan).

**4 Steps:**
1. **NET nikaalo** — har bande ka `net = kul lena - kul dena`. Matrix mirrored hai isliye **sirf positive entries** padhi jaati hain, warna har karza double count hota.
2. **Do camps** — net > 0 wale `creditors`, net < 0 wale `debtors` (debtor ka amount positive banake store, taaki aage ka math dono lists par same chale).
3. **Descending sort** — dono lists bade amount se chhote ki taraf (greedy ka dil).
4. **Two-pointer greedy matching** — sabse bada creditor vs sabse bada debtor; `settleAmount = min(dono)`. Min lene se har transaction me **kam se kam ek banda poori tarah settle** ho jata hai → n logon ke liye **max n-1 transactions** guaranteed.

```
PEHLE (4 transactions):                 NET:
  Rohit  -> Shubham : 200                 Shubham = +400   Manish = +200
  Saurav -> Shubham : 200                 Rohit   = -200   Saurav = -400
  Saurav -> Manish  : 200
  (Manish ko Saurav se 200 lena)        GREEDY MATCH KE BAAD (2 transactions):
                                          Saurav -> Shubham : 400
                                          Rohit  -> Manish  : 200   <- reshuffled!
```

**Interview point:** Kya greedy hamesha absolute minimum deta hai? **Nahi** — "minimum transactions" NP-hard problem hai (subset-sum jaisi). Ye greedy ek **heuristic** hai: hamesha valid, hamesha ≤ n-1, aur practically optimal ke aas-paas.

---

## Class Responsibilities (Ek Nazar Me)

| Class | Role | Responsibility |
| :--- | :--- | :--- |
| [SplitType](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L66) | Enum (type tag) | Client ki zubaan: EQUAL / EXACT / PERCENTAGE — concrete class ka naam client ko nahi pata. |
| [Split](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L79) | Data Model | Ek user ka hissa: (userId, amount) ka simple pair. |
| [Observer](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L99) | Observer Interface | Notification listeners ka contract (`update`). |
| [SplitStrategy](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L126) + 3 subclasses | Strategy | Split calculate karne ke 3 algorithms (Equal / Exact / Percentage). |
| [SplitFactory](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L228) | Simple Factory | `SplitType` → sahi strategy object. |
| [User](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L261) | Model + Concrete Observer | id/naam/email + **sirf individual** expenses ki balance sheet; group notifications receive karta hai. |
| [Expense](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L347) | Data Model | Ek kharcha: description, amount, kisne pay kiya, splits ki list. |
| [DebtSimplifier](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L405) | Algorithm (stateless) | Balance matrix ko minimum-transfers me simplify karna (greedy). |
| [Group](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L538) | Subject + Business Logic | Members (observers), group expenses (source of truth), balance matrix (derived); add/settle/remove/simplify ka logic. |
| [Splitwise](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L834) | Singleton + Facade | Poore app ka data store + client ke liye simple entry point. |

---

## Important Design Decisions (Dhyan Dene Wali Baatein)

### 1. Sign Convention & Mirrored Balance Matrix (sabse zaroori niyam)
```
balances[X] > 0   ->  X ko MUJHE dena hai  (X owes me)  -> mera LENA
balances[X] < 0   ->  Mujhe X ko dena hai  (I owe X)    -> mera DENA
balances[X] == 0  ->  hisaab barabar, entry hata do
```
Har karza **do jagah** likha jata hai (mirrored / double-entry):
```
A.balances[B] = +200   (B ko A ko 200 dene hain)
B.balances[A] = -200   (wahi baat, B ki taraf se)
```
**Fayda:** kisi bhi ek bande ki row padho, uska poora hisaab dikh jata hai. **Cost:** har update do jagah karna padta hai — [updateGroupBalance()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L629) ye khud sambhalta hai.

### 2. Epsilon (0.01) — floating point ka ilaaj
`double` me 800/3 = 266.66... exact 0 kabhi nahi banta. Isliye har jagah rule hai: **"1 paise se kam bacha = settled maan lo, entry uda do."** [canUserLeaveGroup()](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L646) isi cleanup par depend karta hai — 0.0000001 pada reh jata to user kabhi group chhod hi nahi pata.

### 3. Do Alag Duniya: Group vs Individual expenses
- **Group expenses** → `Group` ke andar, `groupBalances` matrix ko affect karte hain.
- **Individual (1-to-1) expenses** → `Splitwise` ke `expenses` map me, `User::balances` ko affect karte hain.

Jaan-boojh ke alag rakha hai: **group chhodte waqt sirf group ka hisaab check hona chahiye** (tumhare personal coffee ke 40 rupaye group chhodne se nahi rokte).

### 4. Ownership & Memory Management (kaun kya delete karta hai)

| Object | Kaun banata hai | Kaun delete karta hai |
| :--- | :--- | :--- |
| `SplitStrategy` | `SplitFactory` (new) | **Caller** (`Group::addExpense` / `addIndividualExpense`) use ke baad |
| Group ke `Expense` | `Group::addExpense` | `Group` destructor |
| Individual `Expense` | `Splitwise` | `Splitwise` destructor |
| `User`, `Group` | `Splitwise` (createUser/createGroup) | `Splitwise` destructor (Group members me sirf pointers hain — Group unhe delete **nahi** karta, warna double-free) |

### 5. Paid-by user ka apna split SKIP hota hai
`Group::addExpense` me jo banda pay karta hai, uski apni entry balance matrix me nahi jaati (`if split.userId != paidByUserId`) — khud ko khud se lena-dena nahi hota.

### 6. SOLID Principles Mapping
- **S:** Har class ka ek kaam — `DebtSimplifier` sirf algorithm, `Group` sirf group ka hisaab, `SplitFactory` sirf creation.
- **O:** Naya split type = nayi Strategy class + factory me ek case; naya notification channel = nayi Observer class.
- **L:** Koi bhi `SplitStrategy` subclass base pointer ki jagah chal jaati hai.
- **D:** `Group` concrete strategies par nahi, `SplitStrategy` abstraction par depend karta hai.

---

## Demo Flow (`main()` — Line 1089-1173)

1. `Splitwise::getInstance()` se singleton manager lo.
2. 4 users banao: Shubham, Rohit, Manish, Saurav → "Hostel Expenses" group me add karo.
3. **Lunch (EQUAL):** 800, Shubham ne pay kiya, 4 logon me barabar → baaki 3 usko 200-200 denge.
4. **Dinner (EXACT):** 700, Manish ne pay kiya, sirf 3 log involved (Rohit nahi!) — [200, 300, 200]. *Involved list group members se alag ho sakti hai.*
5. Group balances dikhao → **simplifyGroupDebts()** chalao → reshuffled (kam) transactions dikhao.
6. **Individual expense:** Coffee 40, Rohit ↔ Saurav (group se bahar, `User::balances` me).
7. **Rohit ko remove karne ki koshish → FAIL** — simplify ke baad Rohit ko Manish ko 200 dene hain (`canUserLeaveGroup` false).
8. Rohit 200 **settle** karta hai → **dobara remove → SUCCESS**. Yahi is demo ka asli point hai: dues clear hue bina koi group nahi chhod sakta.

---

## Future Improvements (Interview Discussion Points)

1. **Strategy input validation:** `ExactSplit` me `sum(values) == totalAmount` aur `values.size() == userIds.size()`; `PercentageSplit` me `sum == 100` — abhi simple rakha hai, out-of-bounds/galat balance possible hai.
2. **Smart pointers:** `SplitFactory` se `unique_ptr<SplitStrategy>` return karna — caller ko `delete` yaad hi na rakhna pade.
3. **Expense edit/delete:** Is single-file version me nahi hai (modular version me tha) — matrix ko scratch se recalculate karna padta.
4. **Notification channels:** User ke andar print ki jagah alag `EmailNotifier`/`PushNotifier` observers.
5. **Thread safety:** Singleton to thread-safe hai, par `addExpense`/`settlePayment` jaise operations par locks nahi hain — concurrent use ke liye mutex chahiye.
