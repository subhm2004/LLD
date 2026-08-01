# Requirements: Vending Machine System

## Functional Requirements

### A. Core (base system)

1. System should support multiple item types.
2. Admin/system should be able to load items with price and quantity.
3. User can select an item.
4. User can insert money in multiple steps.
5. System should validate stock availability before dispensing.
6. System should dispense selected item on sufficient balance.
7. System should return change if paid amount is more than item price.
8. User can cancel transaction and get refund.
9. System should expose current inventory details.

### B. Slots (R1, R5)

10. **R1 — Slots:** vending machine me alag-alag products **alag-alag SLOTS** me
    rakhe hote hain. Har slot ka apna number hota hai (jaise 101, 102, 103).
11. **R5 — Slot se selection:** user **slot number daal ke** product chunta hai
    (jaise "101 daba do" — Coke mil jaayega).

### C. States — STATE PATTERN (R2)

12. **R2 — Machine hamesha teen me se EK state me hoti hai:**
    - **`NoMoneyInsertedState`** — machine me koi paisa nahi hai
    - **`MoneyInsertedState`** — paisa daal diya gaya hai
    - **`DispenseState`** — machine product bahar nikaal rahi hai

    Har state me machine ka **behavior alag** hota hai (jaise `NoMoney` me
    dispense bolo to mana kar degi, par `MoneyInserted` me chalegi).

### D. Users aur Admin (R3, R4)

13. **R3 — Do tarah ke log** system use karte hain:
    - **USER** (aam grahak) — paisa daal ke product khareedta hai
    - **ADMIN** (staff) — machine ka stock sambhalta hai
14. **R4 — Admin ke haq:** admin machine me **naye product daal sakta hai**
    aur **nikaal bhi sakta hai**. Aam user ye nahi kar sakta.

### E. Cash aur Payment (R6–R10)

15. **R6 — Cash:** user machine me cash daal sakta hai.
16. **R7 — Total:** machine jod ke rakhti hai ki ab tak kitna paisa daala gaya.
17. **R8 — Check:** machine dekhti hai ki daala hua paisa chune hue product ke
    liye poora hai ya nahi.
18. **R9 — Zyada paisa:** agar paisa daam se **zyada** hai, to machine **change
    wapas** karegi aur product de degi.
19. **R10 — Kam paisa:** agar paisa daam se **kam** hai, to machine **error
    message** dikhayegi **aur poora paisa wapas** kar degi.

## Non-Functional Requirements

1. Design should be modular and easy to extend.
2. Payment and inventory modules should be decoupled.
3. Basic validations should exist for invalid operations.
4. In-memory operations should be fast for demo-level scale.
5. Naya state ya naya product type add karna ho to purana code CHHEDNA na pade
   (Open/Closed Principle).

## Assumptions

1. Single user transaction at a time.
2. Payment gateway/card reader integration is out of scope.
3. Hardware-level concerns (motors, sensors) are out of scope.
4. Data persistence is not required.
5. Machine ke paas change dene ke liye hamesha paisa hai (coin-change ka
   problem out of scope — wo apne aap me ek alag algorithm hai).

## Design Notes

- **R2 (States)** ke liye **State Design Pattern** use hua hai — har state apni
  class hai (`NoMoneyInsertedState`, `MoneyInsertedState`, `DispenseState`).
  Isse `VendingMachine` me ek bhi `if (state == ...)` nahi likhna padta, aur
  naya state add karna = nayi class banao, purana code chhuo mat.
  (Pattern ki poori theory: `L32 State_design_pattern/`)

- **R10 ek asli BUG tha** purane code me: paisa kam hone par error to aata tha,
  par **paisa machine me hi atka reh jaata tha** (refund hota hi nahi tha).
  Test karke pakda gaya, ab fix hai.

- **R3/R4 (roles)** simple rakhe gaye hain — koi login/password nahi. Admin ke
  functions alag hain aur unhe `AdminPanel` ke through hi bulaya ja sakta hai.
  Asli system me yahan authentication lagta.
