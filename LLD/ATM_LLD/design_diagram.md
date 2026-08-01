# ATM LLD — Design Diagrams

> Yeh file codebase padh ke banayi gayi hai. Mermaid diagrams GitHub pe seedha
> render hote hain (aur VS Code ke mermaid preview me bhi).

---

## 1. Class Diagram — poora structure ek nazar me

```mermaid
classDiagram
    direction LR

    class ATMSystem {
        <<Facade>>
        -Map accounts_
        -Map cards_
        -List~Transaction~ transactions_
        -AuthenticationService authService_
        -CashDispenser cashDispenser_
        -string currentCardNumber_
        -int transactionCounter_
        +addAccount(BankAccount)
        +addCard(Card)
        +login(cardNumber, pin) bool
        +logout()
        +checkBalance() double
        +withdrawCash(amount) NotesMap
        -getCurrentAccountOrThrow() BankAccount*
        -createTransaction(type, amount, success)
    }

    class Card {
        -string cardNumber_
        -string pin_
        -string accountNumber_
        +getCardNumber() string
        +getAccountNumber() string
        +validatePin(enteredPin) bool
    }

    class BankAccount {
        -string accountNumber_
        -double balance_
        +getBalance() double
        +debit(amount) bool
    }

    class Transaction {
        -string transactionId_
        -TransactionType type_
        -double amount_
        -TransactionStatus status_
        -long long timestamp_
        +markSuccess()
        +markFailed()
    }

    class AuthenticationService {
        +authenticate(Card, enteredPin) bool
    }

    class CashDispenser {
        -Map notes_ denom_to_count_desc
        +canDispense(amount) bool
        +dispense(amount) NotesMap
        -computePlan(denoms, idx, remaining, plan)$ bool
    }

    class TransactionType {
        <<enumeration>>
        BALANCE_INQUIRY
        CASH_WITHDRAWAL
    }

    class TransactionStatus {
        <<enumeration>>
        INITIATED
        SUCCESS
        FAILED
    }

    ATMSystem "1" *-- "many" BankAccount : owns (by accountNumber)
    ATMSystem "1" *-- "many" Card : owns (by cardNumber)
    ATMSystem "1" *-- "many" Transaction : audit log
    ATMSystem "1" *-- "1" AuthenticationService : has
    ATMSystem "1" *-- "1" CashDispenser : has
    AuthenticationService ..> Card : validates PIN
    Card ..> BankAccount : linked by accountNumber
    Transaction --> TransactionType : has
    Transaction --> TransactionStatus : has
```

**Padhne ka tareeka:** `*--` = composition (ATMSystem in sab ka maalik hai, uske
marte hi sab khatam). `..>` = dependency/uses. `Card` sirf `accountNumber` string
rakhta hai — asli `BankAccount` object nahi — isliye wo association (by id) hai.

---

## 2. Sequence Diagram — Cash Withdrawal ka poora flow

Yeh diagram sabse important hai: dhyan do ki **paisa debit hone se PEHLE teen guard
checks** hote hain, aur har raaste pe ek Transaction record banta hai (fail bhi ho to).

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant ATM as ATMSystem
    participant Auth as AuthenticationService
    participant Acc as BankAccount
    participant Disp as CashDispenser

    User->>ATM: login(cardNumber, pin)
    ATM->>Auth: authenticate(card, pin)
    Auth->>Auth: card.validatePin(pin)
    Auth-->>ATM: true/false
    alt PIN galat
        ATM-->>User: login fail
    else PIN sahi
        ATM->>ATM: currentCardNumber_ = cardNumber
        ATM-->>User: login success
    end

    User->>ATM: withdrawCash(amount)
    Note over ATM: Guard 1 — amount > 0?
    Note over ATM: Guard 2 — balance >= amount?
    ATM->>Disp: canDispense(amount)
    Note over Disp: Guard 3 — notes ka<br/>sahi combination hai?
    Disp-->>ATM: true/false

    alt koi bhi guard fail
        ATM->>ATM: createTransaction(WITHDRAWAL, amount, FAILED)
        ATM-->>User: throw runtime_error
    else sab pass
        ATM->>Acc: debit(amount)
        ATM->>Disp: dispense(amount)
        Disp-->>ATM: notes map (denom -> count)
        ATM->>ATM: createTransaction(WITHDRAWAL, amount, SUCCESS)
        ATM-->>User: notes de do
    end
```

---

## 3. Cash Dispenser — Denomination algorithm (backtracking)

`CashDispenser` sabse interesting logic hai. Simple greedy (bade note pehle) hamesha
kaam nahi karta, isliye ye **backtracking** karta hai:

```mermaid
flowchart TD
    A[computePlan denoms, idx, remaining] --> B{remaining == 0?}
    B -- haan --> C[return true — plan mil gaya]
    B -- nahi --> D{saare denominations<br/>khatam idx == size?}
    D -- haan --> E[return false — is raaste se nahi bana]
    D -- nahi --> F[maxUse = min remaining/denom, available]
    F --> G[use = maxUse se 0 tak try karo]
    G --> H[plan me use notes daalo]
    H --> I[recurse: agla denom, remaining - use*denom]
    I -- mila --> C
    I -- nahi mila --> J[use ghatao, dobara try]
    J --> G
```

> **Kyun greedy nahi?** Maan lo notes = {500, 300} aur amount = 600. Greedy 500 lega,
> phir 100 baaki — 300 se nahi banta → fail. Backtracking 500 chhod ke 300+300 try
> karta hai → ban gaya. Isliye `use` ko `maxUse` se `0` tak ghata ke har combination
> dekha jaata hai.

---

## 4. Design patterns is LLD me

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `ATMSystem` | client ko ek hi darwaza — andar ke services chhupe |
| **Service layer / SRP** | `AuthenticationService`, `CashDispenser` | auth aur cash-logic alag, ek kaam ek class |
| **State (implicit)** | `currentCardNumber_` | login/logout se session state; khaali = koi session nahi |
| **Audit log** | `transactions_` vector | har action (success/fail) ka record |

**Note:** yeh L22 wale `ATM_Cash_Dispenser` (Chain of Responsibility) se alag hai —
wahan denomination chain-of-responsibility se dispense hoti hai, yahan backtracking se.
