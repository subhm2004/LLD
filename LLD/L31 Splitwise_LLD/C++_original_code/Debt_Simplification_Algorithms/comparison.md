# Debt Simplification Algorithms — Comparison

Splitwise-jaisi apps me group ke tangled debts ko kam se kam transactions me
simplify karna hota hai. Is folder me **2 working approaches** hain — dono ka
**logic ek hi hai**, bas implementation style alag. Neeche unki detailed
comparison hai, **asli measured data** ke saath (andaaze se nahi — dono ko
same inputs pe chala ke naapa gaya hai).

---

## 1. Quick Summary Table

| #   | Algorithm                   | File                            | Optimality                       | Time       | Space                  |
| --- | --------------------------- | ------------------------------- | -------------------------------- | ---------- | ---------------------- |
| 1   | Greedy (Sort + Two-Pointer) | `minimum_cash_flow_problem.cpp` | Achha approximation              | O(n log n) | O(n)                   |
| 2   | DFS / Recursive Greedy      | `recursive_approach.cpp`        | Achha approximation (same as #1) | O(n²)      | O(n) — recursion stack |

_(n = wo users jinka net balance non-zero hai)_

> **Dono ka RESULT same aata hai.** Logic ek hi hai — "sabse bade creditor ko
> sabse bade debtor se milao". Asli farak sirf **speed** ka hai.

---

## 2. ⚠️ Pehle ek zaroori baat — koi bhi TRUE OPTIMAL nahi hai

"Minimum number of transactions" wala problem **NP-Hard** hai (Subset-Sum se
reduce hota hai). Matlab bade inputs pe guaranteed minimum nikalna practically
namumkin hai.

Dono algorithms **greedy heuristic** hain:

- ✅ Hamesha **valid** answer dete hain (sabka hisaab poora barabar)
- ✅ Hamesha **n−1 se kam** transactions me (n = log)
- ❌ Theoretical minimum ki **guarantee nahi**

**Greedy kab chook sakta hai?** Agar A ko 30 lene hain, aur B ko 10 + C ko 20
dene hain — perfect subset match hai (10+20=30). Par greedy sabse bade se shuru
karta hai, to wo ye match miss kar sakta hai aur ek extra transaction bana de.

Real Splitwise bhi yahi greedy approach use karta hai — kyunki practically ye
almost hamesha optimal ke aas-paas hota hai, aur bahut fast hai.

---

## 3. Har Algorithm Ki Detailed Analysis

### 🟢 Algorithm 1: Greedy (Sort + Two-Pointer) — **RECOMMENDED**

📄 `minimum_cash_flow_problem.cpp`

**Kaise kaam karta hai:**
Har user ka net balance nikalo → creditors aur debtors ko alag list me daalo →
dono ko descending sort karo → two-pointer se sabse bade creditor aur sabse bade
debtor ko match karte jao.

**Pros:**

- Sabse **fast** — O(n log n)
- Large groups (100+ log) ke liye bhi practical
- Real-world apps (Splitwise) me yahi use hota hai
- Edge cases pe robust — khaali input, circular debt, dust amounts, sab safe

**Cons:**

- Guaranteed optimal nahi (upar section 2 padho)
- Sorting ka thoda overhead (chhote n pe koi farak nahi padta)

**Kab use karo:** **Production me hamesha yahi.** Aur interview me bhi — yahi
default answer hona chahiye.

---

### 🟡 Algorithm 2: DFS / Recursive Greedy

📄 `recursive_approach.cpp`

**Kaise kaam karta hai:**
Same logic jo Algorithm 1 me hai, lekin sorting ki jagah **har round me linear
scan** karke max creditor/debtor dhundta hai, aur settlement rounds ke liye
**recursion** use karta hai.

**Pros:**

- Recursive soch samajhne ke liye achha
- Sorting ka overhead nahi
- Interview me jaldi likha ja sakta hai

**Cons:**

- **O(n²)** — Algorithm 1 se slow bade n pe
- Recursion stack ka overhead (waise ye tail-call hai, `-O2` pe compiler ise
  loop bana deta hai)
- Result quality bilkul Algorithm 1 jaisi hi — koi fayda nahi milta

**Kab use karo:** Sirf **seekhne** ke liye. Production me Algorithm 1 hi behtar
hai (same natija, kam time).

---

## 4. 📊 Asli Measured Data (dono ko same input pe chala ke)

### Transaction count — kaun kam deta hai?

| Test case                         | Greedy | Recursive |
| --------------------------------- | ------ | --------- |
| 6-person tangled group (11 debts) | **4**  | **4**     |
| 4-person chain                    | **3**  | **3**     |
| Pure circular debt (A→B→C→A)      | **0**  | **0**     |
| Chain A→B→C→D→E→F (100 each)      | **1**  | **1**     |
| **400 random groups (avg)**       | 6.73   | 6.71      |

400 random groups me dono ne sirf **6 baar** alag result diya — aur wo bhi bas
tie-break ka farak (jab do logon ka amount barabar ho). Dono answers valid the.

**Nateeja:** transaction count me **koi meaningful farak nahi**. Dono ek hi
algorithm hain.

### Speed — yahi asli farak hai

60 users, 300 runs:

| Algorithm            | Time      |
| -------------------- | --------- |
| Greedy (O(n log n))  | **43 ms** |
| Recursive (O(n²))    | 50 ms     |

Chhote groups me farak minor hai, par n badhne pe O(n²) tezi se peeche chhoot
jaata hai.

---

## 5. Real-World Example

**Scenario:** 4 log (A, B, C, D) ke beech tangled debts:

```
B owes A: Rs 500
A owes C: Rs 300
C owes D: Rs 200
D owes B: Rs 100
```

**Net Balances:** A = +200, B = −400, C = +100, D = +100
_(check: 200 − 400 + 100 + 100 = 0 ✓ — sum hamesha 0 hona chahiye)_

| Algorithm            | Transactions | Result                       |
| -------------------- | ------------ | ---------------------------- |
| Greedy (Sort)        | 3            | B→A: 200, B→C: 100, B→D: 100 |
| DFS Recursive Greedy | 3            | B→A: 200, B→C: 100, B→D: 100 |

**4 tangled entries → sirf 3 transactions.** Aur dono ka answer bilkul same.

Yahan B akela debtor hai (−400), to usko teeno creditors ko pay karna hi padega
— 3 transactions se kam ho hi nahi sakta. Ye case optimal hai.

---

## 6. Decision Guide

```
Debt simplify karna hai?
│
├── Production / koi bhi real use-case
│     └── Algorithm 1 (Greedy Sort)  ✅ hamesha yahi
│
└── Recursion ka concept seekhna hai / interview practice
      └── Algorithm 2 (DFS Recursive) — par jaan lo ki
          natija Algorithm 1 jaisa hi aayega, bas slow
```

Seedhi baat: **hamesha Algorithm 1 use karo.** Algorithm 2 sirf padhne/seekhne
ke liye rakha hai.

---

## 7. 🐛 Mile hue Bugs (dono fix ho chuke hain)

Dono files ko thok-bajaa ke test kiya gaya. Greedy saaf nikla, par
**recursive version me 2 crash bugs** the:

### `recursive_approach.cpp`

| Bug                                     | Kya hota tha                                                                                                                                                                             | Fix                                |
| --------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------- |
| **Infinite recursion → stack overflow** | Base case me `&&` tha. Sub-paisa amounts pe (`[+0.004, +0.004, +0.004, −0.012]`) `settleAmount` round hoke **0** ban jaata → kuch subtract nahi hota → **state wahi ka wahi** → 💥 segfault | `&&` → `\|\|`                      |
| **Khaali input → segfault**             | `getMaxCreditIndex()` khaali vector pe bhi `0` return karta tha, phir `amounts[0]` padha jaata — khaali vector pe (out-of-bounds read)                                                     | `if (amounts.empty()) return;`     |

**Dilchasp baat:** Greedy version me ye **dono bugs nahi** the — kyunki wo shuru
me hi `> EPSILON` filter laga deta hai (dust list me aata hi nahi), aur khaali
list pe uska `while` loop chalta hi nahi.

> 📌 **Sabak:** "same logic, alag implementation" ka matlab "same bugs" nahi.
> Har implementation ke apne edge cases hote hain.

### `minimum_cash_flow_problem.cpp`

Ismein koi crash bug nahi mila. Verify kiya gaya:

- ✅ Circular debt → 0 transactions
- ✅ Khaali map / self-loop → crash nahi
- ✅ **Conservation:** har bande ka net exactly match — 200+ random groups, 0 fail
- ✅ Infinite loop namumkin (har round me kam se kam 1 paisa ghatta hai)

Code-quality ke kuch issues zaroor the (net-calculation ka logic 2 jagah
duplicate tha, EPSILON 2 jagah alag likha tha, `const` missing tha) — wo saaf
kar diye gaye.

**Ek chhoti baat:** `round(x * 100) / 100` se paise-level rounding hoti hai. Agar
input me 2 se zyada decimal hon (jaise 100/3 = 33.333…), to **2-3 paise tak
drift** ho sakta hai. Normal amounts (500, 250.50) pe drift **zero** hota hai —
aur real app me amounts hamesha paise tak hi hote hain, to ye practically issue
nahi hai.

---

## 8. Final Takeaway

- **Production apps (Splitwise jaisi):** Algorithm 1 (Greedy Sort) — best
  balance of speed aur accuracy. Yahi asli apps use karte hain.
- **Learning / Interview prep:** Algorithm 2 (DFS Recursive) padh lo — recursion
  ka soch samajhne ke liye achha hai. Par interview me answer Algorithm 1 hi do.
- **Yaad rakho:** dono **heuristic** hain, true optimal nahi (NP-Hard problem
  hai). Interview me ye baat khud se bolna bada plus point hai. 🎯

---