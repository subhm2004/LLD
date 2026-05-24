/*
 * Chain of Responsibility — ATM cash dispense (L22)
 *
 * Layered layout:
 *   enums/     → NoteDenomination, DispenseStatus
 *   models/    → CashCassette, ATMCashInventory, WithdrawalRequest
 *   handlers/  → MoneyHandler + concrete handlers (original CoR logic)
 *   managers/  → CashDispenseChainManager (builds ₹1000→₹500→₹200→₹100 chain)
 *   services/  → CashDispenseService (withdraw API)
 *
 * Handler dispense() logic is unchanged from the original single-file demo.
 */

#include <bits/stdc++.h>

#include "models/ATMCashInventory.h"
#include "models/WithdrawalRequest.h"
#include "services/CashDispenseService.h"

using namespace std;

int main() {
    using namespace cor_atm;

    // Same stock as original demo: 3×1000, 5×500, 10×200, 20×100
    ATMCashInventory inventory;
    inventory.addCassette(NoteDenomination::THOUSAND, 3);
    inventory.addCassette(NoteDenomination::FIVE_HUNDRED, 5);
    inventory.addCassette(NoteDenomination::TWO_HUNDRED, 10);
    inventory.addCassette(NoteDenomination::HUNDRED, 20);

    // Service → Manager → Handler chain (CoR)
    CashDispenseService dispenseService;
    dispenseService.initialize(inventory);
    dispenseService.withdraw(WithdrawalRequest(4000));

  return 0;
}
