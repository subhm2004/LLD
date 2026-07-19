# Design Patterns Used — Vending Machine LLD

Vending Machine ka poora design ek hi soch pe khada hai: **"machine ka behavior uske
andar ke STATE se badalna chahiye, aur naya state/product add karna ho to purana code
CHHEDNA na pade."** Iske liye ek main pattern (State), ek structural pattern (Facade),
aur do supporting techniques (stateless shared objects + SRP service split) ka use hua hai.

```
┌────────────────────────────────────────────────────────────────────────────┐
│                          PATTERN OVERVIEW                                    │
├────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern       │  Kahan (class)                  │  Kis requirement ke liye │
├────────────────┼─────────────────────────────────┼─────────────────────────┤
│  State ⭐      │  VendingMachineState + 3 states │  R2 (poora core)         │
│  Facade        │  VendingMachine (context)       │  Client ko simple API    │
│  Stateless     │  3 state objects reuse          │  Memory / design safety  │
│  shared states │  (Flyweight idea)               │                          │
│  SRP / Service │  Inventory, MoneyManager        │  NFR2 decoupling         │
│  split         │                                 │  (R1,R4,R5 / R6,R7)      │
└────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. State Design Pattern ⭐ (system ka dil — Requirement R2)

### 📌 Concept
State pattern kehta hai: jab kisi object ka **internal state** badle, uska **behavior**
bhi badal jaana chahiye — bahar se aisa lage jaise object ki **class hi badal gayi ho**.
Har state ek alag class ban jaati hai, aur "ab kya karna hai" ka faisla us state ke paas
chala jaata hai — context ke paas nahi.

### PATTERN KE ROLES
- **Context** → [`VendingMachine`](./core/VendingMachine.h) — data (inventory, money, selected slot) + current state pointer rakhta hai. Har public call bas current state ko delegate kar deta hai.
- **State interface** → [`VendingMachineState`](./states/VendingMachineState.h) — 4 actions declare karta hai: `insertCash`, `selectSlot`, `dispense`, `refund` (sab pure virtual).
- **Concrete States** → [`ConcreteStates.h`](./states/ConcreteStates.h) me 3 classes (bodies [`StateImpl.h`](./states/StateImpl.h) me):
  - `NoMoneyInsertedState` — machine me koi paisa nahi
  - `MoneyInsertedState` — paisa pada hai (asli business logic yahi — R7, R8, R10)
  - `DispenseState` — product nikal raha hai (R9)

### ⭐ SABSE ZAROORI MECHANISM — har action `VendingMachineState*` **laut**ata hai
```cpp
// VendingMachine.h — har public method bas 1 line
void insertCash(double amount) {
    currentState_ = currentState_->insertCash(this, amount);
    //              └── purana state kaam karta hai ──┘
    //  └── aur jo state wo lauta ke deta hai, WAHI naya current state ─┘
}
```
Matlab **transition ka faisla state khud leta hai**, machine nahi. Machine bilkul "bewakoof"
hai — usko pata hi nahi ki `NoMoney` ke baad `MoneyInserted` aata hai. Wo bas poochti hai
"ab kya?" aur jo state bataye, maan leti hai. "Wahi state me raho" ka matlab bhi simple —
state khud ko wapas kar deta hai (`return machine->getNoMoneyState();`).

### STATE TRANSITION TABLE (poori machine ek nazar me)
| Abhi state \ Action | insertCash | selectSlot | dispense | refund |
|---------------------|-----------|-----------|----------|--------|
| **NO_MONEY**        | → MONEY_INSERTED ✅ | slot yaad, NO_MONEY | ❌ "paisa daalo", NO_MONEY | ❌ NO_MONEY |
| **MONEY_INSERTED**  | balance+=, MONEY_INSERTED (R7) | slot set/change | ✅ paisa poora → DISPENSE (R8) / kam → refund → NO_MONEY (R10) | ✅ refund → NO_MONEY |
| **DISPENSE**        | ❌ "ruko", DISPENSE | ❌ "ruko" | ✅ product + change → NO_MONEY (R9) | ❌ "ruko", DISPENSE |

> **⭐ Notice:** har cell bhara hua hai. Koi bhi galat action kisi bhi state me karo — machine
> **crash nahi hoti**, bas polite error de ke wahin ruk jaati hai. "Invalid action" handling
> apne aap ho gayi, kyunki har state ne har action ka jawab likha hai.

### ⭐ KYUN? (State pattern se kya mila)
> **Bina pattern ke:** `VendingMachine::dispense()` me `if (!itemSelected_) ... if (paid < price) ...`
> jaise checks har function me bikhar jaate. Naya state (`SOLD_OUT`, `MAINTENANCE`) chahiye?
> Har function me jaake branch add karo — ek bhi bhoole to chup-chaap bug. 😵
>
> **State pattern se:** naya state = **nayi class, bas**. Purana code chhuna nahi padta.
> Yahi **Open/Closed Principle** hai (NFR5). Purana code sach me `bool itemSelected_` rakhta
> tha aur uske checks bikhre the — wo State pattern ka ulta tha; refactor karke theek kiya.

---

## 2. Facade Design Pattern (client ko ek simple API)

### 📌 Concept
Facade ek complex subsystem (bahut saari internal classes) ke aage ek **single, patla
interface** rakh deta hai — client ko andar ke moving parts se matlab hi nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`VendingMachine`](./core/VendingMachine.h) (yahi Context bhi hai — dono role ek class me)
- **Subsystem** → `Inventory`, `MoneyManager`, `Slot`, `Item`, aur 3 State classes

### Implementation
System me 8+ classes hain, par [`main.cpp`](./main.cpp) (client) me kitni dikhti hain? Bas
`VendingMachine`, `Item`, aur do enums. **Kisi State class ka naam tak nahi aata.** Client bas:
```cpp
machine.insertCash(50);      // andar: MoneyManager + state transition
machine.selectSlot(103);     // andar: Inventory lookup + stock check
machine.dispense();          // andar: price check, change calc, stock -1, reset
```
Andar poora tamasha (money manage, stock check, state transition) ho jaata hai — client ko
bas natija milta hai.

### ⭐ KYUN? (Why Facade)
> Client aur subsystem **decoupled**. Kal ko `Inventory` slot number ki jagah kuch aur use
> karne lage, ya payment ka tareeka badle — `main.cpp` ka ek line bhi nahi badlega. Sirf
> facade ke andar adjust hoga.

---

## 3. Stateless Shared States (Flyweight ka idea)

### 📌 Concept
State objects ke paas **apna koi data nahi** (stateless hain) — saara data `VendingMachine`
ke paas hai, aur state usse `machine->` se maangta hai. Isi wajah se **3 hi state objects**
bante hain aur poori machine ki zindagi wahi ghoomte rehte hain — har transition pe naya
`new` nahi hota.

```cpp
// VendingMachine constructor — 3 objects, ek hi baar
noMoneyState_(new NoMoneyInsertedState()),
moneyInsertedState_(new MoneyInsertedState()),
dispenseState_(new DispenseState())
// ...phir transitions bas inhi 3 ke beech pointer ghumate hain
```

### ⚠ Ownership ka dhyaan (asli memory-safety yahi hai)
- Destructor teeno states `delete` karta hai — **par `currentState_` ko nahi** (wo inhi teen me se hi kisi ek ko point kar raha hai; do baar delete = crash 💥). 📌 *"Pointer hone ka matlab maalik hona nahi hota."*
- Copy/assignment `= delete` — do machines same state objects share karti to dono ke destructor unhe delete karte → **double free**. (Rule of Three.)

### ⭐ KYUN?
> Stateless design se: (a) memory saaf — N transitions pe bhi sirf 3 objects, (b) state aur
> data kabhi "jhooth" nahi bolte kyunki single source of truth machine hai. Yahi **L30 Flyweight**
> wala idea hai — shared immutable-behavior objects jo reuse hote hain.

---

## 4. SRP / Service Split (decoupled modules — NFR2)

### 📌 Concept
Business logic ko chhoti **single-responsibility** classes me tod do. Har class ka ek hi kaam.

| Class | Ek hi zimmedari | Requirement |
|-------|-----------------|-------------|
| [`Inventory`](./services/Inventory.h) | Slots ka stock — add/remove/dispense/print (paise ka kuch pata nahi) | R1, R4, R5 |
| [`MoneyManager`](./services/MoneyManager.h) | Sirf balance jodna aur reset (daam/change ka pata nahi) | R6, R7 |
| [`Slot`](./models/Slot.h) | Ek khaana — kaunsa item, kitne bache, occupied ya nahi | R1, R5 |
| [`Item`](./models/Item.h) | Immutable value object — naam, daam, type | — |

> **⭐ KYUN?** `MoneyManager` ko ye nahi pata ki paisa kaafi hai ya nahi (wo `MoneyInsertedState`
> decide karta hai) — wo bas jodta/reset karta hai. Isse kal ko payment ka tareeka badle (UPI/card),
> to sirf ek class badlegi. States, Inventory, Slot — kisi ko chhuna nahi padega. Yahi
> **"payment aur inventory decoupled"** (NFR2) hai.

---

## Object Relationships (UML samajhne ke liye)

```
VendingMachine   ◆──  Inventory            (Composition: machine owns karti hai)
VendingMachine   ◆──  MoneyManager         (Composition)
VendingMachine   ◆──  3 State objects      (Composition: new + delete machine karti hai)
VendingMachine   ──▶  currentState_        (Association: point karti hai, own nahi karti)
Inventory        ◆──  Slot  (map<int,Slot>)(Composition)
Slot             ◆──  Item                 (Composition: value member)
ConcreteState    ··▶  VendingMachine       (Dependency: method param `machine`, store nahi karta)
```
- `◆──` **Composition** (strong owns-a: lifetime bandha) · `──▶` **Association** (points-to) · `··▶` **Dependency** (temporary use)

---

## Requirement → Pattern quick map

| Requirement | Pattern / Mechanism |
|-------------|---------------------|
| R1 different products in slots | `Inventory` + `Slot` model |
| R2 three machine states | **State pattern** (3 concrete states) |
| R3 user vs admin | `UserRole` enum + `requireAdmin()` guard |
| R4 admin load/remove | `Inventory::addProduct/removeProduct` |
| R5 pick by slot number | `selectSlot(int)` → state delegate |
| R6 insert cash | `MoneyManager::insertMoney` (via state) |
| R7 add up money | `+=` in MoneyManager |
| R8 check sufficient | `MoneyInsertedState::dispense()` |
| R9 return change | `DispenseState::dispense()` |
| R10 less money → error + refund | `MoneyInsertedState::dispense()` (yahan bug tha, fix hai) |
| Simple client API | **Facade** (`VendingMachine`) |
| No `if(state==)` sprawl | **State** + returning-state transition |

---

## "Ye pattern kyun NAHI use kiya" (interview me poocha jaata hai)

- **Strategy?** Strategy aur State dikhne me same lagte hain (dono interface + concrete classes),
  par **niyat alag hai**: Strategy me *client* algorithm chunta hai aur wo badalta nahi;
  State me *object khud* apna state transition karta hai runtime pe. Yahan machine khud
  NoMoney→MoneyInserted→Dispense me ghoomti hai — isi liye **State** sahi hai, Strategy nahi.
- **Singleton?** Machine ek hi instance ki hai, par usse Singleton banana zaroori nahi —
  `main()` me ek `VendingMachine machine;` kaafi hai. Singleton lagate to testing mushkil
  hoti (global state). Zaroorat na ho to mat lagao.
- **Observer?** Abhi koi "machine ne product diya to X ko batao" wala requirement nahi hai.
  Kal ko digital display / restock-alert chahiye to Observer add ho sakta hai — abhi over-engineering.
```
