// ============================================================================
//  main.cpp  —  Vending Machine ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Har scene ek ya do requirements ko CHALA ke dikhata hai — sirf likha nahi,
//  asli output me dikhta hai.
//
//  ⭐ CLIENT KITNA KAM JAANTA HAI (Facade ka fayda):
//     System me 8+ classes hain — Slot, Item, Inventory, MoneyManager, aur 3
//     State classes. Yahan kitni dikhti hain? Bas `VendingMachine`, `Item`,
//     aur do enums. Kisi State class ka naam tak nahi aata! Client bas
//     `machine.insertCash(50)` bolta hai.
//
//  ============================================================================
//   DEMO ka plan — kaunsa scene kaunsi requirement dikhata hai
//  ----------------------------------------------------------------------------
//     Scene 1 -> R3, R4     Admin stock bharta hai; aam user nahi kar sakta
//     Scene 2 -> R1, R5     Slots + slot number se selection
//     Scene 3 -> R2         Teen states + galat action pe polite error
//     Scene 4 -> R6, R7, R9 Cash, total, aur ZYADA paise pe change
//     Scene 5 -> R8, R10 ⭐ KAM paise pe error + POORA PAISA WAPAS (bug tha!)
//     Scene 6 -> R4         Admin product nikaal deta hai
//     Scene 7 -> #8 (base)  User cancel karke refund leta hai
//  ============================================================================
#include <bits/stdc++.h>

// ⚠ `StateImpl.h` include karna ZAROORI hai (sirf VendingMachine.h nahi) —
// kyunki states ki asli BODY usme hai. Bina iske linker error aayega:
// "undefined symbol: NoMoneyInsertedState::insertCash". (Ye circular dependency
//  todne ki keemat hai — StateImpl.h me poora explanation hai.)
#include "states/StateImpl.h"

using namespace std;
using namespace vending_machine_lld;

// Chhota helper — har scene ka heading saaf dikhe.
static void scene(const string &title) {
    cout << "\n\n╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  " << title << "\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

int main() {
    VendingMachine machine;

    // ========================================================================
    //  SCENE 1 — ADMIN vs USER (Requirement R3, R4)
    // ========================================================================
    scene("SCENE 1 — Admin stock bharta hai (R3, R4)");

    // ⭐ R4: "The admin can put new products in the machine or take them out."
    // Har product apne SLOT me (R1).
    machine.loadItem(UserRole::ADMIN, 101, Item("Coke Can", 40.0, ItemType::COKE), 3);
    machine.loadItem(UserRole::ADMIN, 102, Item("Pepsi Can", 35.0, ItemType::PEPSI), 2);
    machine.loadItem(UserRole::ADMIN, 103, Item("Mineral Water", 20.0, ItemType::WATER), 5);
    machine.loadItem(UserRole::ADMIN, 104, Item("Orange Juice", 50.0, ItemType::JUICE), 2);

    cout << "\n--- Ab aam USER stock bharne ki koshish karta hai (R3) ---\n";
    try {
        machine.loadItem(UserRole::USER, 105, Item("Free Coke", 0.0, ItemType::COKE), 99);
        cout << "❌ Ye nahi hona chahiye tha!\n";
    } catch (const exception &e) {
        cout << "✅ Reject: " << e.what() << "\n";
    }

    // ========================================================================
    //  SCENE 2 — SLOTS (Requirement R1, R5)
    // ========================================================================
    scene("SCENE 2 — Slots (R1) + slot number se selection (R5)");

    machine.showInventory();

    cout << "--- Galat slot number daalte hain ---\n";
    try {
        machine.selectSlot(999);
    } catch (const exception &e) {
        cout << "✅ Reject: " << e.what() << "\n";
    }

    // ========================================================================
    //  SCENE 3 — STATE PATTERN (Requirement R2)
    // ========================================================================
    scene("SCENE 3 — ⭐ Teen states (R2): galat action pe polite error");

    machine.showStatus(); // NO_MONEY_INSERTED

    cout << "\n--- Bina paise ke dispense dabate hain ---\n";
    machine.dispense(); // NoMoneyInsertedState mana karega
    machine.showStatus();

    cout << "\n--- Bina paise ke refund dabate hain ---\n";
    machine.refund(); // ye bhi mana
    machine.showStatus();

    cout << "\n📌 Notice: machine CRASH nahi hui — har galat action pe bas\n";
    cout << "   polite error aur wahi state. Ye State pattern ka fayda hai:\n";
    cout << "   har state ne HAR action ka jawab likha hai.\n";

    // ========================================================================
    //  SCENE 4 — CASH + CHANGE (Requirement R6, R7, R9)
    // ========================================================================
    scene("SCENE 4 — Cash (R6), total (R7), zyada paise pe change (R9)");

    machine.selectSlot(103); // Mineral Water — ₹20
    machine.showStatus();

    cout << "\n--- R6: cash daalte hain (do baar — R7 jodta rehta hai) ---\n";
    machine.insertCash(10.0); // NO_MONEY -> MONEY_INSERTED
    machine.showStatus();
    machine.insertCash(40.0); // 10 + 40 = 50 (R7)
    machine.showStatus();

    cout << "\n--- R9: ₹50 daale, paani ₹20 ka -> ₹30 change milna chahiye ---\n";
    machine.dispense(); // MONEY_INSERTED -> DISPENSE (paisa poora hai)
    machine.showStatus();
    machine.dispense(); // DISPENSE -> product + change -> NO_MONEY
    machine.showStatus();

    // ========================================================================
    //  SCENE 5 — ⭐ KAM PAISE (Requirement R8, R10) — yahan BUG tha!
    // ========================================================================
    scene("SCENE 5 — ⭐ Kam paise: error + POORA paisa wapas (R8, R10)");

    cout << "R10: 'If the money is less than the price, the machine shows an\n";
    cout << "      error message and gives the money back.'\n\n";

    machine.selectSlot(104); // Orange Juice — ₹50
    machine.insertCash(25.0); // sirf 25 — 25 kam hain
    machine.showStatus();

    cout << "\n--- Ab dispense dabate hain (paise kam hain) ---\n";
    machine.dispense();
    machine.showStatus();

    // ⭐ ASLI TEST — kya paisa WAPAS mila?
    cout << "\n>>> Balance ab ₹0.00 hona CHAHIYE (paisa wapas mil gaya):\n";
    if (machine.getCurrentStateName() == "NO_MONEY_INSERTED") {
        cout << "    ✅ SAHI! State NO_MONEY_INSERTED hai — matlab paisa wapas ho gaya\n";
        cout << "    📌 Purane code me ye BUG tha: error to aata tha, par ₹25\n";
        cout << "       machine me hi atka reh jaata tha. Ab fix hai.\n";
    } else {
        cout << "    ❌ BUG! Paisa abhi bhi andar hai\n";
    }

    // ========================================================================
    //  SCENE 6 — ADMIN product nikaalta hai (Requirement R4)
    // ========================================================================
    scene("SCENE 6 — Admin product nikaal deta hai (R4)");

    machine.removeItem(UserRole::ADMIN, 102); // Pepsi hata do
    machine.showInventory();

    cout << "--- Ab us slot ko chunne ki koshish ---\n";
    try {
        machine.selectSlot(102);
    } catch (const exception &e) {
        cout << "✅ Reject: " << e.what() << "\n";
    }

    // ========================================================================
    //  SCENE 7 — CANCEL + REFUND (base requirement #8)
    // ========================================================================
    scene("SCENE 7 — User cancel karke paisa wapas leta hai (base #8)");

    machine.selectSlot(101); // Coke — ₹40
    machine.insertCash(100.0);
    machine.showStatus();

    cout << "\n--- User ka mann badal gaya, cancel dabaya ---\n";
    machine.refund();
    machine.showStatus();

    // ========================================================================
    scene("Demo khatam — final stock");
    machine.showInventory();

    // ⭐ Koi `delete` nahi! `machine` stack pe hai — scope khatam hote hi uska
    // destructor apne aap chalega aur teeno state objects delete kar dega.
    return 0;
}
