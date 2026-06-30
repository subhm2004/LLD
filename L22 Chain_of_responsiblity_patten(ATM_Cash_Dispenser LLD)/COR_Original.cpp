// ============================================================================
//  COR_Original.cpp  —  Chain of Responsibility (original single-file version)
// ----------------------------------------------------------------------------
//  CoR: ek request ko handlers ki CHAIN me se guzaaro; har handler apna hissa
//  karke baaki next ko de deta hai. Yahan ATM cash dispense: ₹1000 -> ₹500 ->
//  ₹200 -> ₹100 handlers. Yeh original monolithic version hai; modular,
//  layered version `C++ Code/` folder me hai.
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

// Abstract Handler (Base Class) — Chain of Responsibility
class MoneyHandler {
protected:
  // Pointer to the next handler in the chain
  MoneyHandler *nextHandler;

public:
  // Constructor to initialize the next handler pointer to nullptr
  MoneyHandler() { this->nextHandler = nullptr; }

  // Set the next handler in the chain
  void setNextHandler(MoneyHandler *next) { nextHandler = next; }

  // Pure virtual function to dispense the money
  virtual void dispense(int amount) = 0;

  // Destructor to clean up the memory
  virtual ~MoneyHandler() = default;
};

// Concrete Handler for ₹1000 notes
class ThousandHandler : public MoneyHandler {
private:
  int numNotes;

public:
  // Constructor to initialize the number of notes
  ThousandHandler(int numNotes) { this->numNotes = numNotes; }

  // Dispense the money
  void dispense(int amount) override {
    int notesNeeded = amount / 1000;

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
      cout << "Dispensing " << notesNeeded << " x ₹1000 notes.\n";

    int remainingAmount = amount - (notesNeeded * 1000);
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
  // Create the handlers
  MoneyHandler *thousandHandler = new ThousandHandler(3);
  MoneyHandler *fiveHundredHandler = new FiveHundredHandler(5);
  MoneyHandler *twoHundredHandler = new TwoHundredHandler(10);
  MoneyHandler *hundredHandler = new HundredHandler(20);

  // Link the handlers
  thousandHandler->setNextHandler(fiveHundredHandler);
  fiveHundredHandler->setNextHandler(twoHundredHandler);
  twoHundredHandler->setNextHandler(hundredHandler);

  // Withdraw the money
  int amountToWithdraw = 4000;

  cout << "\nDispensing amount: ₹" << amountToWithdraw << endl;
  // Dispense the money
  thousandHandler->dispense(amountToWithdraw);

  return 0;
}
