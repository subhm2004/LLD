// ============================================================================
//  states/ConcreteStates.h  —  Teeno asli states (Requirement R2)
// ----------------------------------------------------------------------------
//  R2 ke teen states, teen classes:
//     1. NoMoneyInsertedState -> machine me koi paisa nahi
//     2. MoneyInsertedState   -> paisa daal diya gaya hai
//     3. DispenseState        -> machine product bahar nikaal rahi hai
//
//  ============================================================================
//   STATE TRANSITION TABLE — poori machine ek nazar me
//   (row = abhi kaunsa state, column = kya kiya, cell = agla state)
//  ----------------------------------------------------------------------------
//                    | insertCash    | selectSlot   | dispense        | refund
//   -----------------+---------------+--------------+-----------------+----------
//   NO_MONEY         | MONEY_INSERTED| NO_MONEY     | NO_MONEY        | NO_MONEY
//                    |      ✅       | (slot yaad   | (error: "pehle  | (kuch
//                    |               |  rakh liya)  |  paisa daalo")  |  nahi)
//   -----------------+---------------+--------------+-----------------+----------
//   MONEY_INSERTED   | MONEY_INSERTED| MONEY_INSERTED| DISPENSE ✅    | NO_MONEY
//                    | (paisa jud    | (slot badal  |  (agar paisa    |  ✅
//                    |  gaya, R7)    |  sakte ho)   |   poora — R8)   | (paisa
//                    |               |              | ya NO_MONEY     |  wapas)
//                    |               |              |  (kam paisa ->  |
//                    |               |              |   REFUND, R10)  |
//   -----------------+---------------+--------------+-----------------+----------
//   DISPENSE         | DISPENSE      | DISPENSE     | NO_MONEY ✅     | DISPENSE
//                    | (ruko, nikal  | (ruko)       | (product + change|  (ruko)
//                    |  raha hai)    |              |  — R9)          |
//
//   ⭐ Notice: har cell BHARA hua hai. Koi bhi galat action kisi bhi state me
//   karo — machine crash nahi hogi, bas polite error de ke wahin ruk jaayegi.
//   "Invalid action" handling apne aap ho jaati hai, kyunki HAR state ne HAR
//   action ka jawab likha hai. Yahi pattern ki sabse badi khoobi hai.
//
//   📌 SABSE BADA RULE — STATE AUR DATA KABHI JHOOTH NA BOLEIN:
//        state == NO_MONEY        <=>  balance == 0
//        state == MONEY_INSERTED  =>   balance > 0
//   Yaani agar machine kehti hai "koi paisa nahi", to asal me balance 0 hona
//   HI chahiye. Isi liye har refund/dispense ke baad balance reset hota hai.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_STATES_CONCRETESTATES_H
#define VENDING_MACHINE_LLD_STATES_CONCRETESTATES_H

#include <iomanip>
#include <iostream>

#include "VendingMachineState.h"

using namespace std;

namespace vending_machine_lld {

// ============================================================================
//  1. NO_MONEY_INSERTED — "machine khadi hai, paisa daalo" (R2)
// ----------------------------------------------------------------------------
//  Machine chalu hai, stock hai, par abhi tak kisi ne paisa nahi daala.
//  Yahan sirf DO cheezein sahi hain: paisa daalna, ya slot chunna.
// ============================================================================
class NoMoneyInsertedState : public VendingMachineState {
public:
    // ✅ Sahi action — paisa aaya (R6)
    VendingMachineState *insertCash(VendingMachine *machine, double amount) override;

    // ✅ Sahi action — slot pehle bhi chun sakte ho (paise se pehle)
    //
    // Asli machine me dono chalta hai: pehle item chuno phir paisa daalo, ya
    // pehle paisa daalo phir item. Hum dono allow karte hain — user-friendly.
    VendingMachineState *selectSlot(VendingMachine *machine, int slotNumber) override;

    // ❌ Galat action — bina paise ke product maang rahe ho
    VendingMachineState *dispense(VendingMachine *machine) override;

    // ❌ Galat action — jo paisa daala hi nahi, wo wapas kya karein?
    VendingMachineState *refund(VendingMachine *machine) override;

    string getStateName() override { return "NO_MONEY_INSERTED"; }
};

// ============================================================================
//  2. MONEY_INSERTED — "paisa pada hai, ab product chuno" (R2)
// ----------------------------------------------------------------------------
//  ⭐ Ye sabse "interesting" state hai — asli business logic yahi hai
//  (R8: paisa poora hai?, R10: kam paisa -> refund). Baaki states zyadatar
//  error handling hi hain.
// ============================================================================
class MoneyInsertedState : public VendingMachineState {
public:
    // ✅ Aur paisa daal sakte ho — 20 daala tha, 20 aur daalo = 40 (R7)
    VendingMachineState *insertCash(VendingMachine *machine, double amount) override;

    // ✅ Slot chuno (ya badal do — mann badal gaya to koi baat nahi)
    VendingMachineState *selectSlot(VendingMachine *machine, int slotNumber) override;

    // ⭐ Ye is poori file ka sabse important function hai — R8 aur R10 yahin hain
    VendingMachineState *dispense(VendingMachine *machine) override;

    // ✅ Cancel karo, poora paisa wapas lo (base requirement #8)
    VendingMachineState *refund(VendingMachine *machine) override;

    string getStateName() override { return "MONEY_INSERTED"; }
};

// ============================================================================
//  3. DISPENSE — "product nikal raha hai, ruko" (R2)
// ----------------------------------------------------------------------------
//  Ye ek "busy"/transient state hai — machine ka motor chal raha hai. Yahan
//  sirf EK cheez allowed hai: dispense poora karna. Baaki sab "ruko" bolega.
// ============================================================================
class DispenseState : public VendingMachineState {
public:
    // ❌ Beech me paisa mat daalo
    VendingMachineState *insertCash(VendingMachine *machine, double amount) override;

    // ❌ Ek baar me ek hi product
    VendingMachineState *selectSlot(VendingMachine *machine, int slotNumber) override;

    // ✅ Asli kaam — product bahar + change wapas (R9)
    VendingMachineState *dispense(VendingMachine *machine) override;

    // ❌ Product nikal raha hai, ab refund nahi
    VendingMachineState *refund(VendingMachine *machine) override;

    string getStateName() override { return "DISPENSE"; }
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_STATES_CONCRETESTATES_H
