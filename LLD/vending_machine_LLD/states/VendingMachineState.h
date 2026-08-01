// ============================================================================
//  states/VendingMachineState.h  —  STATE PATTERN ka interface (Requirement R2)
// ----------------------------------------------------------------------------
//  "R2: The vending machine can be in one of three states:
//        - NoMoneyInsertedState : No money is in the machine
//        - MoneyInsertedState   : Money has been put into the machine
//        - DispenseState        : The machine gives out the product"
//
//  State Pattern = "object ka BEHAVIOR uske ANDAR ke state se badal jaata hai,
//                   aur bahar se aisa lagta hai jaise class hi badal gayi ho."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ EK HI BUTTON, TEEN ALAG BEHAVIOR — yahi State pattern hai            │
//  │                                                                          │
//  │  `dispense()` button dabao:                                             │
//  │     NoMoney me      -> "pehle paisa daalo!" (mana)                      │
//  │     MoneyInserted me-> paisa check karo, aur DispenseState me jao        │
//  │     Dispense me     -> product bahar nikaalo + change do                 │
//  │                                                                          │
//  │  Ek hi function, teen alag kaam. Kaunsa chalega — ye STATE decide karta. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  BINA State pattern ke code ka haal (purana code aisa hi tha):           │
//  │                                                                          │
//  │     void dispense() {                                                    │
//  │         if (!itemSelected_) throw ...                                    │
//  │         if (paid < price)   throw ...                                    │
//  │         ...                                                              │
//  │     }                                                                    │
//  │     void insertMoney() { ...wahi checks dobara... }                      │
//  │     void selectItem()  { ...teesri baar... }                             │
//  │                                                                          │
//  │  Har function me `bool itemSelected_` aur balance ke checks bikhre hue.  │
//  │  Naya state chahiye (jaise SOLD_OUT ya MAINTENANCE)? Har function me     │
//  │  jaake branch add karo — aur ek bhi bhoole to chup-chaap bug. 😵         │
//  │                                                                          │
//  │  State pattern se: naya state = nayi class, bas. Purana code CHHUNA      │
//  │  nahi padta. Ye OPEN/CLOSED PRINCIPLE hai.                              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SABSE ZAROORI DESIGN CHOICE — har function `VendingMachineState*` LAUTATA hai:
//
//      currentState_ = currentState_->insertCash(this, amount);
//                      └────── purana state kaam karta hai ──────┘
//      └── aur jo lauta ke deta hai, WAHI naya state ban jaata hai ──┘
//
//  Matlab: TRANSITION ka faisla STATE khud leta hai, machine nahi. Machine
//  bilkul "bewakoof" hai — usko pata hi nahi ki NoMoney ke baad MoneyInserted
//  aata hai. Wo bas poochti hai "ab kya?" aur jo state bataye, maan leti hai.
//
//  "Wahi ke wahi raho" ka matlab bhi yahi — state apne aap ko wapas kar deta
//  hai (`return machine->getNoMoneyState();`). Koi special case likhne ki
//  zaroorat hi nahi.
//
//  📌 Ye pattern ki poori theory aur ek doosra example: `L32 State_design_pattern/`
//     (wahan 4 states hain — NoCoin/HasCoin/Dispense/SoldOut. Yahan requirement
//      ne 3 maange hain, isi liye 3 hi hain.)
// ============================================================================
#ifndef VENDING_MACHINE_LLD_STATES_VENDINGMACHINESTATE_H
#define VENDING_MACHINE_LLD_STATES_VENDINGMACHINESTATE_H

#include <string>

using namespace std;

namespace vending_machine_lld {

// ---- FORWARD DECLARATION — murgi-anda problem ka hal -----------------------
//  State ke functions ko `VendingMachine*` chahiye (param me), aur
//  VendingMachine ko `VendingMachineState*` chahiye (member me). Dono ek doosre
//  ko chahte hain — koi "pehle" likha hi nahi ja sakta.
//
//  `class VendingMachine;` ka matlab: "compiler, itna maan le ki ye class HAI.
//   Andar kya hai wo baad me bataunga." POINTER banane ke liye itna hi kaafi
//  hai (har pointer ka size same hota hai). Poori definition tabhi chahiye jab
//  uske MEMBERS use karo — aur wo `.h` file states ke baad include hoti hai.
class VendingMachine;

class VendingMachineState {
public:
    // ---- Chaar actions jo har state ko handle karne HI padenge -------------
    //
    // `= 0` -> pure virtual. Compiler majboor kar dega ki har state chaaron
    // likhe. Isi wajah se koi state kisi action ko "bhool" nahi sakta — har
    // state har action ka jawab deta hai (galat action pe polite error, crash
    // nahi).

    // R6 — user cash daalta hai
    virtual VendingMachineState *insertCash(VendingMachine *machine, double amount) = 0;

    // R5 — user slot number daal ke product chunta hai
    virtual VendingMachineState *selectSlot(VendingMachine *machine, int slotNumber) = 0;

    // R8, R9, R10 — paisa check karo, product do / change do / refund do
    virtual VendingMachineState *dispense(VendingMachine *machine) = 0;

    // Base requirement #8 — user cancel karke paisa wapas le sakta hai
    virtual VendingMachineState *refund(VendingMachine *machine) = 0;

    // Sirf printing/debugging ke liye — machine abhi kis state me hai
    virtual string getStateName() = 0;

    // ---- Virtual destructor — ZAROORI hai ---------------------------------
    // VendingMachine states ko `VendingMachineState*` (base pointer) me rakhti
    // hai, par asal object `NoMoneyInsertedState` hai (derived). Machine ka
    // destructor `delete noMoneyState_` karega — BASE pointer se. Agar ye
    // destructor virtual na hota, to derived ka destructor chalta hi NAHI ->
    // undefined behavior.
    //
    // 📌 RULE: class me ek bhi `virtual` function hai? To destructor bhi
    //    virtual hona chahiye. Bina soche. Har baar.
    virtual ~VendingMachineState() {}
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_STATES_VENDINGMACHINESTATE_H
