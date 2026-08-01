// ============================================================================
//  core/VendingMachine.h  —  CONTEXT (State pattern ka dil) + FACADE
// ----------------------------------------------------------------------------
//  Ye machine State pattern ka "Context" hai (Requirement R2). Iske paas:
//     (a) DATA           -> inventory (slots), money manager, chuna hua slot
//     (b) CURRENT STATE  -> aur har public call bas usko DELEGATE ho jaati hai
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Dekho ki is class me ek bhi `if (state == ...)` NAHI hai. EK BHI.    │
//  │                                                                          │
//  │  Poori business logic states ke paas chali gayi hai. Machine sirf ek     │
//  │  "dabba" hai jo data aur current state pakde rehta hai. Har action bas   │
//  │  ek line hai:                                                            │
//  │                                                                          │
//  │      currentState_ = currentState_->insertCash(this, amount);            │
//  │                                                                          │
//  │  Machine ko pata bhi nahi ki NoMoney ke baad MoneyInserted aata hai —    │
//  │  wo bas poochti hai "ab kya?" aur jo state bataye, maan leti hai.        │
//  │  Yahi Context ka kaam hai — PATLA (thin) rehna.                          │
//  │                                                                          │
//  │  (Purana code ismein `bool itemSelected_` rakhta tha aur har function me │
//  │   uske checks bikhre the — wo State pattern ka ulta tha.)                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE bhi hai: client (main.cpp) ko Inventory, Slot, MoneyManager,
//     ya kisi State class ka naam tak nahi pata. Wo bas `machine.insertCash(50)`
//     bolta hai aur andar poora tamasha ho jaata hai.
//
//  ⭐ STATES ek baar bante hain, baar-baar nahi:
//     Constructor me 3 state objects `new` hote hain, aur machine ki poori
//     zindagi wahi 3 ghoomte rehte hain. Ye isliye possible hai kyunki states
//     STATELESS hain — unke paas apna koi member variable nahi! Saara data
//     machine ke paas hai, aur state usko `machine->` se maangta hai.
//     (L30 Flyweight ka wahi idea — shared objects jo reuse hote hain.)
// ============================================================================
#ifndef VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H
#define VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H

#include <bits/stdc++.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../enums/ItemType.h"
#include "../enums/UserRole.h"
#include "../models/Item.h"
#include "../models/Slot.h"
#include "../services/Inventory.h"
#include "../services/MoneyManager.h"
#include "../states/ConcreteStates.h"

using namespace std;

namespace vending_machine_lld {

class VendingMachine {
public:
    VendingMachine()
        : selectedSlot_(NO_SLOT_SELECTED),
          noMoneyState_(new NoMoneyInsertedState()),
          moneyInsertedState_(new MoneyInsertedState()),
          dispenseState_(new DispenseState()) {
        // Machine hamesha "koi paisa nahi" se shuru hoti hai — safe default.
        currentState_ = noMoneyState_;
    }

    // Constructor ne 3 baar `new` kiya tha -> ab 3 baar `delete`.
    //
    // ⭐ `currentState_` ko delete NAHI karna! Wo in teeno me se hi kisi EK ko
    // point kar raha hai — ek hi object ko do baar delete = crash. 💥
    // 📌 "Pointer hone ka matlab maalik hona nahi hota."
    ~VendingMachine() {
        delete noMoneyState_;
        delete moneyInsertedState_;
        delete dispenseState_;
    }

    // Copy karne se do machines ek hi state objects share kar leti, aur dono ke
    // destructor unhe delete karte -> DOUBLE FREE. Isi liye copy band.
    // (Rule of Three: destructor likha hai, to copy ctor/assignment bhi sochо.)
    VendingMachine(const VendingMachine &) = delete;
    VendingMachine &operator=(const VendingMachine &) = delete;

    // ========================================================================
    //  PUBLIC API — har call bas current state ko DELEGATE ho jaati hai
    // ========================================================================
    //  Ye 4 function hi poora State pattern hain. Dhyaan do — sab ek jaise hain:
    //  "current state se kaam karwao, aur jo state wo lauta ke de, wahi naya
    //   current state ban jaaye."

    // R6 — user cash daalta hai
    void insertCash(double amount) { currentState_ = currentState_->insertCash(this, amount); }

    // R5 — user SLOT NUMBER daal ke product chunta hai
    void selectSlot(int slotNumber) { currentState_ = currentState_->selectSlot(this, slotNumber); }

    // R8, R9, R10 — paisa check, product do / change do / refund do
    void dispense() { currentState_ = currentState_->dispense(this); }

    // Base #8 — cancel karke paisa wapas lo
    void refund() { currentState_ = currentState_->refund(this); }

    // ========================================================================
    //  ADMIN ke kaam (Requirement R3, R4)
    // ========================================================================
    //  "The admin can put new products in the machine or take them out."
    //
    //  ⭐ Ye functions `UserRole` maangte hain — aam user inhe bula nahi sakta.
    //  Ye R3 (do tarah ke log) aur R4 (admin ke haq) dono ko lagu karta hai.
    //
    //  ⚠ Ye SIMPLE model hai — koi login/password nahi. Jo khud ko ADMIN bataye
    //  wo admin hai. Asli system me yahan authentication hoti (UserRole.h me
    //  detail me likha hai). Yahan focus State pattern pe hai, security pe nahi.

    void loadItem(UserRole role, int slotNumber, const Item &item, int quantity) {
        requireAdmin(role, "product bharne");
        inventory_.addProduct(slotNumber, item, quantity);
    }

    void removeItem(UserRole role, int slotNumber) {
        requireAdmin(role, "product nikaalne");
        inventory_.removeProduct(slotNumber);
    }

    // ========================================================================
    //  Display / status
    // ========================================================================
    void showInventory() const { inventory_.printInventory(); }

    void showStatus() const {
        cout << "  [MACHINE] State: " << currentState_->getStateName() << " | Balance: ₹" << fixed << setprecision(2)
             << moneyManager_.getCurrentBalance();
        if (hasSelectedSlot()) {
            cout << " | Chuna hua slot: " << selectedSlot_;
        }
        cout << "\n";
    }

    // ========================================================================
    //  STATE GETTERS — states inhi se "agla state" maangte hain
    // ========================================================================
    //  NoMoneyInsertedState ko MoneyInsertedState ka object kahan se milega?
    //  Machine se hi: `return machine->getMoneyInsertedState();`
    //  Isi liye machine ko teeno states expose karne padte hain.
    VendingMachineState *getNoMoneyState() { return noMoneyState_; }
    VendingMachineState *getMoneyInsertedState() { return moneyInsertedState_; }
    VendingMachineState *getDispenseState() { return dispenseState_; }

    string getCurrentStateName() const { return currentState_->getStateName(); }

    // ========================================================================
    //  DATA ACCESS — states ka poora "toolkit"
    // ========================================================================
    //  States ke paas apna data nahi hai (stateless hain), to machine ka data
    //  padhne/badalne ke liye unhe ye chahiye.

    Inventory &getInventory() { return inventory_; }
    MoneyManager &getMoneyManager() { return moneyManager_; }

    // ---- Chuna hua slot ----------------------------------------------------
    //
    // ⭐ `NO_SLOT_SELECTED` (= -1) ek "sentinel value" hai — matlab "kuch chuna
    // hi nahi". Iski jagah ek alag `bool slotSelected_` bhi rakh sakte the, par
    // do variables aapas me JHOOTH bol sakte the (`slotSelected_ = true` par
    // `selectedSlot_ = 0`?). Ek hi variable = aisi ulti haalat ban hi nahi sakti.
    // (Direction.h me IDLE ke saath bhi yahi soch thi.)
    static constexpr int NO_SLOT_SELECTED = -1;

    int getSelectedSlot() const { return selectedSlot_; }
    bool hasSelectedSlot() const { return selectedSlot_ != NO_SLOT_SELECTED; }
    void setSelectedSlot(int slotNumber) { selectedSlot_ = slotNumber; }
    void clearSelectedSlot() { selectedSlot_ = NO_SLOT_SELECTED; }

    // ---- Transaction reset — ek hi jagah, taaki kuch chhoot na jaye --------
    //
    // ⭐ Ye chhota helper zaroori hai. Transaction khatam hone ke DO raaste hain:
    //    (1) product mil gaya (DispenseState)
    //    (2) refund ho gaya (R10 ya user ne cancel kiya)
    // Dono me DONO cheezein saaf honi chahiye — balance AUR chuna hua slot.
    // Agar ye do jagah alag-alag likhte, to ek jagah `clearSelectedSlot()`
    // bhoolna bahut aasan hota — aur phir agla user aake `dispense()` dabata
    // to purana slot chal jaata! 😱 Ek helper = ek jagah, koi galti nahi.
    void resetTransaction() {
        moneyManager_.resetBalance();
        clearSelectedSlot();
    }

private:
    Inventory inventory_;       // slots ka stock (R1, R4, R5)
    MoneyManager moneyManager_; // paisa (R6, R7)
    int selectedSlot_;          // user ne kaunsa slot chuna (R5)

    // ---- Teen states — ek baar bante hain, hamesha reuse hote hain (R2) ----
    VendingMachineState *currentState_; // abhi machine kis state me hai
    VendingMachineState *noMoneyState_;
    VendingMachineState *moneyInsertedState_;
    VendingMachineState *dispenseState_;

    // Admin check — ek jagah, taaki har admin function me copy-paste na ho.
    void requireAdmin(UserRole role, const string &action) const {
        if (role != UserRole::ADMIN) {
            throw runtime_error("❌ Sirf ADMIN " + action + " ka kaam kar sakta hai (aapka role: " +
                                userRoleToString(role) + ")");
        }
    }
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_CORE_VENDINGMACHINE_H
