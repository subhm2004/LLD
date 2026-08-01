# GPay LLD — Design Patterns & Design Decisions

UPI-style P2P payment app: **register → bank link → send money → QR pay → request money → ledger**.

Yahan patterns se zyada important wo **money-safety problems** hain jo solve kiye gaye —
paisa gayab hona, limit leak, duplicate payment. Interview me asli marks yahi dilate hain.

---

## 📁 Folder structure

```
GPay_LLD/
├── enums/          PaymentRail, TransactionStatus, TransactionType
├── models/         User, BankAccount, Wallet, Beneficiary, Transaction, MoneyRequest
├── utils/          UpiUtils (VPA validate, phone normalize, QR parse)
├── strategies/     IPaymentRailStrategy + BankAccountRail / WalletRail
├── factories/      PaymentRailFactory, TransactionFactory
├── services/       PinAuth, DailyLimit, TransactionLedger, RequestMoney, Transfer
├── core/           GPaySystem  (Facade)
└── main.cpp        demo + fixed-bug proofs
```

---

## 1. Strategy Pattern — `IPaymentRailStrategy`

"Rail" = paisa kis **patri** pe chalega.

```
IPaymentRailStrategy (interface: railType / debit / credit)
   ├── BankAccountRailStrategy  → user ka PRIMARY linked account
   └── WalletRailStrategy       → userId se wallet (1:1)
```

`TransferService` sirf itna jaanta hai:

```cpp
strategy->debit(payer.getUserId(), amount, bankAccounts, wallets);
strategy->credit(payee.getUserId(), amount, bankAccounts, wallets);
```

Usse **zero matlab** ki andar bank hai ya wallet.

**Demo me iska proof:** `main.cpp` me wahi `sendMoney(...)` call hai — bas `PaymentRail::LINKED_BANK`
ki jagah `PaymentRail::GPAY_WALLET`. Paisa wallet se gaya, aur **ek bhi `if` nahi likhna pada**.

**Bina Strategy ke** `TransferService` me ye hota:
```cpp
if (rail == BANK) { ...20 lines... } else if (rail == WALLET) { ...20 lines... }
```
…aur har naye rail pe wahi function bada hota, aur har badlaav purane rails todne ka risk laata.

### `debit` vs `credit` ka jaan-boojh ke rakha farak (WalletRailStrategy)

| | wallet nahi mila |
|---|---|
| `debit` | **throw** — paisa nikaalna hai aur wallet hi nahi? kuch to galat hai |
| `credit` | **bana do** — paisa aa raha hai, record na hone se rokna galat hai |

📌 *"Be strict on the way out, forgiving on the way in."*

---

## 2. Factory Pattern — do alag maqsad se

### (a) `PaymentRailFactory` — object creation ek jagah

```cpp
static std::shared_ptr<IPaymentRailStrategy> create(PaymentRail rail,
    const std::unordered_map<std::string,std::string>& primaryAccountByUser);
```

Poore codebase me `make_shared<BankAccountRailStrategy>` sirf yahin likha hai. Naya rail =
enum me ek value + yahan ek `case`. Baaki sab untouched.

⭐ **`switch` me `default:` nahi hai — jaan-boojh ke.** Nayi enum value add karke case bhoole
to compiler `-Wswitch` warning dega. `default:` likh dete to compiler chup rehta aur bug
**runtime** pe milta.

### (b) `TransactionFactory` — INVARIANT lock karta hai

Caller `status` param deta hi **nahi**. Factory hamesha `TransactionStatus::INITIATED` bharta hai.

**Kyun zaroori?** Warna koi seedha `SUCCESS` wali transaction bana deta — **bina paisa hile**.
Ledger jhooth bolne lagta. Ab `SUCCESS` sirf `markSuccess()` se aata hai, jo `TransferService`
tabhi bulata hai jab debit **aur** credit dono ho chuke hon.

📌 Factory ka wo use jo log miss karte hain: sirf "object banana" nahi, balki **"object hamesha
sahi initial state me bane"**.

---

## 3. Facade Pattern — `GPaySystem`

Andar 5 services + 2 factories + 2 strategies. `main.cpp` ko ek ka bhi naam nahi pata.

**Karta hai:** data ownership, service wiring, identity resolution (UPI/phone → user),
**authorization** (PIN, "request ka payer tu hi hai na?"), id counters.
**Nahi karta:** paisa hilana, limit ka hisaab, PIN compare, account dhundna.

📌 Facade **coordinator** hai, **God object** nahi.

---

## 4. Service Layer + SRP

| Service | Zimmedari | Ye NAHI karta |
|---|---|---|
| `PinAuthService` | PIN hash + verify | user lookup |
| `DailyLimitService` | roz ka kota (record + release) | paisa hilana |
| `TransactionLedgerService` | transactions store + history | balance |
| `RequestMoneyService` | collect requests ka registry | **paisa bilkul nahi hilata** |
| `TransferService` | **paisa hilana + saare rollbacks** | koi apna business rule |

---

## 5. Repository Pattern — `TransactionLedgerService`

Bahar se `save` / `findById` / `listForUser`; andar `unordered_map`. Kal ko real DB laga do —
sirf is class ka andar badlega.

⭐ **FAILED transactions bhi save hoti hain** — jaan-boojh ke:
- user ko dikhna chahiye ki koshish fail hui (warna wo soch me pad ke **dobara** bhej dega)
- support ko debug ka record chahiye
- fraud detection me failed attempts ka pattern sabse zaroori data hota hai

⭐ `listForUser` payer **aur** payee dono match karta hai — ek entry, do logon ki history.

⭐ Sort **pehle**, `resize(limit)` **baad me** — warna "latest N" nahi, random N milte.

---

## 6. Secondary Index Pattern — teen maps

```
users_          : userId -> User      (asli data)
upiToUserId_    : upiId  -> userId    (index)
phoneToUserId_  : phone  -> userId    (index)
```

O(1) resolution ke liye. Aur ye maps **duplicate rokne** ka kaam bhi karte hain
(`upiToUserId_.count(upiId)` = unique constraint).

⚠ Keemat: teeno ko sync me rakhna padta hai. Isi liye saare writes ek hi jagah (`registerUser`).

---

## 7. Two-step Collect flow — `MoneyRequest` ≠ `Transaction`

```
createRequest()   -> MoneyRequest (PENDING)   [paisa NAHI hila]
   ...payer approve karta hai...
fulfillRequest()  -> Transaction  (SUCCESS)   [ab paisa hila] + request SUCCESS
```

Request banate hi Transaction bana dete to ledger me aisi entries bhar jaatin **jinme paisa
hila hi nahi** — statement galat, reconciliation barbaad.

⭐ **Security:** sirf payer fulfill kar sakta hai —
`if (request.getPayerUserId() != payerUserId) throw`. Bina iske koi bhi kisi ki request
"approve" kar ke paisa kheench leta.

⭐ **Double-pay guard:** `markFulfilled` me `if (!isPending()) throw "already processed"` —
status hi lock ka kaam karta hai.

⭐ **Order:** pehle transfer, **phir** `markFulfilled`. Transfer fail hua to request PENDING
rehti hai → payer dobara try kar sakta hai. Ulta karte to "fulfilled" dikhta par paisa jaata hi na.

---

## 8. Encapsulation — balance ka koi setter nahi

`BankAccount` / `Wallet` me `setBalance()` **jaan-boojh ke nahi hai**. Balance badalne ke
sirf 2 darwaze, dono pe pehra:

```cpp
void debit(double amount) {
    if (amount <= 0 || balance_ < amount) throw std::runtime_error("insufficient balance");
    balance_ -= amount;
}
```

⭐ `amount <= 0` bhi check hota hai — warna `debit(-500)` ka matlab "balance **badha** do"
(paisa chhaap liya 😱).

⭐ Ye invariant **model ke andar** hai, service me nahi — kyunki maalik model hai. Service me
rakhte to har naya caller check bhoolne ka risk laata.

---

## 9. Security patterns

| | Kahan | Kya |
|---|---|---|
| **PIN hashing** | `User.pinHash_`, `PinAuthService` | stored PIN kabhi padha nahi jaata, sirf compare hota hai |
| **Masked account** | `BankAccount.maskedNumber_` | poora account number store hi nahi hota ("XX1234") |
| **Input validation** | `UpiUtils` | QR se aaya VPA bhi validate hota hai — QR koi bhi chhaap sakta hai |
| **Authorization** | `fulfillMoneyRequest` | sirf payer approve kar sakta hai |
| **Self-transfer block** | `TransferService` | khud ko paisa nahi bhej sakte |

⚠ **Honestly missing** (scope ke liye): rate limiting / PIN lockout (abhi 10000 try ho sakte hain),
constant-time comparison, asli KDF (`"hash_" + pin` reversible hai). Interview me ye khud bolna.

---

## 🐛 Paanch bugs — mile, confirm hue, fix hue

Har ek pehle **test likh ke reproduce** kiya gaya, phir fix, phir dobara verify.

### 🐛 1. `TransferService` — debit ho gaya, credit fail → **PAISA GAYAB** (sabse bada)

**Pehle:**
```cpp
try {
    strategy->debit(payer, ...);
    strategy->credit(payee, ...);   // ye throw kar sakta hai!
    txn.markSuccess();
} catch (...) { txn.markFailed(); ledger_.save(txn); throw; }
```
Catch sirf txn ko FAILED mark karta tha — **debit wapas nahi hota tha**.

**Kab hota hai:** payee ne bank account link na kiya ho. Registration pe wallet banta hai,
bank account **nahi**. `BankAccountRailStrategy::credit` throw karta hai.

**Confirm:** Alice(5000) → Bob(bank nahi) ₹250. Transfer FAIL, par Alice ka balance **4750**.
₹250 hawa me. 😱

**Fix:** debit aur credit **alag try-blocks** me. Credit fail hone pe **compensating transaction** —
payer ko paisa wapas credit.

```cpp
try { strategy->credit(payee, ...); }
catch (...) {
    try { strategy->credit(payer, amount, ...); }  // ⭐ REVERSAL
    catch (...) { /* reconciliation queue ka case */ }
    rollbackGuards(...); txn.markFailed(); ledger_.save(txn); throw;
}
```

**Verify:** `main.cpp` safety check [1] → `Alice before=4670 after=4670 → ✅ paisa WAPAS`

---

### 🐛 2. `DailyLimitService` — failed transfers bhi limit kha jaate the

`recordSpend` payment se **pehle** hota hai (sahi hai — pre-check), par failure pe release
karne ka **koi method hi nahi tha**.

**Confirm:** ₹10 balance wale user ne 21 baar ₹5000 bhejne ki koshish ki — **saare fail**,
ek rupaya nahi gaya. Phir bhi: `"daily UPI limit exceeded"`. Poora ₹1 lakh ka kota khatam,
bina ek paisa bheje. 😱

**Fix:** `releaseSpend()` add kiya (negative se bachne ke liye `std::max(0.0, ...)` clamp ke saath),
aur `TransferService` har failure path pe bulata hai.

**Verify:** safety check [2] → `Bob limit before=99880 after=99880 → ✅ limit WAPAS`

📌 Ye bilkul inventory reservation wala pattern hai: **jab bhi kaam se pehle kuch reserve
karo, uska release bhi likho.**

---

### 🐛 3. `clientRequestId` fail hone pe "JAL" jaata tha

Id transfer se pehle insert hoti thi aur failure pe hatti nahi thi. Matlab: pehli koshish
fail (balance kam) → user ne paise daale → **same id se retry** → `"duplicate clientRequestId"` 😤
— jabki koi transfer hua hi nahi tha.

**Fix:** failure pe `processedClientIds.erase(...)`. Success ke baad hi id permanently blocked.

**Verify:** safety check [3] → `retry chali: TXN_8 → ✅ clientRequestId jala nahi`

⚠ Note: ye **"reject duplicate"** model hai, true idempotency nahi (usme duplicate pe purana
**result** wapas milta). UPI apps me ye chalta hai kyunki client naya id generate kar leta hai.

---

### 🐛 4. `BankAccountRailStrategy` — **galat account** se paisa katta tha

**Pehle:**
```cpp
for (auto& e : bankAccounts)
    if (e.second.getUserId() == userId) return e.second;   // "jo pehla mile"
```

Do problem:
1. `unordered_map` ka traversal order **non-deterministic** — do accounts wale user ka paisa
   kabhi HDFC se katta, kabhi SBI se. **Har run me alag.**
2. `GPaySystem` alag se `primaryAccountByUser_` maintain karta hai aur `getPrimaryBankBalance()`
   **usi** se balance dikhata hai. Matlab **UI HDFC ka balance dikhata, paisa SBI se katta**.

**Confirm:** 2 accounts (ACC_1 primary ₹1000, ACC_2 ₹1000) wale user se ₹100 bheja →
primary balance 1000 ka **1000 hi raha**.

**Fix:** strategy ab `primaryAccountByUser` map constructor se leti hai — **guess nahi karti**.
Jo account system ne primary bola hai, bilkul wahi.

📌 Saath me dead field `defaultAccountId_` bhi hata diya — constructor me aata tha par
**kabhi use nahi hota** tha (factory `""` bhejta tha). Aisa dead code sabse dhokebaaz hota
hai: padhne wale ko lagta hai account select ho raha hai, jabki ho kuch aur raha hota hai.

---

### 🐛 5. `GPaySystem` — implicit copy/move → dangling references

**Confirm:** ye line **compile ho jaati thi** —
```cpp
GPaySystem g;  GPaySystem h = std::move(g);   // ✅ compile 😱
```

**Problem:** `transferService_` andar `ledger_` aur `limitService_` ke **references** rakhta hai —
dono **sibling members**. Copy/move memberwise hota hai: references **copy** hoti hain, **re-bind nahi**.

- `h.sendMoney(...)` → transaction **`g`** ke ledger me save hota, `h` ki history hamesha khaali
- `g` scope se bahar → `h` ki references **dangling** → use-after-free

**Fix:**
```cpp
GPaySystem(const GPaySystem&)            = delete;
GPaySystem& operator=(const GPaySystem&) = delete;
GPaySystem(GPaySystem&&)                 = delete;
GPaySystem& operator=(GPaySystem&&)      = delete;
```

**Verify:** ab wahi test `error: call to deleted constructor` deta hai.

📌 **RULE:** jis class ke members ek dusre ko reference/point karte hon, wo class na copy
honi chahiye na move.

---

## ⚠ Doosre important design points

### Member **declaration order** critical hai

C++ me members **declaration** ke order me bante hain (init-list ke order me nahi).
`ledger_` aur `limitService_` `transferService_` se **pehle** declare hain — ulta hota to
`transferService_` un members ka reference leta jo abhi bane hi nahi = UB.

### Guards ka order — sasta check pehle

```
self-transfer / amount (free)  →  idempotency (set lookup)  →  daily limit  →  asli paisa
```
Fail hona hai to **jaldi aur sasta** fail ho.

### `virtual ~IPaymentRailStrategy() = default;`

Object `shared_ptr<IPaymentRailStrategy>` se delete hota hai. Destructor virtual na hota to
derived ka destructor chalta hi nahi → leak + UB.

### `normalizePhone` — duplicate users se bachaav

"9876543210", "+91 98765-43210", "091-9876543210" — teeno ek hi banda. Normalize na karte to
`phoneToUserId_` me **teen alag entries** aur ek hi bande ke 3 accounts. 😱

### `static const std::regex`

Regex compile karna mehnga hai. `static` se poore program me **ek baar** banta hai.

---

## 📊 Pattern summary

| # | Pattern / Technique | Kahan | Fayda |
|---|---|---|---|
| 1 | **Strategy** | `IPaymentRailStrategy` (bank / wallet) | naya rail = nayi class, baaki untouched |
| 2 | **Factory** | `PaymentRailFactory` | creation ek jagah, `default:`-less switch |
| 3 | **Factory (invariant)** | `TransactionFactory` | status hamesha INITIATED se shuru |
| 4 | **Facade** | `GPaySystem` | 9 andar wale components ka ek darwaza |
| 5 | **Service Layer / SRP** | 5 services | ek class = ek kaam |
| 6 | **Repository** | `TransactionLedgerService` | storage swap karna aasan |
| 7 | **Secondary Index** | upi/phone → userId maps | O(1) lookup + unique constraint |
| 8 | **Compensating Transaction** | `TransferService` reversal | paisa kabhi gayab nahi hota |
| 9 | **Reserve / Release** | `DailyLimitService` | failed transfer limit nahi khaata |
| 10 | **State-machine guard** | `MoneyRequest::isPending()` | ek request = ek payment |
| 11 | **Encapsulation** | `BankAccount` / `Wallet` | balance ka koi setter nahi |

---

## 🎯 Extend karna ho to

| Feature | Kya badlega |
|---|---|
| Credit-line / RuPay rail | `PaymentRail` me value + nayi Strategy + factory me case |
| True idempotency | `clientRequestId → txnId` map, duplicate pe **purana txn** return |
| Daily limit reset | `spentToday_` me date rakho, ya Redis TTL key |
| Multiple primary accounts | `setPrimaryAccount(userId, accountId)` — map pehle se hai |
| Real DB | `TransactionLedgerService` ke andar — callers untouched |
| Concurrency | per-account lock, ya DB atomic decrement |
| Double-entry ledger | har transfer pe DO entries (debit + credit) — reconciliation ke liye |

---

## ✅ Verification

- `compile.sh` → **zero warnings** (`-std=c++17 -Wall -Wextra -pthread`)
- Demo ke pehle 6 hisso ka output baseline se **identical** (koi behavior regression nahi)
- Teen safety checks `main.cpp` me **live pass** hote hain
- Copy/move → `error: call to deleted constructor` ✅
- **ASan + UBSan** → clean
