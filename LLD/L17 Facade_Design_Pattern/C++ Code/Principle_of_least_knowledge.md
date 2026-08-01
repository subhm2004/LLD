# Principle of Least Knowledge (Law of Demeter) - Detailed Guide

(talk only to immediate neighbours)

## 1. Vishay Pravesh (Introduction)
Principle of Least Knowledge (PoLK), jise **Law of Demeter (LoD)** bhi kaha jata hai, software design ka ek mahatvapurn siddhant hai. Iska mukhya uddeshya system ke components ke beech **coupling** (dependence) ko kam karna hai. 

Saral shabdon mein: Ek object ko dusre objects ki internal working ya structure ke bare mein kam se kam jankari honi chahiye. Ise aksar "Talk to your friends, not to strangers" ke naam se jana jata hai.

---

## 2. Iske Mukhya Niyam (The Core Rules)
Software engineering mein, agar hamare paas ek function hai jo kisi object ke andar hai, toh wo niyam anusar sirf inhi cheezon se baat kar sakta hai:
1. Wo swayam (The object itself).
2. Wo objects jo us function mein parameters ke roop mein aaye hain.
3. Wo objects jo us function ke andar hi banaye gaye hain.
4. Wo objects jo us class ke mukhya hisse (fields/instance variables) hain.

In niyamon ka palan karne se hum system mein "Method Chaining" se bachte hain.

---

## 3. Principle ka Ullanghan (Violation)
Jab hum ek object ke zariye dusre, phir teesre aur phir chauthe object ki details mangte hain, toh hum is principle ka ullanghan karte hain.

**Udaharan:**
Maaniye aapko ek 'Customer' ke 'Order' ka 'Address' nikal kar uska 'Zip Code' chahiye. Agar aap seedhe Customer se Address aur phir Zip Code mangte hain, toh aapka code 'Address' class ke structure par depend ho jata hai. Agar bhavishya mein Address class badalti hai, toh aapka code toot jayega.

---

## 4. Delegation: Sahi Tarika
Is principle ko apnane ke liye hum **Delegation** ka istemal karte hain. Hum us object se seedhe detail nahi mangte, balki apne "friend" (immediate object) se kehte hain ki wo hame wo kaam karke de de.

Hame Customer se ye nahi kehna chahiye ki "Mujhe apna Address do taaki main Zip Code dekh sakun", balki hame Customer se kehna chahiye "Mujhe apna Zip Code batao". Customer khud andar hi andar apne Address se baat karke hame result de dega.

---

## 5. Principle of Least Knowledge ke Fayde
1. **Maintainability:** Agar system ke kisi ek hisse mein badlav hota hai, toh uska asar pure system par nahi padta.
2. **Looser Coupling:** Objects ek dusre par bahut zyada nirbhar nahi hote, jisse code ko manage karna aasan ho jata hai.
3. **Easier Testing:** Unit testing ke waqt hame bahut saare "fake" ya mock objects nahi banane padte kyunki dependencies kam hoti hain.
4. **Better Encapsulation:** Data chupa rehta hai aur sirf zaroori behavior hi bahar dikhta hai.

---

## 6. Real-World Analogy (Asli Duniya ka Udaharan)
Ek restaurant ka udaharan lete hain:
- Ek **Customer** sirf **Waiter** se baat karta hai.
- Waiter andar jakar **Chef** se baat karta hai.
- Customer ko ye janne ki zaroorat nahi hai ki Chef kitchen mein khana kaise bana raha hai ya kaunse bartan istemal kar raha hai. 
- Agar Customer seedhe kitchen mein jakar Chef ko instruction dene lage, toh ye system design ka ullanghan hoga.

---

## 7. Savdhaniyan (Trade-offs)
Halaanki ye principle bahut accha hai, lekin iska had se zyada istemal karne se system mein bahut saare "wrapper methods" ban jate hain. Ise **Middle Man** code smell kaha jata hai. Isliye balance banaye rakhna zaroori hai.

---

## 8. Summary Comparison

| Pehlu | Principle ka Ullanghan | Principle ka Palan |
| :--- | :--- | :--- |
| **Coupling** | Tight (Zyada nirbharta) | Loose (Kam nirbharta) |
| **Stucture** | Deeply Nested (Train Wreck) | Flat and Direct |
| **Testing** | Mushkil | Aasan |
| **Flexibility** | Kam (Fragile) | Zyada (Robust) |

---