#include <bits/stdc++.h>

#include "core/ATMSystem.h"

using namespace std;
using namespace atm_lld;

int main() {
  ATMSystem *atm = new ATMSystem();

  atm->addAccount(BankAccount("ACC_1001", 25000.0));
  atm->addCard(Card("CARD_001", "1234", "ACC_1001"));

  if (!atm->login("CARD_001", "1234")) {
    cout << "Authentication failed\n";
    return 0;
  }

  cout << "Login success\n";
  cout << "Current Balance: " << atm->checkBalance() << "\n";

  map<int, int> dispensedNotes = atm->withdrawCash(2300);
  cout << "Withdraw successful. Notes:\n";
  for (const auto &entry : dispensedNotes) {
    cout << entry.first << " x " << entry.second << "\n";
  }

  cout << "Balance after withdrawal: " << atm->checkBalance() << "\n";
  atm->logout();
  delete atm;
  return 0;
}
