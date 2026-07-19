// ============================================================================
//  states/StateImpl.h  —  Teeno states ki ASLI BODY (Requirement R2, R8-R10)
// ----------------------------------------------------------------------------
//  ⭐ YE FILE ALAG KYUN HAI? — circular dependency todne ke liye
//
//  Murgi-anda problem:
//     ConcreteStates.h  ko `VendingMachine*` chahiye  (function params me)
//     VendingMachine.h  ko `NoMoneyInsertedState` chahiye (constructor me `new`)
//  Dono ek doosre ko chahte hain! Koi bhi "pehle" nahi likha ja sakta.
//
//  Hal — TEEN hisso me todo:
//     1. ConcreteStates.h -> `class VendingMachine;` (forward decl) +
//                            functions ka DECLARATION (body nahi)
//     2. VendingMachine.h -> ConcreteStates.h include karo, ab `new` kar sakte
//     3. YE FILE          -> VendingMachine.h include karo, ab BODY likho ✅
//
//  Ab tak dono classes poori define ho chuki hain, to yahan `machine->...`
//  bulana bilkul safe hai.
//
//  📌 Ye C++ ka aam muhavara hai — jab do classes ek doosre ko chahiye, to
//     declaration aur definition alag files me todo. (L34 me BoardSetupBridge.h
//     bhi bilkul isi wajah se alag file thi.)
//
//  💡 Har function pe `inline` kyun? Ye header hai — agar 2 alag .cpp me
//     include ho gayi to har function ki DO copy ban jaati aur linker
//     "duplicate symbol" ka error deta. `inline` kehta hai "duplicate theek
//     hai, ek chun lo".
// ============================================================================
#ifndef VENDING_MACHINE_LLD_STATES_STATEIMPL_H
#define VENDING_MACHINE_LLD_STATES_STATEIMPL_H

#include <iomanip>
#include <iostream>

#include "../core/VendingMachine.h"
#include "ConcreteStates.h"

using namespace std;

namespace vending_machine_lld {

// ============================================================================
//  1. NO_MONEY_INSERTED — "paisa daalo pehle"
// ============================================================================

// ✅ Sahi action — paisa aaya (R6)
inline VendingMachineState *NoMoneyInsertedState::insertCash(VendingMachine *machine, double amount) {
    machine->getMoneyManager().insertMoney(amount); // andar validation hai (amount > 0)
    cout << "  💰 ₹" << fixed << setprecision(2) << amount
         << " daala | Balance: ₹" << machine->getMoneyManager().getCurrentBalance() << "\n";

    // ⭐ TRANSITION! Ab machine MONEY_INSERTED me chali gayi.
    // Agli baar `insertCash` bulao to MoneyInsertedState::insertCash chalega —
    // wahi function NAHI jo abhi chala. Behavior badal gaya kyunki state badla.
    // Yahi poore pattern ka nichod hai.
    return machine->getMoneyInsertedState();
}

// ✅ Slot pehle bhi chun sakte ho (paise se pehle) — asli machine me bhi chalta
inline VendingMachineState *NoMoneyInsertedState::selectSlot(VendingMachine *machine, int slotNumber) {
    const Slot &slot = machine->getInventory().getSlot(slotNumber); // galat slot -> throw

    if (!machine->getInventory().hasStock(slotNumber)) {
        cout << "  ❌ " << slot.getItem().getName() << " out of stock hai\n";
        return machine->getNoMoneyState(); // wahi ke wahi
    }

    machine->setSelectedSlot(slotNumber);
    cout << "  🔢 Slot " << slotNumber << " chuna: " << slot.getItem().getName() << " | Daam: ₹" << fixed
         << setprecision(2) << slot.getItem().getPrice() << "\n";
    cout << "     (ab ₹" << slot.getItem().getPrice() << " daalo)\n";

    // Paisa abhi bhi 0 hai -> state nahi badla. Slot bas yaad rakh liya.
    return machine->getNoMoneyState();
}

// ❌ Galat action — bina paise ke product maang rahe ho
inline VendingMachineState *NoMoneyInsertedState::dispense(VendingMachine *machine) {
    cout << "  ❌ Pehle paisa daalo!\n";
    // Khud ko wapas kar do = "state nahi badla". Crash nahi, exception nahi,
    // bas ek saaf message. Har "galat" action ka handling itni hi simple hai.
    return machine->getNoMoneyState();
}

// ❌ Galat action — jo paisa daala hi nahi, wo wapas kya karein?
inline VendingMachineState *NoMoneyInsertedState::refund(VendingMachine *machine) {
    cout << "  ❌ Koi paisa daala hi nahi — refund kya karein?\n";
    return machine->getNoMoneyState();
}

// ============================================================================
//  2. MONEY_INSERTED — "paisa pada hai, ab product chuno"
//  ⭐ Asli business logic yahi hai (R7, R8, R10)
// ============================================================================

// ✅ Aur paisa daal sakte ho (R7 — machine jodti rehti hai)
inline VendingMachineState *MoneyInsertedState::insertCash(VendingMachine *machine, double amount) {
    machine->getMoneyManager().insertMoney(amount);
    cout << "  💰 ₹" << fixed << setprecision(2) << amount << " aur daala | Balance: ₹"
         << machine->getMoneyManager().getCurrentBalance() << "\n";

    // Paisa aur jud gaya, par state wahi (paisa to pehle bhi tha).
    return machine->getMoneyInsertedState();
}

// ✅ Slot chuno / badal do (mann badal gaya to koi baat nahi)
inline VendingMachineState *MoneyInsertedState::selectSlot(VendingMachine *machine, int slotNumber) {
    const Slot &slot = machine->getInventory().getSlot(slotNumber);

    if (!machine->getInventory().hasStock(slotNumber)) {
        cout << "  ❌ " << slot.getItem().getName() << " out of stock — doosra slot chuno ya refund lo\n";
        return machine->getMoneyInsertedState();
    }

    machine->setSelectedSlot(slotNumber);
    cout << "  🔢 Slot " << slotNumber << " chuna: " << slot.getItem().getName() << " | Daam: ₹" << fixed
         << setprecision(2) << slot.getItem().getPrice() << "\n";
    return machine->getMoneyInsertedState();
}

// ============================================================================
//  ⭐⭐ SABSE IMPORTANT FUNCTION — R8, R9, R10 teeno yahin decide hote hain
// ============================================================================
inline VendingMachineState *MoneyInsertedState::dispense(VendingMachine *machine) {
    // Slot chuna hi nahi? To product kya dein?
    if (!machine->hasSelectedSlot()) {
        cout << "  ❌ Pehle slot number daal ke product chuno\n";
        return machine->getMoneyInsertedState();
    }

    const int slotNumber = machine->getSelectedSlot();
    const Slot &slot = machine->getInventory().getSlot(slotNumber);
    const double price = slot.getItem().getPrice();
    const double paid = machine->getMoneyManager().getCurrentBalance();

    // ---- Stock ka double-check (base requirement #5) ----
    // Slot chunte waqt bhi check kiya tha, par tab se ab tak stock khatam ho
    // sakta hai (admin ne nikaal liya?). Paisa lene se PEHLE dobara check karo.
    // "Defense in depth" — do jagah check karna zyada nahi, zaroori hai.
    if (!machine->getInventory().hasStock(slotNumber)) {
        cout << "  ❌ " << slot.getItem().getName() << " abhi out of stock ho gaya — paisa wapas\n";
        cout << "  💵 Refund: ₹" << fixed << setprecision(2) << paid << "\n";
        machine->resetTransaction();
        return machine->getNoMoneyState();
    }

    // ========================================================================
    //  ⭐ R10 — KAM PAISA -> error + POORA PAISA WAPAS
    // ========================================================================
    //  "R10: If the money is less than the price, the machine shows an error
    //        message and gives the money back."
    //
    //  🐛 PURANE CODE ME YE ASLI BUG THA:
    //         if (paid < price) {
    //             throw runtime_error("Insufficient amount...");  // ← bas!
    //         }
    //     Error to phenk diya, par paisa MACHINE ME HI ATKA REH GAYA! 💸
    //     Balance reset hota hi nahi tha. Test karke pakda tha: ₹25 daala,
    //     error mila, phir ₹15 aur daala to balance ₹40 ho gaya — matlab wo
    //     ₹25 andar hi pada tha. User ka paisa kha gayi machine.
    //
    //  ✅ Ab: error message BHI, aur poora paisa WAPAS bhi — jaisa R10 kehta hai.
    if (paid < price) {
        cout << "  ❌ ERROR: Paise kam hain! " << slot.getItem().getName() << " ka daam ₹" << fixed << setprecision(2)
             << price << " hai, aapne ₹" << paid << " daale (₹" << (price - paid) << " kam)\n";
        cout << "  💵 Poora paisa wapas: ₹" << paid << "\n";

        machine->resetTransaction(); // ⭐ YAHI wo fix hai — balance 0, slot clear
        return machine->getNoMoneyState();
    }

    // ---- R8 — paisa poora hai (ya zyada) -> aage badho ----
    cout << "  ✅ Paisa poora hai (₹" << fixed << setprecision(2) << paid << " >= ₹" << price << ") — nikaal rahe hain...\n";

    // ⭐ Yahan paisa NAHI kaata — wo DispenseState karega.
    //
    // Kyun? Kyunki paisa katna aur product dena EK SAATH hona chahiye (atomic).
    // Agar yahan paisa kaat lete aur DispenseState kabhi chalta hi nahi, to
    // paisa gaya aur product mila hi nahi. Dono ek jagah = ya dono, ya kuch nahi.
    return machine->getDispenseState();
}

// ✅ Cancel karo, poora paisa wapas (base requirement #8)
inline VendingMachineState *MoneyInsertedState::refund(VendingMachine *machine) {
    const double amount = machine->getMoneyManager().getCurrentBalance();
    cout << "  💵 Transaction cancel — refund: ₹" << fixed << setprecision(2) << amount << "\n";
    machine->resetTransaction();
    return machine->getNoMoneyState();
}

// ============================================================================
//  3. DISPENSE — "product nikal raha hai, ruko"
// ============================================================================

// ❌ Beech me paisa mat daalo
inline VendingMachineState *DispenseState::insertCash(VendingMachine *machine, double amount) {
    cout << "  ❌ Ruko! Product nikal raha hai. Aapka ₹" << fixed << setprecision(2) << amount << " wapas\n";
    // Notice: `insertMoney` bulaya hi NAHI — yahi "paisa wapas kar diya" hai.
    // Machine ne liya hi nahi, to wapas karne ko kuch karna hi nahi tha.
    return machine->getDispenseState();
}

// ❌ Ek baar me ek hi product
inline VendingMachineState *DispenseState::selectSlot(VendingMachine *machine, int slotNumber) {
    cout << "  ❌ Ruko! Pehle current product nikalne do (slot " << slotNumber << " baad me)\n";
    return machine->getDispenseState();
}

// ============================================================================
//  ⭐ ASLI KAAM — product bahar + change wapas (R9)
// ============================================================================
inline VendingMachineState *DispenseState::dispense(VendingMachine *machine) {
    const int slotNumber = machine->getSelectedSlot();
    const Slot &slot = machine->getInventory().getSlot(slotNumber);
    const double price = slot.getItem().getPrice();
    const double paid = machine->getMoneyManager().getCurrentBalance();

    // ⭐ Product aur paisa EK SAATH (atomic) — MoneyInsertedState me isi liye
    // paisa nahi kaata tha. Ya dono hote hain, ya kuch nahi.
    machine->getInventory().dispenseFromSlot(slotNumber); // stock -1
    const double change = paid - price;

    cout << "  📦 Product nikla: " << slot.getItem().getName() << " (slot " << slotNumber << ")\n";

    // ---- R9 — zyada paisa tha? change wapas ----
    if (change > 0) {
        cout << "  💵 Change wapas: ₹" << fixed << setprecision(2) << change << "\n";
    } else {
        cout << "  💵 Change: ₹0.00 (poore paise the)\n";
    }

    // Transaction khatam — balance 0, slot clear.
    // (Ek hi helper se dono, taaki koi cheez chhoot na jaye. VendingMachine.h
    //  me `resetTransaction` pe note padho.)
    machine->resetTransaction();

    cout << "  ✅ Transaction poora! Machine phir se taiyaar.\n";
    return machine->getNoMoneyState(); // circle poora — wapas shuruaat me
}

// ❌ Product nikal raha hai, ab refund nahi
inline VendingMachineState *DispenseState::refund(VendingMachine *machine) {
    cout << "  ❌ Ab refund nahi ho sakta — product nikal raha hai\n";
    return machine->getDispenseState();
}

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_STATES_STATEIMPL_H
