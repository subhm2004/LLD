# LLD Anti-Patterns — Detailed Theory Guide

Is document me Low Level Design ke 9 common anti-patterns cover kiye gaye hain. Har pattern ke liye theory, problem, real-world impact, aur solution approach diya gaya hai.

---

## 1. God Object

### Theory

God Object ek aisi class hoti hai jisme itni saari responsibilities daal di jaati hain ki wo class "sab kuch janti hai, sab kuch karti hai" — bilkul ek "God" jaisa sab kaam khud hi karne lagta hai. Ye anti-pattern **Single Responsibility Principle** (SOLID ka "S") ko violate karta hai. Ye principle kehta hai: _"Ek class ka sirf ek hi reason hona chahiye change hone ka."_

### Problem

- Class bahut bada aur complex ho jata hai
- Testing mushkil ho jati hai kyunki sab kuch aapas me juda hota hai
- Ek chhota sa change karne pe pura system tootne ka risk badh jata hai
- Team collaboration mushkil hota hai, sab ek hi class me kaam karenge
- Naye developer ke liye class ko samajhna aur onboard hona time-consuming ho jata hai

### Solution Approach

Har responsibility ko alag-alag class me todo:

- `PaymentProcessor` — sirf payment se related kaam
- `InvoiceGenerator` — sirf invoice banana
- `EmailService` — sirf email bhejna
- `DatabaseManager` — sirf DB operations
- `AuthenticationService` — sirf authentication

Isse har class chhoti, focused, aur independently testable ban jaati hai.

---

## 2. Spaghetti Code

### Theory

Jab classes ek dusre pe itna zyada depend karne lagti hain ki unka flow samajhna mushkil ho jata hai — bilkul spaghetti noodles jaisa ulja hua — to ise Spaghetti Code kehte hain. Classic example hota hai **Circular Dependency**: `A -> B -> C -> A`, jahan A, B ko call karti hai, B, C ko call karti hai, aur C wapas A ko call kar deti hai — ek loop jaisa structure ban jata hai.

### Problem

- Code ka flow trace karna mushkil hota hai
- Compile-time issues aa sakte hain (forward declarations ki zaroorat)
- Har class dusri class se tightly coupled ho jati hai
- Ek class change karo to baaki sab pe asar padta hai
- Debugging aur bug-fixing exponentially mushkil ho jaati hai

### Solution Approach

- **Dependency Inversion Principle** use karo (interfaces/abstract classes ke through communicate karo, concrete classes ke through nahi)
- **Mediator Pattern** ya event-driven architecture use karo taaki classes seedhe ek dusre ko na bulayein, balki ek "mediator" ke through communicate karein
- Circular references ko todne ke liye layered architecture apnao (jaise Controller -> Service -> Repository, ek direction me hi dependency flow ho)

---

## 3. Hard Coding Things

### Theory

Hard coding matlab values (jaise strings, numbers, config settings, API keys) ko seedha code ke andar likh dena, instead of unhe configurable (jaise config file, database, environment variable) banane ke.

### Problem

- Value change karni ho to code recompile karna padega
- Different environments (dev/test/prod) ke liye alag builds banane padte hain
- Reusability kam ho jati hai
- Security issue bhi ho sakta hai (jaise API keys ya passwords hardcode karna, jo version control me leak ho sakte hain)
- Same value multiple jagah hardcode hone se inconsistency ka risk

### Solution Approach

- Config files (JSON/YAML) use karo
- Environment variables use karo
- Constants ko ek centralized config class/file me rakho
- Dependency Injection use karo taaki values bahar se pass ho, class ke andar hardcode na ho
- Sensitive data (API keys, secrets) ke liye secret management tools use karo

---

## 4. Gold Plating / Over Engineering

### Theory

Gold Plating tab hota hai jab hum ek simple problem ke liye zaroorat se zyada complex solution bana dete hain — jaise ek chhoti si problem ke liye design patterns, interfaces, aur abstractions ka pura architecture khada kar dena, jabki ek simple function se hi kaam chal jaata.

### Problem

- Code samajhna mushkil ho jata hai (unnecessary complexity)
- Development time waste hota hai
- Maintenance overhead badh jata hai
- **YAGNI Principle** (_You Aren't Gonna Need It_) violate hota hai
- Future flexibility ke naam pe present ki simplicity qurban ho jaati hai

### Solution Approach

- Simple problems ke liye simple solutions rakho
- Pattern tabhi apply karo jab genuinely uski zaroorat ho (jaise future me multiple implementations chahiye ho, ye assumption data/requirement se confirm ho, sirf guess se nahi)
- **KISS Principle** (_Keep It Simple, Stupid_) follow karo
- Refactoring baad me bhi ho sakti hai — jab genuinely complexity ki zaroorat pade, tab pattern introduce karo

---

## 5. DRY (Do Not Repeat Yourself) Violation

### Theory

DRY principle kehta hai: _"Har piece of knowledge ka system me sirf ek hi, unambiguous, authoritative representation hona chahiye."_ Jab hum same logic ko different jagah copy-paste karte hain, to DRY violate hota hai.

### Problem

- Ek jagah bug fix karo, dusri jagah bhool jate ho (inconsistency)
- Code duplicate hone se maintainability kam ho jati hai
- Same logic change karni ho to har jagah dhundh ke change karna padta hai
- Code review aur testing effort badh jata hai kyunki same logic multiple jagah verify karni padti hai

### Solution Approach

- Common logic ko ek shared/helper function me nikaal do
- Ya ek common base class/utility class bana do jisse multiple functions/classes inherit/use karein
- Repeated patterns dekhte hi refactor karne ki habit banao (Rule of Three: agar teesri baar same logic likh rahe ho, to extract karo)

---

## 6. Constructor Overloading / Telescoping Constructor

### Theory

Jab ek class me bahut saare constructors bana diye jate hain — har ek me alag number of parameters — to ise **Telescoping Constructor Anti-Pattern** kehte hain. Jaise-jaise fields badhti hain, constructors ki list bhi badhti jaati hai.

### Problem

- Confusing hota hai ki kaunsa constructor kab use karna hai
- Naye fields add karne pe naya constructor banana padta hai
- Optional parameters handle karna mushkil ho jata hai
- Code readability kharab ho jati hai, especially jab constructor me bahut saare same-type parameters ho (jaise multiple strings/ints, order galat hone ka risk)

### Solution Approach

- **Builder Pattern** use karo, especially jab bahut saari optional fields hon — isse object creation step-by-step, readable, aur chainable ho jaata hai
- Default parameters use karo (jahan language support kare)
- Named parameters ya parameter objects use karo agar language support kare

---

## 7. Overuse of Getter/Setters

### Theory

Encapsulation ka poora point hota hai data ko protect karna aur sirf controlled tarike se access dena. Lekin agar hum har private field ke liye bina soche-samjhe getter/setter bana dete hain (bina kisi validation ke), to encapsulation ka fayda hi khatam ho jata hai kyunki field practically public jaisa ho jata hai.

### Problem

- Data integrity maintain nahi hoti (koi bhi kuch bhi value set kar sakta hai, jaise negative balance!)
- Business rules bypass ho jate hain
- Object apne aap ki state protect nahi kar pata
- Class ka internal representation externally expose ho jata hai, jo future refactoring ko mushkil banata hai

### Solution Approach

- Setter me proper validation logic daalo
- Jahan zaroorat na ho, wahan setter hi mat banao (read-only rakho)
- **Behavior-driven methods** banao (jaise `deposit()`, `withdraw()`) instead of raw `setBalance()` — isse object apni state khud manage karta hai, sirf data-holder nahi rehta

---

## 8. Premature Optimization

### Theory

_"Premature optimization is the root of all evil"_ — Donald Knuth ka famous quote hai. Iska matlab hai zaroorat se pehle hi code ko complex, "fast" banane ki koshish karna, jabki abhi uski zaroorat hi nahi thi.

### Problem

- Code unnecessarily complex ho jata hai
- Readability aur maintainability kam ho jati hai
- Actual performance bottleneck kahin aur hota hai, lekin humne wahan optimize kiya jahan zaroorat nahi thi
- Development time waste hota hai jo actual features banane me lagna chahiye tha

### Solution Approach

- Pehle simple, correct code likho
- **Profile karo** (measure karo) ki actual bottleneck kahan hai — assumptions pe optimize mat karo
- Sirf tabhi optimize karo jab data se pata chale ki genuinely zaroorat hai
- Built-in, well-tested library functions (jaise `std::sort`) trust karo jab tak custom implementation ki proven zaroorat na ho

---

## 9. Overuse of Inheritance

### Theory

Inheritance ka use tabhi karna chahiye jab genuinely **"IS-A"** relationship ho (jaise Duck IS-A Bird). Agar relationship **"HAS-A"** (Composition) type ki hai, lekin humne zabardasti Inheritance laga di, to isse design galat ho jata hai. Iska classic violation hai **Liskov Substitution Principle** (SOLID ka "L") — jo kehta hai ki child class, parent class ki jagah seamlessly use ho sakni chahiye, bina kisi unexpected behavior ke.

### Problem

- Child class parent ke kuch methods ko meaningfully implement nahi kar pati (jaise `ToyDuck` ke paas `fly()` nahi hona chahiye, lekin usse inherit karne ki wajah se milta hai)
- Exception throw karna padta hai jo bad design ka signal hai
- Code fragile ho jata hai — agar kisi ne Bird type expect kiya aur usko ToyDuck mil gaya, to crash ho sakta hai
- Deep inheritance hierarchies debug karna aur maintain karna mushkil ho jata hai

### Solution Approach

- **Composition over Inheritance** principle follow karo
- Interfaces ko chhote-chhote, specific banao (**Interface Segregation Principle** — SOLID ka "I"), jaise separate `Flyable` interface banao jo sirf flying birds implement karein
- Har class sirf wahi capabilities implement kare jo wo genuinely support karti hai

---

## Summary Table

| #   | Anti-Pattern            | SOLID/Design Principle Violated | Core Fix                                       |
| --- | ----------------------- | ------------------------------- | ---------------------------------------------- |
| 1   | God Object              | Single Responsibility Principle | Responsibilities ko alag classes me split karo |
| 2   | Spaghetti Code          | Dependency Inversion Principle  | Mediator Pattern / layered architecture        |
| 3   | Hard Coding             | Configurability                 | Config files / environment variables           |
| 4   | Gold Plating            | YAGNI                           | KISS principle, simple solutions               |
| 5   | DRY Violation           | Don't Repeat Yourself           | Common logic ko helper functions me nikaalo    |
| 6   | Constructor Overloading | Readability                     | Builder Pattern                                |
| 7   | Getter/Setter Overuse   | Encapsulation                   | Validation + behavior-driven methods           |
| 8   | Premature Optimization  | Simplicity first                | Profile before optimizing                      |
| 9   | Inheritance Overuse     | Liskov Substitution Principle   | Composition over Inheritance                   |

---

_In sabhi anti-patterns ko samajhna aur inse bachna, ek acha Low Level Designer banne ka pehla kadam hai. Har pattern ke peeche ka "kyun" samajhna zaroori hai, sirf "kya" yaad rakhna kaafi nahi hai._
