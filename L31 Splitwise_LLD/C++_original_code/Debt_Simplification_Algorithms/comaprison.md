# Debt Simplification Algorithms — Comparison

Splitwise-jaisi apps me group ke tangled debts ko minimum transactions me
simplify karne ke liye 4 alag-alag approaches use ki ja sakti hain. Neeche
in sabki detailed comparison hai — kab, kyun, aur kaunsa approach use karna
chahiye.

---

## 1. Quick Summary Table

| #   | Algorithm                   | File                         | Optimality                                             | Time Complexity | Space Complexity       |
| --- | --------------------------- | ---------------------------- | ------------------------------------------------------ | --------------- | ---------------------- |
| 1   | Greedy (Sort + Two-Pointer) | `debt_simplifier.cpp`        | Achha Approximation                                    | O(n log n)      | O(n)                   |
| 2   | DFS / Recursive Greedy      | `2_dfs_recursive_greedy.cpp` | Achha Approximation (Same as #1)                       | O(n²)           | O(n) — recursion stack |
| 3   | Cycle Cancellation (Graph)  | `3_cycle_cancellation.cpp`   | Achha Approximation (Circular debts explicitly cancel) | O(V × E)        | O(V + E)               |
|  |

_(n = total users jinka net balance non-zero hai, V/E = graph ke vertices/edges)_

---

## 2. Har Algorithm Ki Detailed Analysis

### 🟢 Algorithm 1: Greedy (Sort + Two-Pointer)

**Kaise kaam karta hai:**
Har user ka net balance nikalo → creditors aur debtors ko alag list me
daalo → dono ko descending sort karo → sabse bade creditor aur sabse
bade debtor ko match karte jao.

**Pros:**

- Sabse FAST approach (O(n log n))
- Implement karna easy hai
- Real-world apps (Splitwise) me use hota hai
- Large groups (100+ log) ke liye bhi practical hai

**Cons:**

- Guaranteed optimal nahi hai (kabhi-kabhi 1-2 extra transactions ban
  sakti hain jo avoid ki ja sakti thi)

**Kab use karo:** Production apps me — jaha speed aur scalability zaroori
hai, aur "achha enough" solution chalega.

---

### 🟡 Algorithm 2: DFS / Recursive Greedy

**Kaise kaam karta hai:**
Same logic jo Algorithm 1 me hai (max creditor - max debtor matching),
lekin sorting ki jagah har round me LINEAR SCAN karke max dhundte hain,
aur RECURSION use karte hain settlement rounds ke liye.

**Pros:**

- Code likhna quick hai (interview settings me popular)
- Sorting ka overhead nahi hai
- Understand karna intuitive hai (recursive thinking)

**Cons:**

- O(n²) complexity — Algorithm 1 se slower for large n
- Recursion stack ka overhead (bahut bade groups me stack overflow ka
  risk bhi ho sakta hai)
- Result quality Algorithm 1 jaisi hi hai (same greedy nature)

**Kab use karo:** Chhote groups (jaise 10-20 log wale trip/hostel
expenses), interview practice, ya jab sorting avoid karna ho.

---

### 🔵 Algorithm 3: Cycle Cancellation (Graph-Based)

**Kaise kaam karta hai:**
Debts ko directed weighted graph banate hain, phir jab tak graph me
cycle exist kare, uska bottleneck (minimum edge weight) nikal ke pure
cycle se subtract karte hain — isse circular debts automatically cancel
ho jate hain.

**Pros:**

- CIRCULAR DEBTS ko explicitly aur cleanly handle karta hai (A→B→C→A
  jaisi situations perfectly cancel ho jati hain)
- Conceptually graph-theory se strongly connected hai — bade financial
  netting systems me is tarah ka approach use hota hai
- Visualizable/explainable — graph diagram se samjhana easy hai

**Cons:**

- Implementation complex hai (DFS cycle detection + backtracking)
- Cycle detection baar-baar chalani padti hai jab tak graph acyclic na
  ho jaye — isliye moderately slow ho sakta hai bade dense graphs pe
- Non-cyclic part ko fully simplify nahi karta (sirf cycles cancel
  karta hai, baaki edges as-is reh sakti hain jab tak wo bhi kisi
  aur cycle ka part na banein)

**Kab use karo:** Jab tumhe pata ho ki data me bahut saari circular
dependencies hain (jaise multi-party trading/settlement systems), ya
jab tumhe explicitly dikhana ho ki "yeh log ek doosre ka hi paisa
ghuma rahe the, actual me kisi ko kuch dena hi nahi tha."

---

## 3. Real-World Example Se Comparison

**Scenario:** 4 log (A, B, C, D) ke beech tangled debts:

```
B owes A: Rs 500
A owes C: Rs 300
C owes D: Rs 200
D owes B: Rs 100
```

**Net Balances:** A = +200, B = −400, C = +100, D = +100

| Algorithm            | Transactions Count | Result                                    |
| -------------------- | ------------------ | ----------------------------------------- |
| Greedy (Sort)        | 3                  | B→A: 200, B→C: 100, B→D: 100              |
| DFS Recursive Greedy | 3                  | B→A: 200, B→C: 100, B→D: 100              |
| Cycle Cancellation   | 3                  | A→C: 200, B→A: 400, C→D: 100              |
| Bitmask DP (Optimal) | 3                  | _(confirms 3 is indeed the true minimum)_ |

Is example me sab algorithms ka answer same (3 transactions) hai — jo
confirm karta hai ki greedy approaches yahan already optimal hain.
**Lekin yeh HAMESHA guarantee nahi hota** — complex/edge cases me
Greedy approaches (1, 2, 3) kabhi-kabhi 1 extra transaction bana sakte
hain jo Bitmask DP (4) avoid kar sakta.

---

## 4. Decision Guide — Kaunsa Algorithm Kab Use Karo?

```
Kya group size 100+ users tak jaa sakta hai?
│
├── HAAN → Algorithm 1 (Greedy Sort) use karo
│           (Production-ready, fast, scalable)
│
└── NAHI → Kya circular debts common hain is use-case me?
           │
           ├── HAAN → Algorithm 3 (Cycle Cancellation) consider karo
           │
           └── NAHI → Kya EXACT minimum transactions chahiye
                       (chahe slow ho)?
                       │
                       ├── HAAN (aur n ≤ 15) → Algorithm 4 (Bitmask DP)
                       │
                       └── NAHI → Algorithm 1 ya 2 (Greedy) — dono
                                  practically same result denge
```

---

## 5. Final Takeaway

- **Production apps (Splitwise jaisi):** Algorithm 1 (Greedy Sort) — best
  balance of speed aur accuracy
- **Learning/Interview prep:** Algorithm 2 (DFS Recursive) — samajhna
  aasan, likhna quick
- **Graph-theory heavy systems:** Algorithm 3 (Cycle Cancellation) —
  circular debts ko elegantly handle karta hai
- **Competitive programming / Exact optimal chahiye:** Algorithm 4
  (Bitmask DP) — guaranteed best answer, but sirf chhote inputs ke liye
