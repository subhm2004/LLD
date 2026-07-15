#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

// Forward declarations
class User;
class Group;
class ExpenseManager;

// SplitType enum: Yeh batata hai ki expense ko split (baantna) kaise hai.
// EQUAL: Sabhi logo me barabar amount batega.
// EXACT: Har user ka specific kitna amount hai, woh bataya jayega.
// PERCENTAGE: Har user ka percentage share (%) bataya jayega.
enum class SplitType { EQUAL, EXACT, PERCENTAGE };

// Split class: Yeh class represent karti hai ki ek particular user ka kitna
// share (amount) hai expense me.
class Split {
public:
  string userId; // Kis user ka split hai
  double amount; // Us user ka kitna amount banta hai

  Split(const string &userId, double amount) {
    this->userId = userId;
    this->amount = amount;
  }
};

// Observer Pattern - Notification interface:
// Jab bhi koi naya expense ya settlement hoga, tab sabhi group members ko
// update karne ke liye interface.
class Observer {
public:
  // Pure virtual function: Har concrete observer (jaise User class) isko
  // implement karega.
  virtual void update(const string &message) = 0;
};

// Strategy Pattern - Split strategies:
// Split karne ka tarika dynamic rakhne ke liye base class.
// Isse hum runtime par split karne ki calculations ko switch kar sakte hain.
class SplitStrategy {
public:
  // Split calculate karne ke liye virtual function.
  // values parameter ka use EXACT me amounts ke liye aur PERCENTAGE me
  // percentages ke liye hoga.
  virtual vector<Split> calculateSplit(double totalAmount,
                                       const vector<string> &userIds,
                                       const vector<double> &values = {}) = 0;
};

// EQUAL Split Strategy: Sab me barabar baantna.
class EqualSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount,
                               const vector<string> &userIds,
                               const vector<double> &values = {}) override {
    vector<Split> splits;
    // Total amount ko total number of users se divide kar diya.
    double amountPerUser = totalAmount / userIds.size();

    for (const string &userId : userIds) {
      // Sabhi users ke liye same share/amount ke sath Split object banakar push
      // kiya.
      splits.push_back(Split(userId, amountPerUser));
    }
    return splits;
  }
};

// EXACT Split Strategy: Har ek user ka specific amount pehle se bataya jata
// hai.
class ExactSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount,
                               const vector<string> &userIds,
                               const vector<double> &values = {}) override {
    vector<Split> splits;

    // values vector me har user ke corresponds unka exact expense share hoga.
    for (int i = 0; i < userIds.size(); i++) {
      splits.push_back(Split(userIds[i], values[i]));
    }
    return splits;
  }
};

// PERCENTAGE Split Strategy: Har user ke percentage share ke hisab se
// calculation.
class PercentageSplit : public SplitStrategy {
public:
  vector<Split> calculateSplit(double totalAmount,
                               const vector<string> &userIds,
                               const vector<double> &values = {}) override {
    vector<Split> splits;

    // values vector me percentage shares honge.
    for (int i = 0; i < userIds.size(); i++) {
      // Amount = (Total * Percentage) / 100
      double amount = (totalAmount * values[i]) / 100.0;
      splits.push_back(Split(userIds[i], amount));
    }
    return splits;
  }
};

// Factory Pattern: SplitType ke basis par sahi Strategy class ka object create
// karke dena.
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

// User class: Yeh Concrete Observer hai (jo Observer class se inherit kar raha
// hai). Isme user ki details aur balance sheet (kisne kisko kitna dena hai)
// store hoti hai.
class User : public Observer {
public:
  static int nextUserId; // Static counter taaki har user ko auto-generated
                         // unique ID mil sake (user1, user2...)
  string userId;         // Unique ID of the user
  string name;           // User ka naam
  string email;          // User ki email id

  // Balances Map: Yeh batata hai ki is user ka dusre users ke sath kya len-den
  // hai. balance[otherUserId] > 0 matlab: otherUser is bande ko owe karta hai
  // (paise dega). balance[otherUserId] < 0 matlab: Yeh banda otherUser ko owe
  // karta hai (paise lega).
  map<string, double> balances;

  User(const string &name, const string &email) {
    this->userId = "user" + to_string(++nextUserId);
    this->name = name;
    this->email = email;
  }

  // Observer ka update method: Jab bhi koi activity hogi tab notification
  // message print karega.
  void update(const string &message) override {
    cout << "[NOTIFICATION to " << name << "]: " << message << endl;
  }

  // Update Balance: Kisi specific user ke sath len-den ka status update karne
  // ke liye.
  void updateBalance(const string &otherUserId, double amount) {
    balances[otherUserId] += amount;

    // Agar balance zero ya zero ke bahut paas (floating point approximation) ho
    // jaye, to entry delete kar do.
    if (abs(balances[otherUserId]) < 0.01) {
      balances.erase(otherUserId);
    }
  }

  // Total Owed: Yeh banda dusro ko total kitne paise dega (negative balances ka
  // sum).
  double getTotalOwed() {
    double total = 0;
    for (const auto &balance : balances) {
      if (balance.second < 0) {
        total += abs(balance.second);
      }
    }
    return total;
  }

  // Total Owing: Dusre log is bande ko total kitna owe karte hain (positive
  // balances ka sum).
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
int User::nextUserId = 0;

// Expense Model class: Yeh ek single transaction/expense ki blueprint hai.
class Expense {
public:
  static int nextExpenseId; // Auto-incrementing expense ID counter
  string expenseId;         // Unique ID
  string description; // Kharch ki summary/description (jaise "Lunch", "Dinner")
  double totalAmount; // Total bill amount
  string paidByUserId;  // Kisne bill bhara (Payer ID)
  vector<Split> splits; // Kis-kis me aur kitna-kitna split hua
  string groupId; // Agar group expense hai to group ID, warna empty string

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

// DebtSimplifier: Yeh class pure splitwise app ki core utility hai.
// Yeh group members ke bich ke multiple transactions ko minimal transactions me
// simplify karti hai. Isme Greedy/Min-Max Algorithm use hota hai.
class DebtSimplifier {
public:
  static map<string, map<string, double>>
  simplifyDebts(map<string, map<string, double>> groupBalances) {
    // 1. Har person ka Net Amount (Kitna lena hai vs kitna dena hai ka total
    // net) calculate karenge.
    map<string, double> netAmounts;

    // Sabko pehle 0 se initialize karenge.
    for (const auto &userBalance : groupBalances) {
      netAmounts[userBalance.first] = 0;
    }

    // Net amount ki calculation:
    // groupBalances[A][B] = 200 ka matlab hai B owes A 200 Rs (B ko A ko dena
    // hai). Is case me A ko milega (positive net), B se jayega (negative net).
    for (const auto &userBalance : groupBalances) {
      string creditorId =
          userBalance.first; // Jise paise milne hain (jisne kharch kiya tha)
      for (const auto &balance : userBalance.second) {
        string debtorId =
            balance.first; // Jisne paise dene hain (jo owe karta hai)
        double amount = balance.second;

        // Double counting se bachne ke liye sirf positive amounts check
        // karenge.
        if (amount > 0) {
          netAmounts[creditorId] +=
              amount;                     // Creditor ke net amount me plus hoga
          netAmounts[debtorId] -= amount; // Debtor ke net amount me minus hoga
        }
      }
    }

    // 2. Net amounts ke basis par logo ko do categories me divide karenge:
    // Creditors aur Debtors.
    vector<pair<string, double>>
        creditors; // Vo log jo net-positive me hain (paise lenge)
    vector<pair<string, double>>
        debtors; // Vo log jo net-negative me hain (paise denge)

    for (const auto &net : netAmounts) {
      if (net.second > 0.01) {
        // Creditor category
        creditors.push_back({net.first, net.second});
      } else if (net.second < -0.01) {
        // Debtor category: net.second negative hoga, isliye positive banake
        // store kiya
        debtors.push_back({net.first, -net.second});
      }
    }

    // 3. Optimization ke liye dono list ko descending order (sabse zyada
    // balance pehle) me sort karenge. Isse badi balances aapas me jaldi settle
    // ho jati hain.
    sort(creditors.begin(), creditors.end(),
         [](const pair<string, double> &a, const pair<string, double> &b) {
           return a.second > b.second;
         });
    sort(debtors.begin(), debtors.end(),
         [](const pair<string, double> &a, const pair<string, double> &b) {
           return a.second > b.second;
         });

    // Nayi simplified balances map tayar karenge.
    map<string, map<string, double>> simplifiedBalances;

    // Initialize empty maps.
    for (const auto &userBalance : groupBalances) {
      simplifiedBalances[userBalance.first] = map<string, double>();
    }

    // 4. Greedy algorithm chala kar settlement karenge.
    int i = 0, j = 0; // i for creditors, j for debtors
    while (i < creditors.size() && j < debtors.size()) {
      string creditorId = creditors[i].first;
      string debtorId = debtors[j].first;
      double creditorAmount = creditors[i].second;
      double debtorAmount = debtors[j].second;

      // Dono me se jo bhi chota/minimum amount hai use aapas me settle karenge.
      double settleAmount = min(creditorAmount, debtorAmount);

      // Debtor owes Creditor the settleAmount
      simplifiedBalances[creditorId][debtorId] = settleAmount;
      simplifiedBalances[debtorId][creditorId] = -settleAmount;

      // Settle karne ke baad bacha hua amount calculate karenge.
      creditors[i].second -= settleAmount;
      debtors[j].second -= settleAmount;

      // Agar creditor ka balance clear ho gaya (< 0.01), to next creditor par
      // jao.
      if (creditors[i].second < 0.01) {
        i++;
      }
      // Agar debtor ka balance clear ho gaya (< 0.01), to next debtor par jao.
      if (debtors[j].second < 0.01) {
        j++;
      }
    }

    return simplifiedBalances;
  }
};

// Group class: Yeh observable subject hai jisme members added hote hain aur
// notifications receive karte hain. Yeh group specific balances, expenses aur
// settlements ko manage karti hai.
class Group {
private:
  // Group ke andar userId se member ko search karne ke liye helper method.
  User *getUserByuserId(string userId) {
    User *user = nullptr;

    for (User *member : members) {
      if (member->userId == userId) {
        user = member;
      }
    }
    return user;
  }

public:
  static int nextGroupId; // Auto-incrementing group ID counter
  string groupId;         // Unique Group ID
  string name;            // Group name (jaise "Trip to Goa")
  vector<User *> members; // Observers list (jo notifications receive karenge)
  map<string, Expense *> groupExpenses; // Group ke saare expenses store karne
                                        // ki book (expenseId -> Expense*)
  map<string, map<string, double>>
      groupBalances; // Group balance sheet: memberId -> {otherMemberId ->
                     // balance}

  Group(const string &name) {
    this->groupId = "group" + std::to_string(++nextGroupId);
    this->name = name;
  }

  ~Group() {
    // Destructor: Memory leak se bachne ke liye dynamically allocated expenses
    // ko cleanup karte hain.
    for (auto &pair : groupExpenses) {
      delete pair.second;
    }
  }

  // Member ko group me add karna.
  void addMember(User *user) {
    members.push_back(user);

    // Naye member ke liye empty balance sheet initialize karna.
    groupBalances[user->userId] = map<string, double>();
    cout << user->name << " added to group " << name << endl;
  }

  // Member ko group se hatana.
  bool removeMember(const string &userId) {
    // validation: Agar user ke balances outstanding hain (kisi ka dena bacha
    // hai ya lena hai), to leave nahi karne denge.
    if (!canUserLeaveGroup(userId)) {
      cout << "\nUser not allowed to leave group without clearing expenses"
           << endl;
      return false;
    }

    // Observers list se user ko remove kiya.
    for (User *user : members) {
      if (user->userId == userId) {
        members.erase(remove(members.begin(), members.end(), user),
                      members.end());
        break;
      }
    }

    // Group balance data se us user ka map erase kiya.
    groupBalances.erase(userId);

    // Baki members ke balance sheet me se bhi is user ki references ko erase
    // kiya.
    for (auto &memberBalance : groupBalances) {
      memberBalance.second.erase(userId);
    }
    return true;
  }

  // Observer Pattern: Sabhi members ko alert/notification send karna.
  void notifyMembers(const string &message) {
    for (Observer *observer : members) {
      observer->update(message);
    }
  }

  // Yeh check karta hai ki user group ka member hai ya nahi.
  bool isMember(const string &userId) {
    return groupBalances.find(userId) != groupBalances.end();
  }

  // Group ke andar do logon ke aapas ke transaction balance ko update karna.
  void updateGroupBalance(const string &fromUserId, const string &toUserId,
                          double amount) {
    groupBalances[fromUserId][toUserId] += amount;
    groupBalances[toUserId][fromUserId] -= amount;

    // Agar absolute value 0 ke close aa jaye, to records clear karne ke liye
    // map se entry delete kar dete hain.
    if (abs(groupBalances[fromUserId][toUserId]) < 0.01) {
      groupBalances[fromUserId].erase(toUserId);
    }
    if (abs(groupBalances[toUserId][fromUserId]) < 0.01) {
      groupBalances[toUserId].erase(fromUserId);
    }
  }

  // Kya user group chhod sakta hai? Tabhi chhod payega jab bacha hua balance 0
  // ho.
  bool canUserLeaveGroup(const string &userId) {
    if (!isMember(userId)) {
      throw runtime_error("user is not a part of this group");
    };

    map<string, double> userBalanceSheet = groupBalances[userId];
    for (auto &balance : userBalanceSheet) {
      if (abs(balance.second) > 0.01) {
        return false; // Still outstanding amount exists.
      }
    }
    return true;
  }

  // User ki is specific group me kitni balances hain.
  map<string, double> getUserGroupBalances(const string &userId) {
    if (!isMember(userId)) {
      throw runtime_error("user is not a part of this group");
    };
    return groupBalances[userId];
  }

  // Group me naya Expense add karna.
  bool addExpense(string &description, double amount, string &paidByUserId,
                  vector<string> &involvedUsers, SplitType splitType,
                  const vector<double> &splitValues = {}) {
    // 1. Pehle validations checking
    if (!isMember(paidByUserId)) {
      throw runtime_error("user is not a part of this group");
    }

    for (const string &userId : involvedUsers) {
      if (!isMember(userId)) {
        throw runtime_error("involvedUsers are not a part of this group");
      }
    }

    // 2. SplitFactory se appropriate split strategy get karke balances
    // calculate karna.
    vector<Split> splits =
        SplitFactory::getSplitStrategy(splitType)->calculateSplit(
            amount, involvedUsers, splitValues);

    // 3. Expense object create karke group records me add karna.
    Expense *expense =
        new Expense(description, amount, paidByUserId, splits, groupId);
    groupExpenses[expense->expenseId] = expense;

    // 4. Balances update karna.
    for (Split &split : splits) {
      if (split.userId != paidByUserId) {
        // Jisne pay kiya use dusre member se paise lene hain (positive), aur
        // dusre ko use dene hain (negative)
        updateGroupBalance(paidByUserId, split.userId, split.amount);
      }
    }

    // 5. Group notifications send karna (Observer Pattern).
    cout << endl
         << "=========== Sending Notifications ====================" << endl;
    string paidByName = getUserByuserId(paidByUserId)->name;
    notifyMembers("New expense added: " + description + " (Rs " +
                  to_string(amount) + ")");

    // Console printing logic for tracking.
    cout << endl << "=========== Expense Message ====================" << endl;
    cout << "Expense added to " << name << ": " << description << " (Rs "
         << amount << ") paid by " << paidByName
         << " and involved people are : " << endl;
    if (!splitValues.empty()) {
      for (int i = 0; i < splitValues.size(); i++) {
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

  // Group ke andar do logon ke bich payment settle karna.
  bool settlePayment(string &fromUserId, string &toUserId, double amount) {
    if (!isMember(fromUserId) || !isMember(toUserId)) {
      cout << "user is not a part of this group" << endl;
      return false;
    }

    // Balance sheets ko update karenge.
    updateGroupBalance(fromUserId, toUserId, amount);

    string fromName = getUserByuserId(fromUserId)->name;
    string toName = getUserByuserId(toUserId)->name;

    // Settle hone par notification bhejenge.
    notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " +
                  to_string(amount));

    cout << "Settlement in " << name << ": " << fromName << " settled Rs "
         << amount << " with " << toName << endl;

    return true;
  }

  // Group ki puri balance sheet print karna.
  void showGroupBalances() {
    cout << "\n=== Group Balances for " << name << " ===" << endl;

    for (const auto &pair : groupBalances) {
      string memberId = pair.first;
      string memberName = getUserByuserId(memberId)->name;

      cout << memberName << "'s balances in group:" << endl;

      auto userBalances = pair.second;
      if (userBalances.empty()) {
        cout << "  No outstanding balances" << endl;
      } else {
        for (const auto &userBalance : userBalances) {
          string otherMemberUserId = userBalance.first;
          string otherName = getUserByuserId(otherMemberUserId)->name;

          double balance = userBalance.second;
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

  // Group transactions ko simplify karna.
  void simplifyGroupDebts() {
    map<string, map<string, double>> simplifiedBalances =
        DebtSimplifier::simplifyDebts(groupBalances);
    groupBalances = simplifiedBalances;

    cout << "\nDebts have been simplified for group: " << name << endl;
  }
};
int Group::nextGroupId = 0;

// Main ExpenseManager class (Singleton - Facade):
// Splitwise (Main Controller): Facade pattern provide karti hai takki client ko
// internal classes (Group, User, Split) ka complications na dekhna pade.
// Singleton pattern ensure karta hai ki pure app me Splitwise controller ka
// sirf ek hi instance exist kare.
class Splitwise {
private:
  map<string, User *> users;   // System ke saare users (userId -> User*)
  map<string, Group *> groups; // System ke saare groups (groupId -> Group*)
  map<string, Expense *>
      expenses; // Individual expenses ka record (expenseId -> Expense*)

  static Splitwise *instance;
  Splitwise() {
  } // Private Constructor taaki bahar se new Splitwise() call na kiya ja sake.

public:
  static Splitwise *getInstance() {
    if (instance == nullptr) {
      instance = new Splitwise();
    }
    return instance;
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

  // Kisi user ko group me add karne ka portal.
  void addUserToGroup(const string &userId, const string &groupId) {
    User *user = getUser(userId);
    Group *group = getGroup(groupId);

    if (user && group) {
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

  // Bina group ke individual payment settle karna.
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

  // Bina group ke individual expense add karna.
  void addIndividualExpense(string description, double amount,
                            string paidByUserId, string toUserId,
                            SplitType splitType,
                            const vector<double> &splitValues = {}) {

    SplitStrategy *strategy = SplitFactory::getSplitStrategy(splitType);
    vector<Split> splits =
        strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);

    Expense *expense = new Expense(description, amount, paidByUserId, splits);
    expenses[expense->expenseId] = expense;

    User *paidByUser = getUser(paidByUserId);
    User *toUser = getUser(toUserId);

    paidByUser->updateBalance(toUserId, amount);
    toUser->updateBalance(paidByUserId, -amount);

    cout << "Individual expense added: " << description << " (Rs " << amount
         << ") paid by " << paidByUser->name << " for " << toUser->name << endl;
  }

  // User ki overall balance details print karna.
  void showUserBalance(string &userId) {
    User *user = getUser(userId);
    if (!user)
      return;

    cout << endl
         << "=========== Balance for " << user->name
         << " ====================" << endl;
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

Splitwise *Splitwise::instance = nullptr;

// Main program driver function: Yaha pure process ka implementation aur
// verification kiya gaya hai.
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
  vector<string> groupMembers = {user1->userId, user2->userId, user3->userId,
                                 user4->userId};
  manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0,
                             user1->userId, groupMembers, SplitType::EQUAL);

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

  cout << endl
       << "==========Attempting to remove Rohit from group==========" << endl;
  manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

  cout << endl
       << "======== Making Settlement to Clear Rohit's Debt ==========" << endl;
  manager->settlePaymentInGroup(hostelGroup->groupId, user2->userId,
                                user3->userId, 200.0);

  cout << endl
       << "======== Attempting to Remove Rohit Again ==========" << endl;
  manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

  cout << endl
       << "=========== Updated Group Balances ====================" << endl;
  manager->showGroupBalances(hostelGroup->groupId);

  return 0;
}