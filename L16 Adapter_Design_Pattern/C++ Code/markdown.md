# 🚀 Adapter Design Pattern

Adapter pattern ka asli maksad hai **"Purane (Legacy) code ko bina chede, naye system ke layak banana."**

---

## 🛠 Kab Use Karein? (Use Cases)

Is pattern ka istemal tab hota hai jab:

1. **Incompatible Interfaces:** Aapke paas ek purani class hai jo kaam toh sahi karti hai, lekin uska interface (functions ke naam ya data format) naye system se match nahi karta.
2. **Third-Party Libraries:** Aapne koi library download ki jo XML format mein data deti hai, lekin aapka pura App JSON par chalta hai. Aap library ka code toh badal nahi sakte, toh beech mein Adapter laga dete hain.
3. **Legacy Code Integration:** Jab koi naya component purane "Sarkari" code se baat karna chahta hai aur dono ki bhasha (data types) alag ho.
4. **Data Translation:** Ek format se dusre format mein data convert karte waqt (Jaise: XML to JSON, Celsius to Fahrenheit).

---

## 🌟 Asli Fayde (Benefits)

Is pattern ko use karne ke 3 sabse bade fayde hain:

### 1. Open/Closed Principle (OCP)

Aapko purani class (Adaptee) ke code mein ek line bhi badalne ki zaroorat nahi hai. Aap sirf ek naya Adapter banate hain. Iska fayda ye hai ki purane code mein koi naya bug aane ka khatra nahi rehta.

### 2. Single Responsibility Principle (SRP)

Data conversion (Parsing/Translation) ki saari sar-dardi Adapter class ke paas hoti hai. Client class ko sirf apna kaam karne se matlab hai, use isse farak nahi padta ki piche data XML tha ya JSON.

### 3. Reusability

Aap ek hi purani class ke liye multiple adapters bana sakte hain. Aaj XML to JSON chahiye, kal XML to CSV chahiye ho toh bas ek naya adapter class add karna hoga.

---

## 🧩 Is-A vs Has-A (The Core Logic)

Adapter pattern in do rishton par tika hai:

- **Is-A (Inheritance):** Adapter class naye interface ko inherit karti hai. Isse Client ko lagta hai ki wo naya system hi use kar raha hai.
- **Has-A (Composition):** Adapter ke andar purani class ka object hota hai. Asli kaam wahi purani class karti hai, adapter bas "Packing" badal deta hai.

---

## ⚠️ Nuksan (Drawbacks)

- **Code Complexity:** Ek extra class add ho jati hai, jo kabhi-kabhi chote projects mein overload lag sakti hai.
- **Performance:** Beech mein ek extra layer aane se bahut hi minor (negligible) delay ho sakta hai conversion logic ki wajah se.

---

_Summary: Adapter ek "Plug-Converter" ki tarah hai jo purane device ko naye socket mein chalne ki ijazat deta hai._
