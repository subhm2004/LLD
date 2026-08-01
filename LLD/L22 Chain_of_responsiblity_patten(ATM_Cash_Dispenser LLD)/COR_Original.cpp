// ============================================================================
//  COR_Original.cpp  —  CHAIN OF RESPONSIBILITY (single-file MONOLITH) — ATM
// ----------------------------------------------------------------------------
//  CoR = "ek request ko handlers ki CHAIN me se guzaaro — har handler apna
//         HISSA karke bacha hua kaam NEXT handler ko de deta hai."
//
//  Asli duniya ka example: ATM se paise nikaalna 🏧
//    Tum ₹4000 maangte ho. ATM ke paas alag denominations ke notes hain
//    (₹1000, ₹500, ₹200, ₹100). Kaunse kitne dega? — HANDLERS ki chain decide
//    karti hai: pehle ₹1000 wala jitne de sake de, bacha hua ₹500 wale ko,
//    uska bacha ₹200 wale ko... aise sabse bade note se chhote tak.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  CHAIN KA FLOW (₹4000 nikaalne pe):                                     │
//  │                                                                          │
//  │   ₹4000 -> [₹1000 handler] -> 3 notes de (₹3000), ₹1000 bacha           │
//  │              │ next ko ₹1000                                             │
//  │              ▼                                                           │
//  │            [₹500 handler]  -> 2 notes de (₹1000), ₹0 bacha              │
//  │              │ kuch nahi bacha -> chain yahin ruk gayi                   │
//  │              ▼                                                           │
//  │            [₹200 handler]  -> (call hi nahi hua, ₹0 tha)                │
//  │            [₹100 handler]  -> (call hi nahi hua)                        │
//  │                                                                          │
//  │  Har handler: apni denomination ke jitne notes de sakta hai de deta     │
//  │  hai, remaining NEXT ko forward. Aakhri handler (₹100) ke baad bhi      │
//  │  amount bacha to "Insufficient fund" — chain khatam.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES:
//    1. Handler (base)   -> MoneyHandler : nextHandler pointer + dispense()
//    2. ConcreteHandlers -> Thousand/FiveHundred/TwoHundred/HundredHandler
//    3. Client           -> main() : chain banata hai aur head se dispense
//
//  Ye original MONOLITHIC version hai (sab ek file me). Modular layered
//  version `C++ Code/` folder me hai (enums/models/handlers/managers/services).
//
//  ⚠️ NOTE — is code me ek DESIGN LIMITATION hai (interview me bolne layak):
//  har handler amount ke hisaab se notesNeeded nikalta hai (amount/1000),
//  par ye "greedy" approach hamesha optimal ya feasible nahi hoti. Jaise
//  ₹1000 maango par sirf ₹500×1 aur ₹200×... ho — ye code us edge case ko
//  perfectly handle nahi karta. Real ATM me backtracking/DP se exact
//  combination nikaalte hain. Yahan pattern SEEKHNA point hai, algorithm nahi.
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

// ============================================================================
//  ABSTRACT HANDLER (Base) — Chain of Responsibility ki REEDH KI HADDI
// ----------------------------------------------------------------------------
//  Har handler ke paas 2 cheezein: (1) nextHandler pointer (chain ki agli
//  kadi), (2) dispense() jo har concrete handler apne tareeke se karta hai.
//  Yahi do cheezein pura CoR banati hain — ek linked-list of handlers.
// ============================================================================
class MoneyHandler {
protected:
  // Chain ki AGLI kadi ka pointer (nullptr = ye aakhri handler hai)
  MoneyHandler *nextHandler;

public:
  MoneyHandler() { this->nextHandler = nullptr; }

  // Chain wire karne ke liye — thousand->setNext(fiveHundred) wagera
  void setNextHandler(MoneyHandler *next) { nextHandler = next; }

  // Pure virtual — har denomination handler apna dispense logic dega
  virtual void dispense(int amount) = 0;

  virtual ~MoneyHandler() = default;
};

// Concrete Handler for ₹1000 notes
class ThousandHandler : public MoneyHandler {
private:
  int numNotes;

public:
  // Constructor to initialize the number of notes
  ThousandHandler(int numNotes) { this->numNotes = numNotes; }

  // dispense() ka pattern (SAARE handlers me yahi hai, sirf denomination alag):
  void dispense(int amount) override {
    // STEP 1: kitne ₹1000 note chahiye is amount ke liye?
    int notesNeeded = amount / 1000;

    // STEP 2: itne notes hain kya? Nahi to jitne hain utne hi do (aur
    //         stock 0 karo); haan to notesNeeded ghata do stock se.
    if (notesNeeded > numNotes) {
      notesNeeded = numNotes;  // jitne available utne hi de sakte
      numNotes = 0;            // stock khaali
    } else {
      numNotes -= notesNeeded; // stock se ghata do
    }

    // STEP 3: jo notes de rahe hain, print karo
    if (notesNeeded > 0)
      cout << "Dispensing " << notesNeeded << " x ₹1000 notes.\n";

    // STEP 4: bacha hua amount NEXT handler ko forward (CoR ka core!)
    int remainingAmount = amount - (notesNeeded * 1000);
    if (remainingAmount > 0) {
      if (nextHandler != nullptr)
        nextHandler->dispense(remainingAmount);  // chhote note wale ko de do
      else {
        // Ye AAKHRI handler tha aur amount abhi bhi bacha -> ATM me
        // sahi combination ke notes nahi the
        cout << "Remaining amount of " << remainingAmount
             << " cannot be fulfilled (Insufficinet fund in ATM)\n";
      }
    }
  }
};

// Concrete Handler for ₹500 notes
class FiveHundredHandler : public MoneyHandler {
private:
  int numNotes;

public:
  FiveHundredHandler(int numNotes) { this->numNotes = numNotes; }
  // Dispense the money
  void dispense(int amount) override {
    int notesNeeded = amount / 500;

    // If the number of notes needed is greater than the number of notes
    // available, dispense the maximum number of notes available
    if (notesNeeded > numNotes) {
      notesNeeded = numNotes;
      numNotes = 0;
      // If the number of notes needed is less than the number of notes
      // available, dispense the number of notes needed
    } else {
      numNotes -= notesNeeded;
    }

    if (notesNeeded > 0)
      cout << "Dispensing " << notesNeeded << " x ₹500 notes.\n";

    int remainingAmount = amount - (notesNeeded * 500);
    if (remainingAmount > 0) {
      if (nextHandler != nullptr)
        nextHandler->dispense(remainingAmount);
      else {
        cout << "Remaining amount of " << remainingAmount
             << " cannot be fulfilled (Insufficinet fund in ATM)\n";
      }
    }
  }
};

// Concrete Handler for ₹200 notes
class TwoHundredHandler : public MoneyHandler {
private:
  int numNotes;

public:
  // Constructor to initialize the number of notes
  TwoHundredHandler(int numNotes) { this->numNotes = numNotes; }
  // Dispense the money

  void dispense(int amount) override {
    int notesNeeded = amount / 200;

    // If the number of notes needed is greater than the number of notes
    // available, dispense the maximum number of notes available
    if (notesNeeded > numNotes) {
      notesNeeded = numNotes;
      numNotes = 0;
      // If the number of notes needed is less than the number of notes
      // available, dispense the number of notes needed
    } else {
      numNotes -= notesNeeded;
    }

    if (notesNeeded > 0)
      cout << "Dispensing " << notesNeeded << " x ₹200 notes.\n";

    int remainingAmount = amount - (notesNeeded * 200);
    if (remainingAmount > 0) {
      if (nextHandler != nullptr)
        nextHandler->dispense(remainingAmount);
      else {
        cout << "Remaining amount of " << remainingAmount
             << " cannot be fulfilled (Insufficinet fund in ATM)\n";
      }
    }
  }
};

// Concrete Handler for ₹100 notes
class HundredHandler : public MoneyHandler {
private:
  int numNotes;

public:
  // Constructor to initialize the number of notes
  HundredHandler(int numNotes) { this->numNotes = numNotes; }
  // Dispense the money

  void dispense(int amount) override {
    int notesNeeded = amount / 100;

    // If the number of notes needed is greater than the number of notes
    // available, dispense the maximum number of notes available
    if (notesNeeded > numNotes) {
      notesNeeded = numNotes;
      numNotes = 0;
      // If the number of notes needed is less than the number of notes
      // available, dispense the number of notes needed
    } else {
      numNotes -= notesNeeded;
    }

    if (notesNeeded > 0)
      cout << "Dispensing " << notesNeeded << " x ₹100 notes.\n";

    int remainingAmount = amount - (notesNeeded * 100);
    if (remainingAmount > 0) {
      if (nextHandler != nullptr)
        nextHandler->dispense(remainingAmount);
      else {
        cout << "Remaining amount of " << remainingAmount
             << " cannot be fulfilled (Insufficinet fund in ATM)\n";
      }
    }
  }
};

// Client
int main() {
  // STEP 1: Handlers banao — har ek ke paas apne notes ka stock
  // (3 x ₹1000, 5 x ₹500, 10 x ₹200, 20 x ₹100)
  MoneyHandler *thousandHandler = new ThousandHandler(3);
  MoneyHandler *fiveHundredHandler = new FiveHundredHandler(5);
  MoneyHandler *twoHundredHandler = new TwoHundredHandler(10);
  MoneyHandler *hundredHandler = new HundredHandler(20);

  // STEP 2: Chain WIRE karo — bade note se chhote note ki taraf.
  // ORDER IMPORTANT hai! ₹1000 pehle taaki kam notes me kaam ho.
  // ₹1000 -> ₹500 -> ₹200 -> ₹100
  thousandHandler->setNextHandler(fiveHundredHandler);
  fiveHundredHandler->setNextHandler(twoHundredHandler);
  twoHundredHandler->setNextHandler(hundredHandler);

  // STEP 3: HEAD (thousandHandler) se dispense shuru — chain khud
  // aage badhegi jitni zaroorat hai.
  int amountToWithdraw = 4000;
  cout << "\nDispensing amount: ₹" << amountToWithdraw << endl;
  thousandHandler->dispense(amountToWithdraw);

  // NOTE: handlers delete nahi hue (chhota demo) -> leak. Modular version
  // (C++ Code/) me CashDispenseChainManager destructor me clean karta hai.
  return 0;
}

// ============================================================================
//  EXPECTED OUTPUT (₹4000, stock: 3x1000, 5x500, ...):
// ----------------------------------------------------------------------------
//  Dispensing amount: ₹4000
//  Dispensing 3 x ₹1000 notes.    <- ₹3000 diya, ₹1000 bacha -> ₹500 wale ko
//  Dispensing 2 x ₹500 notes.     <- ₹1000 diya, ₹0 bacha -> chain khatam
//
//  ₹1000 wale ne 3 (max stock) diye, ₹500 wale ne baaki 2 — total ₹4000.
//  ₹200/₹100 handlers call hi nahi hue (kuch bacha hi nahi). Yahi CoR:
//  har handler apna hissa, baaki aage. ✅
// ============================================================================
