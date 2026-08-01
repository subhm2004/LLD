# Vending Machine LLD — Design Diagrams

> Codebase padh ke banaya. Yeh **State Design Pattern** ka textbook example hai — machine
> ka behavior uske current state pe depend karta hai, aur har action machine ko agle state
> me le jaa sakti hai.

---

## 1. Class Diagram — State pattern structure

```mermaid
classDiagram
    direction TB

    class VendingMachine {
        <<Context>>
        -Inventory inventory_
        -MoneyManager moneyManager_
        -int selectedSlot_
        -VendingMachineState* currentState_
        -VendingMachineState* noMoneyState_
        -VendingMachineState* moneyInsertedState_
        -VendingMachineState* dispenseState_
        +insertCash(amount)
        +selectSlot(slotNumber)
        +dispense()
        +refund()
        +loadItem(role, slot, item, qty)
        +resetTransaction()
    }

    class VendingMachineState {
        <<interface>>
        +insertCash(machine, amount)* VendingMachineState*
        +selectSlot(machine, slot)* VendingMachineState*
        +dispense(machine)* VendingMachineState*
        +refund(machine)* VendingMachineState*
        +getStateName()* string
    }

    class NoMoneyInsertedState
    class MoneyInsertedState
    class DispenseState

    class Inventory {
        -Map slots_
        +addProduct(slot, item, qty)
        +hasStock(slot) bool
        +dispenseFromSlot(slot)
    }
    class Slot {
        -int slotNumber_
        -Item item_
        -int quantity_
        -bool occupied_
        +reduceQuantity()
        +hasStock() bool
    }
    class Item {
        -string name_
        -double price_
        -ItemType type_
    }
    class MoneyManager {
        -double currentBalance_
        +insertMoney(amount)
        +resetBalance()
    }

    VendingMachineState <|.. NoMoneyInsertedState
    VendingMachineState <|.. MoneyInsertedState
    VendingMachineState <|.. DispenseState

    VendingMachine o-- VendingMachineState : currentState_ (points to one)
    VendingMachine *-- NoMoneyInsertedState : owns
    VendingMachine *-- MoneyInsertedState : owns
    VendingMachine *-- DispenseState : owns
    VendingMachine *-- Inventory
    VendingMachine *-- MoneyManager
    Inventory *-- Slot
    Slot *-- Item
    NoMoneyInsertedState ..> VendingMachine : transitions
    MoneyInsertedState ..> VendingMachine : transitions
    DispenseState ..> VendingMachine : transitions
```

> **State pattern ka core:** har state ki 4 methods return type `VendingMachineState*` hai —
> yaani har action **agla state** lauta ti hai. `VendingMachine` bas
> `currentState_ = currentState_->action(this, ...)` karta hai. Machine ko `if (state == X)`
> jaisa koi bada switch likhne ki zaroorat nahi — behavior state object me hi hai.

---

## 2. ⭐⭐ State Transition Diagram — machine ka dil

```mermaid
stateDiagram-v2
    [*] --> NO_MONEY

    NO_MONEY --> MONEY_INSERTED : insertCash()
    NO_MONEY --> NO_MONEY : selectSlot() / dispense() / refund()<br/>(paisa daalo pehle)

    MONEY_INSERTED --> MONEY_INSERTED : insertCash() / selectSlot()<br/>(balance badhao, product chuno)
    MONEY_INSERTED --> DISPENSE : dispense()<br/>[slot chuna + stock hai + paisa poora]
    MONEY_INSERTED --> NO_MONEY : refund()<br/>OR dispense() fail (kam paisa / out of stock)<br/>[resetTransaction + refund]

    DISPENSE --> NO_MONEY : dispense()<br/>[product nikla + change wapas + reset]
    DISPENSE --> DISPENSE : insertCash / selectSlot / refund<br/>(ruko, product nikal raha hai)

    NO_MONEY --> [*]
```

**Har state ka har action pe response:**

| State \ Action | insertCash | selectSlot | dispense | refund |
|---|---|---|---|---|
| **NO_MONEY** | → MONEY_INSERTED | slot yaad, rahe NO_MONEY | ❌ "paisa daalo" | ❌ kuch nahi |
| **MONEY_INSERTED** | balance+, rahe | slot set, rahe | ✅ → DISPENSE / ❌ refund → NO_MONEY | 💵 → NO_MONEY |
| **DISPENSE** | ❌ ruko | ❌ ruko | 📦 nikalo → NO_MONEY | ❌ ab nahi |

---

## 3. Sequence — successful purchase

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant VM as VendingMachine
    participant S1 as NoMoneyState
    participant S2 as MoneyInsertedState
    participant S3 as DispenseState
    participant Inv as Inventory

    User->>VM: selectSlot(2)
    VM->>S1: selectSlot(machine, 2)
    S1-->>VM: (stock check ok) return NoMoneyState

    User->>VM: insertCash(50)
    VM->>S1: insertCash(machine, 50)
    S1-->>VM: return MoneyInsertedState
    Note over VM: currentState_ = MONEY_INSERTED

    User->>VM: dispense()
    VM->>S2: dispense(machine)
    Note over S2: paid >= price? stock hai?
    S2-->>VM: return DispenseState

    User->>VM: dispense()
    VM->>S3: dispense(machine)
    S3->>Inv: dispenseFromSlot(2) (qty--)
    S3->>VM: resetTransaction() + change wapas
    S3-->>VM: return NoMoneyState
    Note over VM: transaction poora, phir se taiyaar
```

---

## 4. ⭐ Admin vs User — role guard

Product bharna/nikaalna sirf ADMIN kar sakta hai (paisa/dispense USER ke liye):

```mermaid
flowchart LR
    A[loadItem / removeItem] --> B{role == ADMIN?}
    B -- haan --> C[Inventory update]
    B -- nahi --> D[throw: sirf ADMIN kar sakta]
```

---

## 5. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **State** | `VendingMachineState` → NoMoney / MoneyInserted / Dispense | behavior state pe depend; naya state add karna aasan, koi bada switch nahi |
| **Facade / Context** | `VendingMachine` | client sirf 4 actions jaanta hai; states andar |
| **Service layer** | `Inventory`, `MoneyManager` | stock aur paisa alag-alag zimmedari |
| **Role guard** | `requireAdmin()` | authorization ek jagah |

> ⭐ **State objects ek baar bante hain** (constructor me `new`, destructor me `delete`),
> aur reuse hote hain — har transition pe naya object nahi banta, bas pointer badalta hai.
> Isi liye states **stateless** hain (saara data `VendingMachine` context me).
> `VendingMachine` ki copy `= delete` hai (raw state pointers own karta hai — double-free se bachaav).
