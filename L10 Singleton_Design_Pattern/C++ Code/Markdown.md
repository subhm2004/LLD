# 🛡️ Singleton Design Pattern: The "One & Only" Object

Singleton pattern ek creational design pattern hai jo ye ensure karta hai ki poore application lifecycle mein ek class ka sirf **ek hi instance** (object) ho. Ye pattern ek global point of access provide karta hai.

---

## 1. Problem Statement (Dikkat kya hai?)

Jab hum normal classes use karte hain aur direct `new` keyword ka use karte hain:
- **Resource Wastage:** Har baar object banane se memory aur expensive resources (jaise Database connections) waste hote hain.
- **Data Inconsistency:** Alag-alag objects alag-alag state hold karenge, toh poore app mein synchronization nahi rahega.
- **Conflict:** Multiple objects ek hi shared resource (jaise File System) ko access karke crash ya data corruption kar sakte hain.

---

## 2. Implementation Variants: Evolution

### A. NoSingleton (The Anti-Pattern)
Ismein constructor `public` hota hai, isliye client jitne chahe objects bana sakta hai.
- **Logic:** `(s1 == s2)` evaluates to **0 (False)**.

### B. Basic Singleton (Lazy Initialization)
Constructor ko `private` karke hum instantiation ko control karte hain.
- **Workflow:** Object sirf tab banta hai jab pehli baar `getInstance()` call ho.

### C. Thread-Safe Singleton (Mutex Locking)
Multi-threaded environment mein race condition se bachne ke liye `std::mutex` ka use hota hai.

### D. Double-Checked Locking (Optimized) ⚡
Ye production-ready code hai jo locking ke overhead ko kam karta hai.
- **Workflow:** Pehle bina lock ke check karo -> Agar `nullptr` hai, tabhi lock lagao -> Lock ke baad phir se check karo.

---

## 3. Deep Technical Analysis 🛠️

### 🧬 The Three Pillars of Singleton
1.  **Private Constructor:** Bahar se `new` keyword ko block karne ke liye.
2.  **Static Pointer:** Memory mein ek hi jagah object ka reference store karne ke liye.
3.  **Static Access Method (`getInstance`):** Unique instance provide karne ke liye.



---

## 4. ⚖️ Comparison Table: Eager vs Lazy Initialization

| Feature | Eager Initialization | Lazy (Double-Checked) |
| :--- | :--- | :--- |
| **Object Creation** | Program Startup (Main se pehle) | Jab pehli baar call ho |
| **Thread Safety** | ✅ Automatic (By OS/Compiler) | ✅ Explicit (Via Mutex) |
| **Performance** | 🚀 High (No runtime locks) | ⚡ High (Optimized locks) |
| **Memory Usage** | Hamesha occupy rahegi | Sirf zarurat padne par |

---

## 5.Summary: Kab Kaunsa Use Karein? 🚀

**1.** **Eager Initialization:** Tab use karein jab object chota ho aur aapko pata ho ki wo 100% use hone wala hai.

**2.** **Lazy Initialization(Double-Checked Locking):** Sabse best hai heavy objects (e.g., Database Pool) ke liye multi-threaded apps mein.

**3.** **Meyers' Singleton(C++11+)**: Sabse modern aur chota tarika jo implicit thread-safe hota hai.