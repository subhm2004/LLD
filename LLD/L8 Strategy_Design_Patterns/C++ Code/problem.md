# Strategy Design Pattern - Robot System Analysis

## 1. Problem (Dikkat kya hai?)

Jab hum normal inheritance use karte hain, toh kuch bade issues aate hain:

- **The "Flying Duck" Problem:** Maan lo ek `Robot` base class hai jisme `fly()` method hai. Ab agar tum `WorkerRobot` banaoge jo zameen par chalta hai, toh inheritance ki wajah se uske paas bhi `fly()` aa jayega jo ki galat hai.
- **Rigid Code:** Agar 10 robots ko ek hi tarah se walk karwana hai, toh logic duplicate hota hai.
- **Inflexibility:** Runtime par robot ka behavior badalna (jaise walking se udne lagna) lagbhag impossible ho jata hai.
- **Code Reuse** ache se nhi ho pata hai isse
- **To add a new feature alot of changes are requied which is not good at all**
- **Open Close Principle Break kr dega ye**

---

## 2. Strategy Design Pattern (Brief Note)

**Strategy Design Pattern Defines a family of algorithms,put them into seprate classes,so that they can be changed at runtime.**

Strategy Pattern ek **Behavioral Design Pattern** hai jo "Composition" par focus karta hai na ki "Inheritance" par.

**Mantra:** _"Identify the parts that vary and separate them from those that stay the same."_

Iska main logic ye hai ki hum behaviors (chalna, udna, bolna) ko ek alag "Family of Algorithms" bana dete hain. Robot class ab in behaviors ko inherit nahi karti, balki unhe **Contain** karti hai (Has-A relationship). Isse hum asani se behaviors ko swap kar sakte hain.

---

## 3. Solution (The Real Deal)

Solution ye hai ki hum har badalne wale behavior ke liye ek **Interface** (Abstract Class) banayein:

1. **Encapsulation:** Walk, Talk, aur Fly ko alag-alag classes mein pack kar diya.
2. **Dynamic Binding:** Base class mein interface ka pointer rakha taaki runtime par decide ho sake ki kaunsa behavior chalana hai.
3. **Open-Closed Principle:** Naya behavior add karne ke liye purana code chhedne ki zaroorat nahi hai.

---

## 4. Detailed Workflow (Step-by-Step) 🚀

Ye pattern kaam kaise karta hai, uska pura rasta yahan hai:

1. **Interface Blueprint:** Sabse pehle `WalkableRobot` jaisa ek interface banta hai jo kehta hai "Mujhe nahi pata walk kaise hogi, bas `walk()` function hona chahiye."
2. **Strategy Plug-ins:** Phir hum `NormalWalk` aur `NoWalk` jaisi concrete classes banate hain jo actual logic likhti hain. Ye plug-ins ki tarah hain.
3. **Robot Setup (Composition):** `Robot` class ke andar hum pointers banate hain (e.g., `WalkableRobot *walkBehavior`). Robot class ko walk karna nahi aata, wo bas pointer hold karti hai.
4. **Behavior Injection:** Jab `main()` mein object banta hai, hum constructor ke through specific behavior (Strategy) pass karte hain.
   - _Example:_ `CompanionRobot` ko humne `NormalWalk` inject kar diya.
5. **Delegation:** Jab user `robot->walk()` call karta hai, toh Robot class apna kaam khud karne ke bajaye us pointer ko "Delegate" (saunp) deti hai: `walkBehavior->walk()`.

---

## 5. Important Tips & Tricks 💡

- **Virtual Destructor is Mandatory:** Agar tumne Base class (`Robot`) ka destructor `virtual` nahi banaya, toh derived class aur behaviors delete nahi honge, jisse **Memory Leak** hoga. Hamesha `virtual ~Robot() {}` likho.
- **Composition > Inheritance:** Hamesha koshish karo ki features ko "Add" (Compose) karo, "Inherit" nahi. Isse code flexible rehta hai.
- **Heap Memory Cleanup:** Agar tum `new` se object bana rahe ho (jaise `new NormalWalk()`), toh destructor mein `delete` karna mat bhoolna.
- **Runtime Swap:** Ek `setter` function (e.g., `setFlyBehavior`) bana kar tum chalti application mein robot ka behavior badal sakte ho. Jaise ki ek robot pehle chal raha tha, par upgrade ke baad udne laga.
- **Smart Pointers:** Agar tum modern C++ (C++11 ya usse upar) use kar rahe ho, toh `std::unique_ptr` use karo, isse manual `delete` ka jhanjhat khatam ho jata hai.

## Important Concept

- Haan, agar tum inheritance (base aur derived classes) use kar rahe ho, toh aankh band karke Base class ke destructor ko virtual bana do. Ye ek standard "Best Practice" hai.

---
