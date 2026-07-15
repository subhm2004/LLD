# Splitwise App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L31 Splitwise_LLD/C++_original_code/Splitwise_app.cpp` file me use hone wale sabhi design patterns ko detail me explain kiya gaya hai. 

---

## Quick Summary (Overview of Patterns)

Yeh application ek classic Low-Level Design (LLD) problem (Splitwise) ko solve karta hai. Isme system ko scalable, maintainable aur decoupled banane ke liye **5 major Design Patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Splitwise App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Jab bhi group me koi expense add ya settle ho, tab sabhi group members (observers) ko notifications bhejwane ke liye. |
| **2. Strategy Pattern** | Behavioral | Runtime par split calculations (EQUAL, EXACT, PERCENTAGE) ko switch/select karne ke liye. |
| **3. Factory Pattern** | Creational | `SplitType` ke basis par sahi Strategy class ka object runtime par instantiate karne ke liye. |
| **4. Singleton Pattern** | Creational | Pure application me `Splitwise` main controller ka ek hi global instance maintain karne ke liye. |
| **5. Facade Pattern** | Structural | Subsystem classes (Group, User, Strategy, etc.) ki complexity ko hide karke client (`main()`) ko ek simple interface provide karne ke liye. |

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Splitwise app me jab bhi koi member naya expense add karta hai ya split settle karta hai, toh baaki ke saare group members ko notify/alert karna hota hai. Observer pattern subject (`Group`) aur observers (`User`) ke bich one-to-many dependency ko clean tarike se handle karta hai taaki loose coupling bani rahe.

#### **Implementation in Code:**
* **Subject (Observable):** [Group](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L332) class observable ki tarah act karti hai. Isme observers (`members` list) hold hote hain.
* **Observer Interface:** [Observer](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L37-L42) ek abstract class hai jisme pure virtual function `update(message)` hai.
* **Concrete Observer:** [User](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L133) class `Observer` se inherit karti hai aur `update` function ko override karti hai notifications print karne ke liye.

```cpp
// Observer Interface (Line 37-42)
class Observer {
public:
  virtual void update(const string &message) = 0;
};

// Concrete Observer (Line 133-157)
class User : public Observer {
public:
  // ... details ...
  void update(const string &message) override {
    cout << "[NOTIFICATION to " << name << "]: " << message << endl;
  }
};

// Subject / Observable (Line 332-414)
class Group {
public:
  vector<User *> members; // List of Observers
  
  // Notification dispatching logic
  void notifyMembers(const string &message) {
    for (Observer *observer : members) {
      observer->update(message);
    }
  }
};
```

---

### 2. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Expenses ko split karne ke multiple algorithms ho sakte hain (jaise: Equal share, exact amounts, ya percentage shares). Strategy pattern in algorithms ko independent classes me encapsulate kar deta hai aur client runtime par decide karta hai ki kaunsi strategy lagani hai.

#### **Implementation in Code:**
* **Strategy Interface:** [SplitStrategy](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L47-L55) class split calculate karne ke liye virtual function `calculateSplit` define karti hai.
* **Concrete Strategies:** 
  1. [EqualSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L58-L74): Total amount ko sabhi members me barabar baant ta hai.
  2. [ExactSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L78-L91): Har member ka user-provided exact amount assign karta hai.
  3. [PercentageSplit](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L95-L110): Percentage share ke based par amount calculate karta hai.

```cpp
// Strategy Interface (Line 47-55)
class SplitStrategy {
public:
  virtual vector<Split> calculateSplit(double totalAmount,
                                       const vector<string> &userIds,
                                       const vector<double> &values = {}) = 0;
};

// Concrete Strategy: Equal Split (Line 58-74)
class EqualSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount, const vector<string> &userIds, const vector<double> &values = {}) override {
    vector<Split> splits;
    double amountPerUser = totalAmount / userIds.size();
    for (const string &userId : userIds) {
      splits.push_back(Split(userId, amountPerUser));
    }
    return splits;
  }
};
```

---

### 3. Factory Design Pattern (Simple Factory)
#### **Kyu use kiya gaya? (Intent)**
Strategy pattern create karne ke liye client ko concrete classes (`EqualSplit`, `ExactSplit`, `PercentageSplit`) ko directly instantiate nahi karna padta. Factory pattern object creation logic ko centralize kar deta hai jisse client direct coupling se bach jata hai.

#### **Implementation in Code:**
* [SplitFactory](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L114-L128) class me ek static method `getSplitStrategy(SplitType type)` hai jo switch-case run karke dynamic strategy create karke return karta hai.

```cpp
// Factory Class (Line 114-128)
class SplitFactory {
public:
  static SplitStrategy *getSplitStrategy(SplitType type) {
    switch (type) {
    case SplitType::EQUAL:
      return new EqualSplit();
    case SplitType::EXACT:
      return new ExactSplit();
    case SplitType::PERCENTAGE:
      return new PercentageSplit();
    default:
      return new EqualSplit();
    }
  }
};
```

---

### 4. Singleton Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Pure system me users, groups aur expenses ki consistency maintain karne ke liye hume ek single central coordinator/controller chahiye hota hai. Singleton pattern guarantee karta hai ki class (`Splitwise`) ka sirf ek instance pure application lifespan me exist karega aur uska global access point hoga.

#### **Implementation in Code:**
* [Splitwise](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L593-L610) class ka constructor `private` rakha gaya hai taaki koi bahar se object create na kar sake.
* Static member `instance` check karke return karne ke liye static getter function `getInstance()` implement kiya gaya hai.

```cpp
// Singleton Implementation (Line 593-610)
class Splitwise {
private:
  // Maps of Users, Groups, Expenses
  map<string, User *> users;
  map<string, Group *> groups;
  
  static Splitwise *instance;
  Splitwise() {} // Private Constructor (Prevents direct instantiation)

public:
  static Splitwise *getInstance() {
    if (instance == nullptr) {
      instance = new Splitwise();
    }
    return instance;
  }
  // ... methods ...
};

// Singleton initialization (Line 788)
Splitwise *Splitwise::instance = nullptr;
```

---

### 5. Facade Design Pattern
#### **Kyu use kiya gaya? (Intent)**
System ke andur multiple independent entities hain (Group, User, Split, Expense, SplitFactory, SplitStrategy). Client (`main()`) ko in sabhi complex relationships ko manage na karna pade, isliye `Splitwise` class ek unified Facade ki tarah kaam karti hai. Client sirf facade ke highly simplified method calls jaise `addExpenseToGroup()` ya `settlePaymentInGroup()` karta hai, aur facade internally objects ko coordinate karta hai.

#### **Implementation in Code:**
* Client ([main function](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L792)) directly manager/facade se connect hota hai aur subsystems (Group, User) ke complicated objects aur internal logic (e.g. Factory, Strategy calculations) ko facade khud hide kar leta hai.

```cpp
// Facade Methods in Splitwise class (Line 673-686)
void addExpenseToGroup(string &groupId, string description, double amount,
                       string &paidByUserId, vector<string> &involvedUsers,
                       SplitType splitType,
                       const vector<double> &splitValues = {}) {
    // 1. Group retrieve karta hai
    Group *group = getGroup(groupId);
    if (!group) return;
    
    // 2. Complex internal calculations ko delegate karta hai group logic me
    group->addExpense(description, amount, paidByUserId, involvedUsers,
                      splitType, splitValues);
}
```

---

## Additional Key Architecture Elements

* **Greedy / Min-Max Debt Simplification (Not a GoF Design Pattern, but key logic):**
  [DebtSimplifier](file:///Users/shubham/Desktop/LLD/L31%20Splitwise_LLD/C++_original_code/Splitwise_app.cpp#L223-L327) class ka algorithm pure Splitwise app ka core algorithmic feature hai. Yeh transactional graph me net balance calculate karke min/max heap concept/greedy approach se transaction paths ko optimize karke minimum transfers me settle karta hai.

