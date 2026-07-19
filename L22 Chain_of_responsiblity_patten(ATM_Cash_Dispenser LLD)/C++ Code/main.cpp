// ============================================================================
//  main.cpp — CHAIN OF RESPONSIBILITY: ATM cash dispenser (MODULAR version)
// ----------------------------------------------------------------------------
//  COR_Original.cpp ka MODULAR/LAYERED version — same CoR logic, par clean
//  folders me tuta hua (real-project structure).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  LAYERS (har concern apni jagah):                                       │
//  │    enums/     -> NoteDenomination (1000/500/200/100), DispenseStatus    │
//  │    models/    -> CashCassette (ek drawer), ATMCashInventory (sab stock),│
//  │                  WithdrawalRequest (kitna nikalna hai)                  │
//  │    handlers/  -> MoneyHandler + 4 concrete (CoR ka asli dil)            │
//  │    managers/  -> CashDispenseChainManager (chain WIRE karta hai)        │
//  │    services/  -> CashDispenseService (client-facing withdraw API)       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  FLOW: main -> Service.withdraw() -> Manager ne bana rakhi chain ->
//        handlers ek-ek karke notes dispense karte hain (CoR).
//
//  MONOLITH vs MODULAR: dispense() ka CoR logic BILKUL SAME hai dono me —
//  farq sirf organization ka. Monolith seekhne ke liye (sab dikhta hai),
//  modular real code jaisa (inventory model, service layer, clean cleanup).
// ============================================================================

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
