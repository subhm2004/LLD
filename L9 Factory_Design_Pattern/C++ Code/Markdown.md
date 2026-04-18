# 🏗️ Factory Design Pattern Family: Burger Shop System

Is project mein humne Object Creation ki complexity ko handle karne ke liye teen patterns ka implementation dekha hai. Ye patterns client code ko concrete classes se alag (decouple) karte hain.

---

## 1. Problem Statement (Dikkat kya hai?)

Jab hum client side (`main` function) par direct `new` keyword use karte hain:

- **Tight Coupling:** Client ko har ek burger class (`Basic`, `Premium`, `Wheat`) ka pata hona chahiye.
- **OCP Violation:** Naya burger add karne ke liye har jagah code change karna padega.
- **Dependency:** High-level module low-level classes par depend karne lagta hai.

---

## 2. The Three Patterns: Workflow & Logic

### A. Simple Factory (`SimpleFactory.cpp`)

Ye ek helper class hai jo object creation ka logic ek jagah (centralize) kar deti hai.

**Workflow:**

1. **Request:** Client `createBurger("standard")` call karta hai.
2. **Decision:** Factory ke andar ka `if-else` block "standard" string ko check karta hai.
3. **Creation:** Factory `new StandardBurger()` karke uska pointer return kar deti hai.
4. **Execution:** Client sirf `prepare()` call karta hai, use nahi pata object kaise bana.

---

### B. Factory Method (`FactoryMethod.cpp`)

Yahan hum inheritance ka use karte hain taaki subclasses decide karein ki kaunsa object banana hai.

**Workflow:**

1. **Selection:** Client decide karta hai ki use kaunse brand ki factory chahiye (e.g., `SinghBurger`).
2. **Abstraction:** Client sirf interface method `createBurger()` ko jaanta hai.
3. **Dynamic Dispatch:** Runtime par `SinghBurger` ki implementation chalti hai jo normal bun wale burger return karti hai.
4. **Extensibility:** Agar `KingBurger` factory choose ki, toh bina `main()` ka code badle saare burgers "Wheat Bun" wale milne lagenge.

---

### C. Abstract Factory (`AbstractFactory.cpp`)

Ye "Factory of Factories" hai jo related products ki poori family (Burger + Garlic Bread) banati hai.

**Workflow:**

1. **Theme Setup:** Client ek "Theme" factory select karta hai (e.g., `KingBurger` for Wheat Theme).
2. **Coordinated Creation:** - `createBurger()` call karne par `PremiumWheatBurger` milta hai.
   - `createGarlicBread()` call karne par `CheeseWheatGarlicBread` milta hai.
3. **Consistency:** Ye pattern ensure karta hai ki user "Wheat Burger" ke saath "Normal Bread" mix na kare. Poora combo ek hi family ka hota hai.

---

## 3. Deep Technical Analysis 🛠️

### 🧬 Polymorphism & Virtual Functions

Hamaari base class `Burger` ek **Interface** hai:

```cpp
virtual void prepare() = 0; // Pure virtual function
```

# ⚖️ The Ultimate Showdown: Simple vs. Method vs. Abstract Factory

Is section mein hum teeno patterns ko **SOLID Principles** ke lens se dekhenge ki kahan kaunsa principle follow ho raha hai aur kahan break.

---

## 1. Comparison Table: Core Differences

| Feature                        | Simple Factory                                        | Factory Method                                           | Abstract Factory                                     |
| :----------------------------- | :---------------------------------------------------- | :------------------------------------------------------- | :--------------------------------------------------- |
| **Intent**                     | Object creation logic ko encapsulate karna.           | Subclasses ko decide karne dena ki kaunsa object banega. | Related products ki "Family" banana.                 |
| **Relationship**               | Has-A (Client has a Factory).                         | Is-A (Inheritance based).                                | Composition (Factory of Factories).                  |
| **OCP (Open-Closed)**          | ❌ **Break hota hai** (Naya burger = if-else change). | ✅ **Follow hota hai** (Naya burger = Nayi class).       | ✅ **Follow hota hai** (Nayi family = Nayi factory). |
| **DIP (Dependency Inversion)** | Partially Followed.                                   | Strictly Followed.                                       | Strictly Followed.                                   |

---

## 2. SOLID Principles Analysis 🛡️

### A. Open-Closed Principle (OCP)

> _Software entities should be open for extension, but closed for modification._

- **Simple Factory (FAIL):** Agar tumhe `VeggieBurger` add karna hai, toh tumhe `BurgerFactory` ki `createBurger()` method ke andar jaakar ek naya `else if` likhna padega. Tumne existing class ko **modify** kiya, isliye OCP break hua.
- **Factory Method (WIN):** Agar `KingBurger` ko naya burger launch karna hai, toh wo sirf ek nayi class banayega. Purana code (SinghBurger ya Base Factory) bilkul nahi chheda jayega.

### B. Dependency Inversion Principle (DIP)

> _Depend upon abstractions, not concretions._

- **Without Factory:** `Main` depends on `PremiumBurger` (High-level depends on Low-level).
- **With Factory:** `Main` depends on `Burger` (Interface). Factory handles the rest. Saara code abstract interfaces par chalta hai, concrete classes par nahi.

### C. Single Responsibility Principle (SRP)

- **WIN:** Teeno patterns SRP follow karte hain kyunki unhone Object Creation ka bojh `Main` function se hata kar ek dedicated Factory class ko de diya hai.

---

## 3. Workflow Comparison (Code ki Nazar Se)

### Case 1: Simple Factory

**Scenario:** Client wants a burger.

- `Client` -> `SimpleFactory` -> `Burger`
- **Logic:** "Bhai Factory, mujhe 'Basic' wala burger de." (Factory ke andar 10 line ka if-else hai).

### Case 2: Factory Method

**Scenario:** Different brands making burgers.

- `Client` -> `SinghFactory (Derived)` -> `SinghBurger (Product)`
- **Logic:** "Mujhe Singh ki factory wala burger chahiye." (Creation logic distributed hai).

### Case 3: Abstract Factory

**Scenario:** Full meal (Burger + Bread).

- `Client` -> `MealFactory (Interface)` -> `KingFactory (Concrete)` -> `(WheatBurger + WheatBread)`
- **Logic:** "Mujhe Wheat wali poori family chahiye." (Related products linked hain).

---

## 4. Summary: Konsa Kab Use Karein? 🚀

1. **Simple Factory:** - Use karein jab products fix hon aur zyada badalne wale na hon.
   - _Example:_ Ek simple calculator jahan sirf (+, -, \*, /) operations hain.

2. **Factory Method:** - Use karein jab aap chahte ho ki aapka system flexible ho aur log apni khud ki factories add kar sakein.
   - _Example:_ Ek Game Engine jahan user apne custom Monsters add kar sake.

3. **Abstract Factory:** - Use karein jab aapko "Sets" mein kaam karna ho.
   - _Example:_ Ek UI Toolkit jahan "Dark Mode" select karne par Button, Text, aur Background teeno ek saath Dark ho jayein.

---

## 5. Pro-Tip for Interviews 💡

Agar interviewer pooche: **"Abstract Factory aur Factory Method mein sabse bada difference kya hai?"**

> **Jawab:** "Factory Method sirf **ek product** banane par focus karta hai (inheritance ke through), jabki Abstract Factory **multiple related products** (families) banane par focus karta hai (composition ke through)."
