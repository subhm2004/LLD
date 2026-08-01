# 🧩 Low Level Design (LLD) — Complete Interview Guide

> Ye guide web research + is repo ke **actual C++ code** ko jod ke banayi hai. Har concept ke
> saath repo ka folder diya hai — theory padho, phir wahi cheez asli code me dekho.
> Language: **Hinglish** (jaise poore repo me comments hain). Iska bhai `HLD_Interview.md` bhi padho.

---

## 📑 Table of Contents

1. [LLD kya hai + interview me kya dekha jaata hai](#s1)
2. [LLD crack karne ka 7-step framework](#s2)
3. [OOP fundamentals (deep dive + code)](#s3)
4. [SOLID principles (bad vs good code)](#s4)
5. [UML diagrams (class/sequence/state)](#s5)
6. [Design Patterns — Creational](#s6)
7. [Design Patterns — Structural](#s7)
8. [Design Patterns — Behavioral](#s8)
9. [Pattern comparisons (interview favourites)](#s9)
10. [C++ specific deep dive](#s10)
11. [Concurrency in LLD](#s11)
12. [Classic LLD problems (detailed approach)](#s12)
13. [Rapid-fire Q&A (100+)](#s13)
14. [Do's & Don'ts](#s14)
15. [Is repo ka index (kaunsa concept kahan)](#s15)
16. [Complete worked example — Parking Lot (full code)](#s16)
16b. Complete worked example — Splitwise (full code)
17. Common anti-patterns
18. Quick revision cheat sheet
19. Glossary
20. Mock interview transcript

---

<a name="s1"></a>
## 1. LLD kya hai + interview me kya dekha jaata hai

**Low Level Design (LLD)** = ek system ke **classes, objects, unke relationships, aur
interactions** ko design karna. HLD "system ke bade components" decide karta hai (DB, cache,
services), LLD "ek component ke andar ka code kaise organize hoga" decide karta hai.

### LLD vs HLD — ek line me
| | LLD | HLD |
|---|---|---|
| Scope | ek service/module ke andar | poora system, sab services |
| Output | class diagram, code | architecture diagram, components |
| Focus | OOP, design patterns, SOLID | scalability, DB, caching, network |
| Example sawaal | "Design a parking lot" | "Design Instagram" |
| Language | code likhna padta (C++/Java) | boxes + arrows |

### Interviewer kya dekhta hai (scoring criteria)?
1. **Requirements gathering** — kya tu sawaal poochta hai ya andaaza laga ke code likhta hai?
2. **Abstraction** — sahi classes/interfaces nikaal paaya?
3. **Extensibility** — naya feature add karna kitna aasan hai?
4. **Design patterns** — sahi jagah sahi pattern (thoop ke nahi)?
5. **SOLID adherence** — SRP, OCP, etc. follow kiya?
6. **Code quality** — clean, readable, naming, error handling?
7. **Trade-offs** — apne design ke pros/cons khud bol paaya?
8. **Edge cases + concurrency** — thread-safety, null, invalid input?

> ⭐ **Sabse bada differentiator:** requirements clarify karna. Jo candidate turant code
> likhna shuru karta hai wo fail hota hai. Jo pehle 5 min sawaal poochta hai wo pass hota hai.

### LLD interview ka structure (typical 45-60 min)
```
0-10 min   : Requirements clarify + scope define
10-20 min  : Core entities + relationships (class diagram)
20-40 min  : Code likhna (interfaces pehle, phir concrete)
40-50 min  : Ek flow end-to-end walk-through
50-60 min  : Extensions, trade-offs, concurrency discussion
```

---

<a name="s2"></a>
## 2. LLD crack karne ka 7-step framework

Ye framework har LLD problem pe lagta hai. Ratt lo:

### STEP 1 — Requirements clarify karo (functional + non-functional)
Kabhi bhi turant code mat likho. Pehle poocho:

**Functional requirements (system kya karega):**
- "Parking lot me kaunse vehicle types? (car, bike, truck)"
- "Payment kaise hoga? (cash, card, UPI)"
- "Multiple floors/gates?"

**Non-functional requirements (kaise karega):**
- "Kitne concurrent users? (concurrency chahiye?)"
- "Latency important hai?"
- "System kitna scale karega?"

**Scope pin karo (YAGNI):**
- "Ye feature abhi chahiye ya future me?"
- Interviewer se confirm karo kya-kya IN scope hai, kya OUT.

### STEP 2 — Core entities nikaalo (noun extraction)
Problem statement me se **nouns** dhundo — wahi tumhari classes hain.
> "A **parking lot** has **spots**. A **vehicle** enters through a **gate**, gets a **ticket**."
> → `ParkingLot`, `ParkingSpot`, `Vehicle`, `Gate`, `Ticket`

Har entity ki **ek responsibility** honi chahiye (SRP).

### STEP 3 — Attributes + methods define karo (verb extraction)
- **Nouns** → attributes/classes
- **Verbs** → methods
> "vehicle **parks**, user **pays**, system **generates** ticket"
> → `park()`, `pay()`, `generateTicket()`

### STEP 4 — Relationships define karo
- **is-a** (inheritance): `Car is-a Vehicle`
- **has-a** (composition/aggregation): `ParkingLot has-a ParkingSpot[]`
- **uses** (dependency): `CheckoutService uses PaymentStrategy`
- Ownership pin karo: kaun kiska maalik hai? (destructor kiske paas?)

### STEP 5 — Design patterns apply karo (jahan zaroorat ho)
Trigger phrases → pattern:
| Signal | Pattern |
|---|---|
| "kuch algorithm swap hoga" (pricing, ranking) | **Strategy** |
| "object creation complex/conditional" | **Factory** |
| "ek badla to doosron ko batao" | **Observer** |
| "runtime pe behavior add" | **Decorator** |
| "behavior state pe depend" | **State** |
| "ek hi instance chahiye" | **Singleton** |
| "request ko handlers ki chain me" | **Chain of Responsibility** |
| "subsystem ka ek darwaza" | **Facade** |

### STEP 6 — Code likho (top-down)
1. Enums pehle (types define)
2. Interfaces (abstractions)
3. Models (data classes)
4. Services (business logic)
5. Facade (sab jodne wala)
6. main/demo (ek flow)

### STEP 7 — Walk-through + extensions
- Ek complete flow dikhao (book ticket → pay → confirm)
- "Naya feature X aaya to kya badlega?" khud bolo
- Concurrency/edge-cases discuss karo

> ⭐ **YAGNI + DRY + KISS:** jo abhi nahi chahiye mat banao (YAGNI), duplicate logic ek jagah
> (DRY), simple rakho over-engineer mat karo (KISS).

---

<a name="s3"></a>
## 3. OOP Fundamentals (deep dive + code)

### 3.1 — 4 Pillars of OOP

#### 🟢 Abstraction
**Complexity chhupao, sirf zaroori dikhao.** "Car chalane ke liye steering pata ho, engine ka
internal nahi."

```cpp
// Abstraction: bahar wale ko sirf pay() dikhta hai, andar ka logic nahi
class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() = default;
    virtual PaymentResult pay(double amount, const std::string& instrument) = 0;
};
```
> Repo: `Ecommerce_Cart_Checkout_LLD/strategies/IPaymentStrategy.h`

#### 🟢 Encapsulation
**Data + methods ek jagah, state private.** Bahar se seedha data nahi badal sakte — controlled
access through methods.

```cpp
class BankAccount {
public:
    double getBalance() const { return balance_; }  // read
    bool debit(double amount) {                       // controlled write
        if (amount <= 0 || amount > balance_) return false;
        balance_ -= amount;
        return true;
    }
private:
    double balance_;  // ⭐ private — koi setBalance() nahi, warna rules bypass
};
```
> **Interview point:** `setBalance()` jaan-boojh ke nahi diya — warna koi
> `account.setBalance(999999)` karke paisa bana leta. Ye encapsulation ka asli fayda hai.
> Repo: `ATM_LLD/models/BankAccount.h`, `GPay_LLD/models/BankAccount.h`

#### 🟢 Inheritance
**is-a rishta, code reuse.** Child parent ka behavior inherit karta hai.

```cpp
class Vehicle {                    // base
protected:
    std::string id_;
    VehicleStatus status_;
public:
    virtual ~Vehicle() = default;
    virtual std::string getSubtype() const = 0;  // child define karega
    std::string describe() const { return id_ + " " + getSubtype(); }
};

class Car : public Vehicle {       // Car is-a Vehicle
    std::string getSubtype() const override { return "CAR"; }
};
```
> Repo: `Car_Rental_System_LLD/models/Vehicle.h` + `Car.h/Truck.h/Van.h/Motorcycle.h`

#### 🟢 Polymorphism
**Ek interface, kai roop.** Same call, alag behavior (runtime pe decide).

```cpp
std::vector<std::unique_ptr<Vehicle>> vehicles;
for (auto& v : vehicles) {
    std::cout << v->getSubtype();  // Car -> "CAR", Truck -> "TRUCK" (runtime dispatch)
}
```

**Do types of polymorphism:**
- **Compile-time (static):** function overloading, templates. Compiler decide karta.
- **Runtime (dynamic):** virtual functions, inheritance. Runtime pe vtable se decide.

### 3.2 — Abstraction vs Encapsulation (bahut confuse karte log)
| | Abstraction | Encapsulation |
|---|---|---|
| Level | DESIGN (kya dikhana) | IMPLEMENTATION (kaise chhupana) |
| Achieved by | interface, abstract class | private/protected + getters/setters |
| Focus | outside view | inside data protection |
| Example | "Car me brake hai" | "brake ka hydraulic system private" |

### 3.3 — ⭐ Composition vs Inheritance (SABSE zyada poocha jaata)

```cpp
// ❌ Inheritance (tight coupling — Car is-a Engine? NO!)
class Car : public Engine { };  // galat rishta

// ✅ Composition (has-a — Car has-a Engine)
class Car {
    Engine engine_;              // compose
    PricingStrategy* strategy_;  // runtime pe swap ho sakta
};
```

**Rule: "Favor composition over inheritance"** — kyun?
1. **Flexibility:** composition runtime pe behavior swap kar sakta (`car.setStrategy(...)`);
   inheritance compile-time pe fix.
2. **Fragile base class problem:** base badla to saare children toot sakte hain.
3. **Multiple behaviors:** ek class kai behaviors compose kar sakti; C++ me multiple inheritance
   diamond problem laata.

> Repo: `PricingService` me `PricingStrategy` **compose** kiya (inherit nahi) — isliye
> pricing algorithm runtime pe swap hota hai. `Car_Rental_System_LLD/services/PricingService.h`

### 3.4 — Interface vs Abstract class
```cpp
// Interface (pure abstract — sirf contract)
class IShape {
public:
    virtual ~IShape() = default;
    virtual double area() const = 0;      // pure virtual
    virtual double perimeter() const = 0;
};

// Abstract class (kuch implementation + kuch abstract)
class Shape {
protected:
    std::string color_;
public:
    virtual ~Shape() = default;
    std::string getColor() const { return color_; }  // concrete (shared)
    virtual double area() const = 0;                   // abstract (child define)
};
```

| | Interface | Abstract class |
|---|---|---|
| Implementation | koi nahi (pure) | kuch ho sakti |
| Multiple inherit | ✅ (kai interfaces) | ⚠ (diamond problem) |
| Kab use | pure contract chahiye | common code + contract |
| C++ | sab `= 0` | mix of `= 0` + concrete |

### 3.5 — Constructor types (C++)
```cpp
class Foo {
public:
    Foo() = default;                          // default constructor
    Foo(int x) : x_(x) {}                      // parameterized
    Foo(const Foo& other) : x_(other.x_) {}    // copy constructor
    Foo(Foo&& other) noexcept : x_(other.x_) {} // move constructor
    explicit Foo(double d) {}                   // explicit (implicit conversion band)
private:
    int x_ = 0;
};
```
> **`explicit` kyun?** Implicit conversion rokta hai. `Foo f = 3.0;` `explicit` se compile
> error dega — accidental conversions se bachata.

---

<a name="s4"></a>
## 4. SOLID Principles (bad vs good code)

SOLID = 5 principles jo maintainable, extensible code ke liye. **Har interview me guaranteed.**

### 4.1 — S: Single Responsibility Principle (SRP)
> "Ek class ko badalne ki **ek hi wajah** honi chahiye."

```cpp
// ❌ BAD — Order 3 kaam kar rahi (3 wajah badalne ki)
class Order {
    void calculateTotal();   // business logic
    void saveToDatabase();   // persistence
    void sendEmail();        // notification
    void printInvoice();     // presentation
};

// ✅ GOOD — har class ek kaam
class Order { double calculateTotal(); };       // sirf data + calc
class OrderRepository { void save(Order&); };   // persistence
class EmailService { void send(Order&); };      // notification
class InvoicePrinter { void print(Order&); };   // presentation
```
> Repo: `Ecommerce_Cart_Checkout_LLD` me 9 services — `PricingService` sirf paisa,
> `InventoryService` sirf stock, `OrderService` sirf storage. Har ek ki ek responsibility.

### 4.2 — O: Open/Closed Principle (OCP)
> "Classes extension ke liye OPEN, modification ke liye CLOSED."

```cpp
// ❌ BAD — naya payment method = existing code badlo (risky)
class PaymentProcessor {
    void pay(string type, double amt) {
        if (type == "card") { /* ... */ }
        else if (type == "upi") { /* ... */ }
        else if (type == "cod") { /* ... */ }  // naya add = ye function badlo
    }
};

// ✅ GOOD — naya method = nayi class (existing untouched)
class IPaymentStrategy { virtual PaymentResult pay(double amt) = 0; };
class CardStrategy : public IPaymentStrategy { /* ... */ };
class UpiStrategy  : public IPaymentStrategy { /* ... */ };
// Naya WalletStrategy add karo — PaymentProcessor ka ek line bhi nahi badla
```
> Repo: `GPay_LLD` me naya `PaymentRail` add karna = enum value + nayi Strategy + factory case.
> `TransferService` untouched. Ye OCP hai.

### 4.3 — L: Liskov Substitution Principle (LSP)
> "Child ko parent ki jagah use kar sako **bina behavior tode**."

```cpp
// ❌ BAD — LSP violation (Square is-a Rectangle? galat)
class Rectangle {
    virtual void setWidth(int w) { width_ = w; }
    virtual void setHeight(int h) { height_ = h; }
};
class Square : public Rectangle {
    void setWidth(int w) override { width_ = height_ = w; }  // surprise! height bhi badla
    void setHeight(int h) override { width_ = height_ = h; }
};
// Rectangle expect karne wala code Square se toot jaayega:
// r.setWidth(5); r.setHeight(4); assert(r.area() == 20);  // Square me 16 aayega!

// ✅ GOOD — dono ko alag rakho, ya common IShape interface
```
> **LSP ki nishaani:** agar child me koi method `throw "not supported"` karta hai ya parent ka
> contract todta hai — LSP violation. Repo: koi bhi `IPaymentStrategy` `pay()` me sahi kaam
> karta hai, koi surprise nahi.

### 4.4 — I: Interface Segregation Principle (ISP)
> "Chhote focused interfaces > ek bada fat interface. Client ko wo methods na dikhein jo wo use nahi karta."

```cpp
// ❌ BAD — fat interface (printer ko scan/fax kyun?)
class IMachine {
    virtual void print() = 0;
    virtual void scan() = 0;
    virtual void fax() = 0;
};
class SimplePrinter : public IMachine {
    void print() override { /* ok */ }
    void scan() override { throw "not supported"; }  // ⚠ forced to implement
    void fax() override { throw "not supported"; }
};

// ✅ GOOD — segregated interfaces
class IPrinter { virtual void print() = 0; };
class IScanner { virtual void scan() = 0; };
class SimplePrinter : public IPrinter { /* sirf print */ };
class MultiFunction : public IPrinter, public IScanner { /* dono */ };
```
> Repo: `Truecaller_LLD/strategies/ISpamScoringStrategy.h` me sirf 1 method — patla interface,
> implement karna aasan.

### 4.5 — D: Dependency Inversion Principle (DIP)
> "High-level modules low-level pe depend na karein — dono ABSTRACTION pe depend karein."

```cpp
// ❌ BAD — CheckoutService seedha CardStrategy pe depend (concrete)
class CheckoutService {
    CardStrategy card_;  // tightly coupled — UPI chahiye to code badlo
};

// ✅ GOOD — abstraction pe depend, concrete inject karo (Dependency Injection)
class CheckoutService {
    IPaymentStrategy& strategy_;  // abstraction
public:
    CheckoutService(IPaymentStrategy& s) : strategy_(s) {}  // inject
};
```
> **DIP + DI (Dependency Injection):** DIP principle hai, DI usko achieve karne ka tarika.
> Repo: har service constructor me dependencies inject hoti hain
> (`CartService(CatalogService&)`, `CheckoutService(6 services)`).

### 4.6 — Patterns SOLID ko kaise support karte hain
| Pattern | Principle |
|---|---|
| Strategy | Open/Closed (naya algo = nayi class) |
| Factory | Dependency Inversion (concrete creation chhupa) |
| Observer | SRP + loose coupling |
| Decorator | Open/Closed (behavior add bina modify) |
| Facade | ISP (client ko simple interface) |

---

<a name="s5"></a>
## 5. UML Diagrams (jo interview me draw karne padte hain)

### 5.1 — Class Diagram relationships (yaad rakho)
```
Inheritance (is-a)      : ──────▷  (hollow triangle)   Car ──▷ Vehicle
Realization (implements): ┈┈┈┈┈▷  (dashed + triangle)  Card ┈▷ IPayment
Composition (strong)    : ◆──────  (filled diamond)     House ◆── Room  (room dies with house)
Aggregation (weak)      : ◇──────  (hollow diamond)     Team ◇── Player (player survives)
Association (uses)      : ──────>  (arrow)               Order ──> Customer
Dependency (temp uses)  : ┈┈┈┈┈>  (dashed arrow)        Service ┈> Logger
```

### 5.2 — Composition vs Aggregation (bahut poocha)
```cpp
// Composition: Room House ke bina exist nahi kar sakta (strong ownership)
class House {
    std::vector<Room> rooms_;  // rooms House ke andar, House mara to rooms bhi
};

// Aggregation: Player Team ke bina bhi exist karta (weak)
class Team {
    std::vector<Player*> players_;  // players bahar se aaye, Team mari to players zinda
};
```

### 5.3 — Sequence diagram
Objects ke beech **time-ordered messages**. "Book ticket" ka flow: User → System → Service →
Repository. Repo ke har `design_diagram.md` me sequence diagrams hain (mermaid me).

### 5.4 — State diagram
Object ke states + transitions. Vending machine: `NO_MONEY → MONEY_INSERTED → DISPENSE`.
Repo: `vending_machine_LLD/design_diagram.md` me poora state diagram.

> 💡 Is repo ke **saare 8 `design_diagram.md` files** me mermaid class/sequence/state diagrams
> hain — real UML kaise dikhta dekhne ke liye wo padho (ATM, IRCTC, Car Rental, Parking,
> Elevator, Vending, Meeting, LinkedIn).

---

<a name="s6"></a>
## 6. Design Patterns — Creational (object banana)

Creational patterns object banane ke tarike control karte hain — creation logic ko chhupa ke
flexibility dete hain.

### 6.1 — 🔵 Singleton
**Intent:** ek class ka **sirf ek instance** ho, aur global access point ho.

**Kab:** config, logger, connection pool, cache — jahan ek hi shared instance chahiye.

```cpp
// ✅ Meyers Singleton — C++11 se thread-safe (static local ki init guaranteed)
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;   // ⭐ ek hi baar banega, thread-safe
        return instance;
    }
    void log(const std::string& msg) { /* ... */ }

    // Copy/move BAND karo — warna doosra instance ban jaayega
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
private:
    Logger() = default;   // ⭐ constructor private — bahar se new nahi kar sakte
};

// Use: Logger::getInstance().log("hello");
```

**Interview Q&A:**
- **Q: Thread-safe kaise?** C++11 guarantee karta hai ki `static` local variable ki
  initialization thread-safe hai (magic statics). Isliye Meyers singleton lock-free thread-safe.
- **Q: Problems?** Global state (testing mushkil — mock nahi kar sakte), hidden dependencies,
  tight coupling. **Modern advice:** Singleton ke bajaye Dependency Injection use karo jahan
  possible. Singleton sirf tab jab genuinely ek hi instance ka sense ho (logger, config).
- **Q: Double-checked locking?** Pre-C++11 me manual thread-safety ke liye use hota tha
  (`if (!instance) { lock; if (!instance) instance = new; }`). C++11 me zaroorat nahi.
- **Q: Eager vs Lazy?** Eager = program start pe banao. Lazy = pehli baar maangne pe banao
  (Meyers lazy hai). Lazy better agar object heavy ho aur hamesha na chahiye.

> Repo: `Logger_LLD` me Meyers singleton.

### 6.2 — 🔵 Factory Method
**Intent:** object creation ka `switch`/`if-else` ek jagah chhupa do. Client ko concrete class
ka naam pata na ho.

**Kab:** creation logic conditional/complex ho; naye types add hote rehte ho.

```cpp
class IPaymentStrategy { public: virtual ~IPaymentStrategy() = default; virtual PaymentResult pay(double) = 0; };
class CardStrategy : public IPaymentStrategy { /* ... */ };
class UpiStrategy  : public IPaymentStrategy { /* ... */ };
class CodStrategy  : public IPaymentStrategy { /* ... */ };

// ⭐ Factory — creation ek jagah. Naya method = yahan ek case.
class PaymentStrategyFactory {
public:
    static std::unique_ptr<IPaymentStrategy> create(PaymentMethod method) {
        switch (method) {
            case PaymentMethod::CARD: return std::make_unique<CardStrategy>();
            case PaymentMethod::UPI:  return std::make_unique<UpiStrategy>();
            case PaymentMethod::COD:  return std::make_unique<CodStrategy>();
        }
        throw std::runtime_error("unknown payment method");
    }
};
```

**Interview Q&A:**
- **Q: Factory ka fayda?** Client ko `new CardStrategy()` nahi likhna padta — wo enum deta hai,
  factory object deta hai. Concrete classes decouple ho jaati hain (DIP).
- **Q: `switch` me `default:` kyun nahi?** Naya enum value add karo to compiler `-Wswitch`
  warning dega. `default:` likh dete to bug runtime pe milta.
- **Q: Factory + Strategy jodi kyun?** Strategy batata hai "kaam kaise hoga", Factory batata hai
  "kaunsa object milega". Naya type add = 2 jagah (enum + factory), baaki untouched.

> Repo: `PaymentStrategyFactory` (Ecommerce), `VehicleFactory` (Car Rental), `MeetingFactory`,
> `BookingFactory` (IRCTC), `PaymentRailFactory`, `PaymentProcessorFactory` (Parking).

### 6.3 — 🔵 Abstract Factory
**Intent:** **related objects ka family** banao bina concrete classes bataye.

**Kab:** ek theme/platform ke multiple related products (Windows UI vs Mac UI).

```cpp
// Products
class Button { public: virtual void render() = 0; virtual ~Button() = default; };
class Checkbox { public: virtual void render() = 0; virtual ~Checkbox() = default; };

class WinButton : public Button { void render() override { /* windows */ } };
class MacButton : public Button { void render() override { /* mac */ } };
class WinCheckbox : public Checkbox { /* ... */ };
class MacCheckbox : public Checkbox { /* ... */ };

// ⭐ Abstract Factory — ek family banata hai
class IGUIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
    virtual ~IGUIFactory() = default;
};
class WindowsFactory : public IGUIFactory {
    std::unique_ptr<Button> createButton() override { return std::make_unique<WinButton>(); }
    std::unique_ptr<Checkbox> createCheckbox() override { return std::make_unique<WinCheckbox>(); }
};
// MacFactory bhi aisa hi — Mac wale products banata
```

**Factory Method vs Abstract Factory:**
| | Factory Method | Abstract Factory |
|---|---|---|
| Banata | ek product | related products ka family |
| Example | `createVehicle()` | `createButton() + createCheckbox()` (matching theme) |
| Complexity | simple | zyada (interface of factories) |

### 6.4 — 🔵 Builder
**Intent:** complex object **step-by-step** banao. Bahut saare optional parameters ho to helpful.

**Kab:** object me kai optional fields, ya construction multi-step ho (immutable object banane ke liye bhi).

```cpp
class Pizza {
public:
    class Builder {
        Pizza pizza_;
    public:
        Builder& size(const std::string& s) { pizza_.size_ = s; return *this; }
        Builder& cheese(bool c) { pizza_.cheese_ = c; return *this; }
        Builder& addTopping(const std::string& t) { pizza_.toppings_.push_back(t); return *this; }
        Pizza build() { return pizza_; }   // ⭐ final object
    };
private:
    std::string size_;
    bool cheese_ = false;
    std::vector<std::string> toppings_;
    Pizza() = default;
    friend class Builder;
};

// Use: fluent chaining
Pizza p = Pizza::Builder().size("large").cheese(true).addTopping("mushroom").build();
```

**Interview Q&A:**
- **Q: Builder vs telescoping constructor?** Telescoping = `Pizza(size)`, `Pizza(size, cheese)`,
  `Pizza(size, cheese, toppings)`... — unreadable, error-prone. Builder = named steps, readable.
- **Q: Builder vs Factory?** Factory = ek call me object (kaunsa type). Builder = multi-step
  construction (ek complex object ke parts). Builder tab jab construction hi complex ho.

> Gap: repo me standalone Builder nahi hai — Pizza/Notification builder add kar sakte ho.

### 6.5 — 🔵 Prototype
**Intent:** existing object ko **clone** karke naya banao (constructor se nahi).

**Kab:** object banana mehnga ho (DB se load, complex init), aur similar objects chahiye.

```cpp
class Shape {
public:
    virtual std::unique_ptr<Shape> clone() const = 0;   // ⭐ khud ki copy
    virtual ~Shape() = default;
};
class Circle : public Shape {
    int radius_;
public:
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);   // copy constructor call
    }
};
```
> Rare in interviews, par "deep vs shallow copy" ke context me poocha ja sakta.
> **Deep copy:** pointed data bhi copy. **Shallow copy:** sirf pointer copy (dono same data point).

### 6.6 — 🔵 Object Pool
**Intent:** mehenge objects (DB connections, threads) ko reuse karo, bar-bar create/destroy nahi.

**Kab:** object creation costly ho aur frequently chahiye (connection pool).

```cpp
class ConnectionPool {
    std::vector<std::unique_ptr<Connection>> available_;
public:
    std::unique_ptr<Connection> acquire() {
        if (available_.empty()) return std::make_unique<Connection>();
        auto conn = std::move(available_.back());
        available_.pop_back();
        return conn;
    }
    void release(std::unique_ptr<Connection> conn) {
        available_.push_back(std::move(conn));   // wapas pool me
    }
};
```
> HLD se overlap: DB connection pooling. `HLD_Interview.md` me detail.

### 📋 Creational patterns summary
| Pattern | Ek line | Repo |
|---|---|---|
| Singleton | ek hi instance | Logger |
| Factory Method | ek product, creation chhupa | VehicleFactory, PaymentStrategyFactory |
| Abstract Factory | related products ka family | (gap) |
| Builder | complex object step-by-step | (gap) |
| Prototype | clone karke banao | (gap) |
| Object Pool | mehenge objects reuse | (connection pooling) |

---

<a name="s7"></a>
## 7. Design Patterns — Structural (object jodna)

Structural patterns objects/classes ko jod ke bade structures banate hain, flexibility rakhte hue.

### 7.1 — 🟣 Facade
**Intent:** complex subsystem ke saamne ek **simple interface (darwaza)**. Client ko andar ke
components ka pata na ho.

```cpp
// Andar: 9 services. Bahar: ek darwaza.
class EcommerceCheckoutSystem {
    CatalogService catalog_;
    CartService cartService_;
    InventoryService inventory_;
    PaymentService paymentService_;
    CheckoutService checkoutService_;
    // ... 4 aur services
public:
    // Client sirf ye methods jaanta hai — andar ki services ka naam bhi nahi
    void addToCart(const std::string& user, const std::string& product, int qty);
    Order checkout(const std::string& user, PaymentMethod method, ...);
};
```

**Interview Q&A:**
- **Q: Facade vs Adapter?** Facade = interface **simplify** karta (complex → easy). Adapter =
  interface **badalta** (incompatible → compatible).
- **Q: Facade God object to nahi ban jaata?** Facade **coordinator** hona chahiye, logic andar
  services me. Agar facade khud business logic karne lage → God object anti-pattern.

> Repo: **HAR** core system Facade hai — `ATMSystem`, `GPaySystem`, `ParkingLot`,
> `TruecallerSystem`, `EcommerceCheckoutSystem`, `MeetingSchedulerSystem`, `LinkedinSystem`.

### 7.2 — 🟣 Decorator
**Intent:** object me **runtime pe** behavior add karo, bina uski class badle. Wrapping.

```cpp
class IRentalCost {
public:
    virtual double getCost() const = 0;
    virtual std::string getDescription() const = 0;
    virtual ~IRentalCost() = default;
};

// Base
class BaseRental : public IRentalCost {
    double base_; int days_;
public:
    double getCost() const override { return base_; }
    std::string getDescription() const override { return "Base rental"; }
};

// ⭐ Decorator — ek IRentalCost ko WRAP karta hai
class AddOnDecorator : public IRentalCost {
    std::shared_ptr<IRentalCost> inner_;   // wrapped object
    AddOn addOn_; int days_;
public:
    AddOnDecorator(std::shared_ptr<IRentalCost> inner, AddOn a, int d)
        : inner_(std::move(inner)), addOn_(a), days_(d) {}
    double getCost() const override {
        return inner_->getCost() + addOn_.dailyPrice() * days_;  // recurse + apna
    }
    std::string getDescription() const override {
        return inner_->getDescription() + " + " + addOn_.name();
    }
};

// Use: layers chadhao
std::shared_ptr<IRentalCost> cost = std::make_shared<BaseRental>(500, 3);
cost = std::make_shared<AddOnDecorator>(cost, gpsAddOn, 3);       // +GPS
cost = std::make_shared<AddOnDecorator>(cost, insuranceAddOn, 3); // +Insurance
// cost->getCost() -> 500 + GPS + Insurance (recursively)
```

**Decorator vs Inheritance:**
| | Inheritance | Decorator |
|---|---|---|
| Kab decide | compile-time (static) | runtime (dynamic) |
| Combinations | har combo = nayi class (explosion!) | kitne bhi layers, kisi bhi order |
| Example | `CarWithGPSAndInsurance` class | `wrap(wrap(base, gps), insurance)` |

> **Kyun Decorator?** Add-ons ka koi bhi combination runtime pe (GPS+Insurance+ChildSeat...).
> Inheritance se 2^n classes banti. Decorator me n classes, koi bhi combo.
> Repo: `Car_Rental_System_LLD/decorators/`. Bhi: Java I/O streams (`BufferedReader(FileReader)`).

### 7.3 — 🟣 Adapter
**Intent:** do **incompatible interfaces** ko jodo. Ek purane/third-party class ko naye interface
me fit karo.

```cpp
// Hamara system ye expect karta hai
class IMediaPlayer { public: virtual void play(const std::string& file) = 0; };

// Third-party library (hum badal nahi sakte)
class VlcLibrary { public: void playVlc(const std::string& path) { /* ... */ } };

// ⭐ Adapter — VlcLibrary ko IMediaPlayer bana deta hai
class VlcAdapter : public IMediaPlayer {
    VlcLibrary vlc_;
public:
    void play(const std::string& file) override {
        vlc_.playVlc(file);   // interface translate
    }
};
```

**Kab:** legacy code, third-party library jiska interface tumhare system se match nahi karta.
> Real: payment gateway integration — har gateway ka apna API, adapter se common interface.

### 7.4 — 🟣 Composite
**Intent:** objects ko **tree structure** me arrange karo (part-whole). Individual aur group
ko **same tarah** treat karo.

```cpp
// Common interface — file aur folder dono ise implement karte
class FileSystemNode {
public:
    virtual int getSize() const = 0;
    virtual void print(const std::string& indent = "") const = 0;
    virtual ~FileSystemNode() = default;
};

// Leaf — file
class File : public FileSystemNode {
    std::string name_; int size_;
public:
    int getSize() const override { return size_; }
    void print(const std::string& indent) const override { /* file name */ }
};

// ⭐ Composite — folder (bacche ho sakte)
class Folder : public FileSystemNode {
    std::string name_;
    std::vector<std::shared_ptr<FileSystemNode>> children_;
public:
    void add(std::shared_ptr<FileSystemNode> node) { children_.push_back(node); }
    int getSize() const override {
        int total = 0;
        for (auto& c : children_) total += c->getSize();  // ⭐ recurse — file/folder same
        return total;
    }
    void print(const std::string& indent) const override {
        for (auto& c : children_) c->print(indent + "  ");
    }
};
```

**Kab:** file system, org hierarchy, UI component tree, menu/submenu.
> **Key idea:** client ko farak nahi padta ki `File` hai ya `Folder` — dono `FileSystemNode` hain.
> `folder.getSize()` recursively sab jod deta.
> Gap: repo me standalone Composite nahi — File System LLD add kar sakte ho.

### 7.5 — 🟣 Bridge
**Intent:** **abstraction** aur **implementation** ko alag karo taaki dono independently badal
sakein. "Do dimensions of variation."

```cpp
// Implementation side
class IRenderer {
public:
    virtual void renderCircle(float r) = 0;
    virtual ~IRenderer() = default;
};
class VectorRenderer : public IRenderer { void renderCircle(float r) override {} };
class RasterRenderer : public IRenderer { void renderCircle(float r) override {} };

// Abstraction side — renderer ko COMPOSE karta (bridge)
class Shape {
protected:
    IRenderer& renderer_;   // ⭐ bridge to implementation
public:
    Shape(IRenderer& r) : renderer_(r) {}
    virtual void draw() = 0;
    virtual ~Shape() = default;
};
class Circle : public Shape {
    float radius_;
public:
    Circle(IRenderer& r, float rad) : Shape(r), radius_(rad) {}
    void draw() override { renderer_.renderCircle(radius_); }
};
// Ab Shape (Circle/Square) aur Renderer (Vector/Raster) independently badhte hain
// Bina Bridge ke: VectorCircle, RasterCircle, VectorSquare... (M×N classes)
```

**Bridge vs Strategy?** Dono composition use karte. Bridge = **structural** (2 hierarchies
alag rakhna, dono badhte hain). Strategy = **behavioral** (ek algorithm swap karna).
> Repo: `Logger_LLD` me bridge-jaisa (log level + output appender alag).

### 7.6 — 🟣 Proxy
**Intent:** asli object ke saamne ek **placeholder/wrapper** — access control, lazy loading,
caching, logging ke liye.

```cpp
class IImage { public: virtual void display() = 0; virtual ~IImage() = default; };

class RealImage : public IImage {   // heavy — disk se load
    std::string file_;
public:
    RealImage(const std::string& f) : file_(f) { loadFromDisk(); }
    void display() override { /* show */ }
    void loadFromDisk() { /* mehenga */ }
};

// ⭐ Proxy — lazy loading (asli image tabhi banao jab zaroorat)
class ImageProxy : public IImage {
    std::string file_;
    std::unique_ptr<RealImage> real_;
public:
    ImageProxy(const std::string& f) : file_(f) {}
    void display() override {
        if (!real_) real_ = std::make_unique<RealImage>(file_);  // ⭐ lazy
        real_->display();
    }
};
```

**Proxy types:** Virtual (lazy load), Protection (access control), Remote (network call wrap),
Caching (result cache). 
> **Proxy vs Decorator?** Dono wrap karte. Decorator = **behavior add**. Proxy = **access control**
> (same interface, but controls when/how real object used).

### 7.7 — 🟣 Flyweight
**Intent:** bahut saare similar objects ka **shared state** ek jagah rakho — memory bachao.

```cpp
// Intrinsic state (shared) — TreeType ek baar banega
class TreeType {
    std::string name_; std::string texture_; std::string color_;
public:
    TreeType(std::string n, std::string t, std::string c) : name_(n), texture_(t), color_(c) {}
};

// Flyweight factory — same type reuse
class TreeTypeFactory {
    std::unordered_map<std::string, std::shared_ptr<TreeType>> types_;
public:
    std::shared_ptr<TreeType> getType(const std::string& name, ...) {
        if (types_.count(name)) return types_[name];   // ⭐ reuse
        auto t = std::make_shared<TreeType>(name, ...);
        types_[name] = t;
        return t;
    }
};
// Tree object sirf position rakhta (extrinsic), TreeType share karta
// 1M trees, par sirf 3 TreeType objects — memory saved
```

**Kab:** game me lakhon trees/bullets, text editor me har character ka font object.

### 📋 Structural patterns summary
| Pattern | Ek line | Repo |
|---|---|---|
| Facade | subsystem ka ek darwaza | har core system |
| Decorator | runtime pe behavior add | Car Rental add-ons |
| Adapter | incompatible interfaces jodo | (gap) |
| Composite | tree (part-whole same treat) | (gap — File System) |
| Bridge | abstraction + impl alag | Logger |
| Proxy | access control / lazy / cache | (gap) |
| Flyweight | shared state, memory bachao | (gap) |

---

<a name="s8"></a>
## 8. Design Patterns — Behavioral (object communication)

Behavioral patterns objects ke beech responsibility aur communication ko organize karte hain.

### 8.1 — 🟠 Strategy
**Intent:** ek family of algorithms ko interchangeable banao. Algorithm ko runtime pe swap karo.

```cpp
class PricingStrategy {
public:
    virtual double calculate(double base, int units) const = 0;
    virtual ~PricingStrategy() = default;
};
class DailyPricing  : public PricingStrategy { double calculate(double b, int d) const override { return b * d; } };
class WeekendPricing: public PricingStrategy { double calculate(double b, int d) const override { return b * d * 1.5; } };

// Context — strategy ko COMPOSE karta, runtime pe set kar sakta
class PricingService {
    std::shared_ptr<PricingStrategy> strategy_;
public:
    PricingService(std::shared_ptr<PricingStrategy> s) : strategy_(std::move(s)) {}
    void setStrategy(std::shared_ptr<PricingStrategy> s) { strategy_ = std::move(s); }
    double price(double base, int units) { return strategy_->calculate(base, units); }
};
```

**Kab:** payment methods, pricing rules, sorting algorithms, compression, spam scoring —
"ek kaam ke kai tarike".
> Repo: Pricing (Car Rental, Parking), PaymentRail (GPay), SpamScoring (Truecaller),
> SlotFinding (Meeting), LoadBalancing (LoadBalancer). **Repo ka sabse common pattern.**

### 8.2 — 🟠 Observer
**Intent:** ek object (subject) badle to uske saare **dependents (observers)** ko **auto-notify**.

```cpp
class IObserver {
public:
    virtual void onNotify(const std::string& event) = 0;
    virtual ~IObserver() = default;
};

class Subject {   // publisher
    std::vector<IObserver*> observers_;
public:
    void subscribe(IObserver* o) { observers_.push_back(o); }
    void notifyAll(const std::string& event) {
        for (auto* o : observers_) o->onNotify(event);   // ⭐ broadcast
    }
};

class EmailObserver : public IObserver { void onNotify(const std::string& e) override { /* email */ } };
class SmsObserver   : public IObserver { void onNotify(const std::string& e) override { /* sms */ } };
```

**Kab:** notification systems, event handling, MVC (model badle → view update), pub-sub.
> Repo: `Parking` DisplayBoard (availability update), `Meeting`/`LinkedIn` notifications,
> `Car Rental` overdue notifier, `Logger` appenders.
> **Interview Q: Push vs Pull?** Push = subject data bhejta (`onNotify(data)`). Pull = observer
> khud maang ke leta (`onNotify(); observer.getData()`). Push simple, pull flexible.

### 8.3 — 🟠 State
**Intent:** object ka behavior uske **internal state** pe depend kare. State object khud decide
karta hai agla state.

```cpp
class VendingMachine;   // forward

class State {
public:
    virtual State* insertCash(VendingMachine* m, double amt) = 0;
    virtual State* selectItem(VendingMachine* m, int slot) = 0;
    virtual State* dispense(VendingMachine* m) = 0;
    virtual ~State() = default;
};

class NoMoneyState : public State {
public:
    State* insertCash(VendingMachine* m, double amt) override {
        return m->getMoneyInsertedState();   // ⭐ state KHUD transition decide karta
    }
    State* dispense(VendingMachine* m) override {
        std::cout << "pehle paisa daalo"; return this;  // stay
    }
    // ...
};
// Context
class VendingMachine {
    State* current_;
public:
    void insertCash(double amt) { current_ = current_->insertCash(this, amt); }
};
```

**Strategy vs State (SABSE poocha comparison):**
| | Strategy | State |
|---|---|---|
| Kaun swap decide | **client** (bahar se set) | **state khud** (transition) |
| States ek-doosre ko jaante? | nahi | haan (agla state lauta te) |
| Structure | same (interface + concrete) | same |
| Intent | algorithm swap | behavior state pe depend |
> Repo: `vending_machine_LLD` (State textbook example), `Elevator` (direction states).

### 8.4 — 🟠 Chain of Responsibility
**Intent:** request ko **handlers ki chain** me pass karo. Har handler ya to handle karta hai ya
agle ko pass karta.

```cpp
class Handler {
protected:
    Handler* next_ = nullptr;
public:
    Handler* setNext(Handler* n) { next_ = n; return n; }
    virtual void handle(int amount) {
        if (next_) next_->handle(amount);   // ⭐ pass to next
    }
    virtual ~Handler() = default;
};

// ATM cash dispenser — har note ka handler
class Rupee500Handler : public Handler {
public:
    void handle(int amount) override {
        int notes = amount / 500;
        if (notes > 0) { /* dispense */ std::cout << notes << " x 500\n"; }
        Handler::handle(amount % 500);   // baaki agle ko
    }
};
// chain: 500 -> 200 -> 100 -> 50
```

**Kab:** ATM cash dispensing, logging (levels), request middleware, approval hierarchy
(manager → director → CEO based on amount), event bubbling.
> Repo: `L22 ...(ATM_Cash_Dispenser)` — pure Chain of Responsibility. `Logger` levels.

### 8.5 — 🟠 Command
**Intent:** request ko **object** bana do (encapsulate). Isse undo/redo, queue, log possible.

```cpp
class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;    // ⭐ undo ke liye
    virtual ~Command() = default;
};

class InsertTextCommand : public Command {
    Document& doc_; std::string text_; int pos_;
public:
    void execute() override { doc_.insert(pos_, text_); }
    void undo() override { doc_.erase(pos_, text_.size()); }   // reverse
};

// Invoker — commands ka history rakhta (undo stack)
class Editor {
    std::stack<std::unique_ptr<Command>> history_;
public:
    void run(std::unique_ptr<Command> cmd) {
        cmd->execute();
        history_.push(std::move(cmd));
    }
    void undo() {
        if (!history_.empty()) { history_.top()->undo(); history_.pop(); }
    }
};
```

**Kab:** undo/redo (text editor), macro recording, task queue, remote control buttons,
transaction rollback.
> Gap: repo me Command nahi — Text Editor LLD add kar sakte ho (Command + Memento).

### 8.6 — 🟠 Memento
**Intent:** object ka **snapshot** save karo taaki baad me restore kar sako (undo). Encapsulation
tode bina.

```cpp
// Memento — state ka snapshot (opaque)
class EditorMemento {
    std::string content_;
    friend class Editor;
    EditorMemento(std::string c) : content_(std::move(c)) {}
};

class Editor {
    std::string content_;
public:
    EditorMemento save() { return EditorMemento(content_); }        // ⭐ snapshot
    void restore(const EditorMemento& m) { content_ = m.content_; } // ⭐ rollback
    void type(const std::string& t) { content_ += t; }
};

// Caretaker — mementos rakhta (undo stack)
```

**Command vs Memento (undo ke do tarike):**
- **Command:** har operation ka reverse store karo (`undo()` method).
- **Memento:** poora state snapshot store karo, restore karo.
- Command efficient (sirf delta), Memento simple (poora state).

### 8.7 — 🟠 Template Method
**Intent:** algorithm ka **skeleton (steps ka order)** base class me fix, kuch steps child override kare.

```cpp
class DataProcessor {
public:
    void process() {          // ⭐ template method — order FIX
        readData();
        transformData();      // child define karega
        saveData();
    }
    virtual ~DataProcessor() = default;
protected:
    void readData() { /* common */ }
    virtual void transformData() = 0;   // ⭐ hook — child implement
    void saveData() { /* common */ }
};
class CsvProcessor : public DataProcessor {
    void transformData() override { /* CSV specific */ }
};
```

**Template Method vs Strategy?** Template = **inheritance** (compile-time, ek algo skeleton).
Strategy = **composition** (runtime, poora algo swap). Template me sirf steps override hote.

### 8.8 — 🟠 Iterator
**Intent:** collection ke elements ko **traverse** karo bina uski internal structure expose kiye.
> C++ me STL iterators (`begin()`/`end()`) built-in. Custom container me `begin()/end()` +
> iterator class define karo taaki range-for chale.

### 8.9 — 🟠 Mediator
**Intent:** objects ke beech direct communication ke bajaye ek **central mediator** ke through.
Many-to-many ko one-to-many banao.

```cpp
// Bina mediator: har object har object ko jaanta (n² connections)
// Mediator ke saath: sab mediator se baat karte (n connections)
class ChatRoom {   // mediator
    std::vector<User*> users_;
public:
    void broadcast(const std::string& msg, User* from) {
        for (auto* u : users_) if (u != from) u->receive(msg);
    }
};
```
**Kab:** chat room, air traffic control, UI dialog (buttons ek doosre ko coordinate).
> Repo: `Elevator` controller thoda mediator-jaisa (panels + cars coordinate).

### 8.10 — 🟠 Visitor
**Intent:** object structure se **operations alag** karo. Nayi operation add karne ke liye classes
mat badlo.
```cpp
// Har element accept(visitor) karta, visitor har type ke liye visit() rakhta
// Nayi operation = naya visitor (elements untouched)
```
**Kab:** AST traversal (compiler), file system operations. Rare in interviews (double dispatch complex).

### 📋 Behavioral patterns summary
| Pattern | Ek line | Repo |
|---|---|---|
| Strategy | swappable algorithms | bahut jagah (pricing, payment, spam) |
| Observer | ek badla → sab notify | Parking board, notifications |
| State | behavior state pe depend | Vending Machine |
| Chain of Responsibility | request chain me pass | L22 ATM, Logger |
| Command | request ko object (undo) | (gap) |
| Memento | snapshot save/restore | (gap) |
| Template Method | algo skeleton fix | (gap) |
| Iterator | traverse bina structure expose | STL built-in |
| Mediator | central coordinator | Elevator (thoda) |
| Visitor | operations classes se alag | (rare) |

---

<a name="s9"></a>
## 9. Pattern comparisons — interview favourites

Ye comparisons **guaranteed** poochi jaati hain:

### Strategy vs State
- **Strategy:** client algorithm chunta hai; strategies ek-doosre ko nahi jaante.
- **State:** state khud agla state decide karta (transition andar); states ek-doosre ko jaante.

### Factory Method vs Abstract Factory
- **Factory Method:** ek product banata (`createVehicle`).
- **Abstract Factory:** related products ka family (`createButton + createCheckbox` matching theme).

### Decorator vs Inheritance
- **Inheritance:** compile-time, har combo nayi class (2^n explosion).
- **Decorator:** runtime, kitne bhi layers kisi bhi order.

### Decorator vs Proxy
- **Decorator:** behavior **add** karta (multiple wraps).
- **Proxy:** access **control** karta (lazy/protection/cache), usually ek wrap.

### Adapter vs Facade
- **Adapter:** interface **badalta** (incompatible → compatible).
- **Facade:** interface **simplify** karta (complex subsystem → easy).

### Adapter vs Bridge
- **Adapter:** existing incompatible code jodta (baad me).
- **Bridge:** design time pe abstraction + impl alag rakhta (pehle se planned).

### Composite vs Decorator
- Dono recursive structure. **Composite** = tree (part-whole). **Decorator** = ek object ke
  around layers.

### Chain of Responsibility vs Decorator
- **CoR:** ek handler request handle kar ke chain rok sakta hai.
- **Decorator:** sab layers execute hoti hain (chain nahi rukti).

### Observer vs Pub-Sub
- **Observer:** subject seedha observers ko jaanta (direct reference).
- **Pub-Sub:** ek **message broker** beech me (publisher/subscriber ek doosre ko nahi jaante) — HLD scale.

### Command vs Strategy
- **Strategy:** algorithm swap (kaise karna).
- **Command:** request ko object banana (kya karna) — undo/queue ke liye.

---

<a name="s10"></a>
## 10. C++ Specific Deep Dive

Repo C++ me hai, aur C++ LLD interviews me ye concepts guaranteed hain.

### 10.1 — Virtual functions + vtable/vptr ⭐
```cpp
class Base { public: virtual void foo(); };   // virtual -> vtable banta
class Derived : public Base { public: void foo() override; };

Base* b = new Derived();
b->foo();   // runtime pe Derived::foo() — kaise?
```
**Mechanism:**
- Har class jisme virtual function hai uska ek **vtable** (function pointers ka array) banta hai.
- Har object ke andar ek **vptr** (hidden pointer) hota hai jo apni class ke vtable ko point karta.
- Call: `vptr` dereference → vtable me index → function pointer call.
- **Cost:** ek extra indirection per call, aur compiler **inline nahi kar sakta** (target
  compile-time pe pata nahi).

**Interview Q&A:**
- **Q: Virtual call ka overhead?** Ek pointer dereference (vtable lookup) + no inlining. Modern
  CPUs pe usually negligible, par hot loops me matter kar sakta.
- **Q: Object size pe asar?** Ek vptr add hota hai (8 bytes 64-bit pe).
- **Q: Static vs dynamic binding?** Non-virtual = static (compile-time, type se). Virtual =
  dynamic (runtime, actual object se).

### 10.2 — Virtual destructor ⭐⭐ (bahut poocha)
```cpp
class Base { public: /* NO virtual dtor */ ~Base() {} };
class Derived : public Base { int* data_; public: ~Derived() { delete[] data_; } };

Base* b = new Derived();
delete b;   // ⚠ sirf ~Base() chalega, ~Derived() NAHI — data_ LEAK!
```
**Fix:** `virtual ~Base() = default;`
> **Rule:** jis class me koi virtual function hai (ya jo base banegi polymorphic use ke liye),
> uska destructor **virtual** hona chahiye. Repo me har interface (`IPaymentStrategy`, etc.)
> me `virtual ~X() = default`.

### 10.3 — RAII (Resource Acquisition Is Initialization)
**Resource ki lifetime object ki lifetime se bandhi.** Object scope se bahar → destructor →
resource free. Leak ka mauka hi nahi, exception-safe.
```cpp
// ❌ Manual — exception aaya to unlock/free miss
mtx.lock();
doWork();          // agar throw kiya to mtx locked reh gaya (deadlock!)
mtx.unlock();

// ✅ RAII — scope khatam hote hi auto-unlock (exception aaye tab bhi)
{
    std::lock_guard<std::mutex> guard(mtx);   // constructor: lock
    doWork();
}   // destructor: auto-unlock (chahe exception aaye)
```
**RAII examples:** `lock_guard` (mutex), `unique_ptr`/`shared_ptr` (memory), `fstream` (file),
`vector` (heap array).

### 10.4 — Smart pointers ⭐
```cpp
// unique_ptr — exclusive ownership (copy NAHI, sirf move)
std::unique_ptr<Foo> p = std::make_unique<Foo>();
// auto q = p;              // ❌ compile error (copy band)
auto q = std::move(p);      // ✅ ownership transfer, p ab null

// shared_ptr — shared ownership (reference counting)
std::shared_ptr<Foo> s1 = std::make_shared<Foo>();
auto s2 = s1;               // ✅ refcount = 2
// last shared_ptr marne pe object delete (refcount 0)

// weak_ptr — non-owning observer (circular reference todta)
std::weak_ptr<Foo> w = s1;  // refcount nahi badhata
if (auto locked = w.lock()) { /* alive hai */ }
```

**Interview Q&A:**
- **Q: unique vs shared kab?** Default `unique_ptr` (sasta, clear ownership). `shared_ptr` sirf
  jab genuinely multiple owners. `shared_ptr` me atomic refcount overhead.
- **Q: Circular reference problem?** `A` ka `shared_ptr<B>` aur `B` ka `shared_ptr<A>` → refcount
  kabhi 0 nahi hota → memory leak. Fix: ek taraf `weak_ptr`.
- **Q: make_shared vs new?** `make_shared` ek hi allocation (object + control block saath),
  exception-safe, efficient. `shared_ptr<Foo>(new Foo)` do allocations.
- **Q: unique_ptr member class ko move-only bana deta?** Haan — repo me `Coupon` me
  `unique_ptr<IDiscountStrategy>` → `Coupon` move-only (copy band). Chain effect.

### 10.5 — Rule of 0 / 3 / 5 ⭐
```cpp
// Rule of THREE: agar ek define kiya, teeno chahiye
class RawOwner {
    int* data_;
public:
    ~RawOwner() { delete data_; }                          // 1. destructor
    RawOwner(const RawOwner& o) { data_ = new int(*o.data_); }  // 2. copy ctor
    RawOwner& operator=(const RawOwner& o) { /* ... */ return *this; }  // 3. copy assign
};

// Rule of FIVE: C++11 me move bhi
class Modern {
    // ... + move ctor + move assign (Rule of 5)
    Modern(Modern&&) noexcept;
    Modern& operator=(Modern&&) noexcept;
};

// Rule of ZERO (BEST): raw resources mat rakho, smart pointers use karo
class Clean {
    std::unique_ptr<int> data_;   // ⭐ koi manual dtor/copy/move nahi chahiye
    std::vector<Foo> items_;      // RAII members khud sambhalte
};
```
> **Rule of Zero preferred:** agar members khud RAII hain (smart pointers, containers) to koi
> special member likhne ki zaroorat nahi — compiler-generated theek hain.

### 10.6 — Move semantics
```cpp
std::string a = "hello";
std::string b = std::move(a);   // a ka data CHURAYA (copy nahi), a ab valid-but-empty
```
- **lvalue** = naam wali cheez (`a`). **rvalue** = temporary (`"hello"`, `a + b`).
- `std::move` object ko rvalue banata → move constructor call → data steal (deep copy nahi).
- Bade objects (vector/string) me huge saving.
- Repo: `saveOrder(Order order)` by-value + `std::move` — caller apna object "de deta".

### 10.7 — `= delete` copy/move (self-referential classes)
```cpp
class System {
    ServiceA a_;
    ServiceB b_;
    Facade f_{a_, b_};   // f_ me a_, b_ ke REFERENCES
public:
    // ⚠ default move: references COPY hoti hain, RE-BIND nahi
    //    -> moved object ke f_ purane object ke a_,b_ ko point karega -> dangling!
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) = delete;
    System& operator=(System&&) = delete;
};
```
> **Real bug repo me:** `GPaySystem`, `EcommerceCheckoutSystem` — members ek doosre ko reference
> karte the, implicit move dangling references deta tha. Fix: `= delete`. (design docs me detail)

### 10.8 — const correctness
```cpp
class Order {
    double amount_;
public:
    double getAmount() const { return amount_; }  // const method — object modify nahi karta
    // const + non-const overload (read view vs write view)
    Item& getItem(int i) { return items_[i]; }
    const Item& getItem(int i) const { return items_[i]; }
};
```
- `const` method me `this` const hota — member modify nahi kar sakte.
- Getter hamesha `const`. Parameters `const&` (copy avoid + modify band).

### 10.9 — Member initialization order ⭐
```cpp
class Foo {
    A a_;   // pehle banega
    B b_;   // baad me (declaration order, NOT init-list order)
public:
    Foo() : b_(a_), a_() {}   // ⚠ b_ pehle likha par a_ pehle banega!
};
```
> **Members declaration order me construct hote hain**, init-list ke order me nahi. Agar `b_`
> `a_` ka reference le aur `b_` pehle declare ho → uninitialized `a_` use → UB. Repo:
> `EcommerceCheckoutSystem` me `catalog_` before `cartService_` (jo catalog ka ref leta hai).

### 10.10 — Exception safety
- **Basic guarantee:** exception aaye to no leak, valid state (par kya state pata nahi).
- **Strong guarantee:** operation ya poora ho ya kuch na badle (rollback). Repo: Ecommerce
  checkout `try/catch(...)` se reservation rollback.
- **No-throw guarantee:** kabhi throw nahi (destructors, `swap`, move — `noexcept`).
- **`throw;` (bare rethrow)** original exception preserve karta (`throw e;` slicing kar sakta).

### 10.11 — STL quick reference (LLD me kaam ke)
| Container | Use | Complexity |
|---|---|---|
| `vector` | dynamic array, order matters | push_back O(1) amortized, index O(1) |
| `unordered_map` | key-value, O(1) lookup | avg O(1), worst O(n) |
| `map` | sorted key-value | O(log n) |
| `unordered_set` | unique elements, O(1) | avg O(1) |
| `set` | sorted unique | O(log n) |
| `deque` | dono end pe insert | O(1) both ends |
| `priority_queue` | max/min heap | push/pop O(log n) |
| `list` | doubly linked | O(1) insert/erase at iterator |

> **LLD tip:** `unordered_map` non-deterministic order deta — jab output order matter kare
> (history, search results) to sort karo ya `map` use karo. Repo me ye baar-baar aata.

---

<a name="s11"></a>
## 11. Concurrency in LLD

Repo me `IRCTC_LLD` (per-run locking) aur `Concurrent_HashMap_LLD` concurrency ke examples hain.

### 11.1 — Race condition
Do threads shared data ko bina sync ke access karein aur result **timing pe depend** kare.
```cpp
int counter = 0;
void increment() { counter++; }   // ⚠ read-modify-write — NOT atomic
// do threads -> lost updates (counter expected 2000 par 1500 aata)
```

### 11.2 — Mutex + lock_guard
```cpp
std::mutex mtx;
int counter = 0;
void increment() {
    std::lock_guard<std::mutex> guard(mtx);   // RAII lock
    counter++;                                  // ab safe
}   // auto-unlock
```
- **`lock_guard`** — simple RAII, scope me lock. Manual unlock nahi.
- **`unique_lock`** — flexible (defer lock, manual unlock, condition_variable ke saath).
- **`scoped_lock`** (C++17) — multiple mutex ek saath, deadlock-free.

### 11.3 — Deadlock ⭐
Do threads circular wait me:
```cpp
// Thread 1: lock(A) then lock(B)
// Thread 2: lock(B) then lock(A)   -> DEADLOCK
```
**4 conditions (Coffman):** mutual exclusion, hold-and-wait, no preemption, circular wait.
**Bachaav:**
1. **Lock ordering** — hamesha ek hi order me locks lo (A phir B, kabhi B phir A nahi).
2. **`std::scoped_lock(A, B)`** — dono ek saath, deadlock-free.
3. **Timeout** — `try_lock_for`, na mile to release + retry.
4. **Lock granularity** — kam locks, chhote critical sections.
> Repo: IRCTC me non-nested lock ordering + per-run mutex.

### 11.4 — Fine vs coarse-grained locking
- **Coarse:** ek bada lock poore system pe — simple par slow (sab serialize).
- **Fine:** chhote per-resource locks — fast (parallelism) par complex.
> Repo: IRCTC per-train-run mutex — alag trains parallel, same train serialized.

### 11.5 — atomic
```cpp
std::atomic<int> counter{0};
counter++;   // ⭐ lock-free atomic (mutex se sasta simple counters ke liye)
```
- Simple counter/flag ke liye mutex overkill — atomic use karo.
- **Memory ordering:** `memory_order_seq_cst` (default, safest), `relaxed` (sirf atomicity,
  ordering nahi — counters ke liye). Repo: ConcurrentHashMap `relaxed`.

### 11.6 — condition_variable
```cpp
std::mutex mtx;
std::condition_variable cv;
std::queue<int> q;

void producer() {
    { std::lock_guard<std::mutex> g(mtx); q.push(42); }
    cv.notify_one();   // ⭐ waiter ko jagao
}
void consumer() {
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, []{ return !q.empty(); });   // ⭐ wait jab tak condition false
    int val = q.front(); q.pop();
}
```
**Kab:** producer-consumer, thread pool, bounded buffer. `wait` me predicate (lambda) do —
spurious wakeup se bachne ke liye.

### 11.7 — Thread-safe Singleton
```cpp
static Logger& getInstance() {
    static Logger instance;   // ⭐ C++11: static local init thread-safe (magic statics)
    return instance;
}
```

### 11.8 — Optimistic vs Pessimistic locking (DB/booking me)
- **Pessimistic:** pehle lock lo, phir kaam (booking systems). Conflict frequent ho to.
- **Optimistic:** bina lock kaam karo, commit pe check "kisi ne badla to nahi?" (version number).
  Conflict rare ho to fast.
> BookMyShow/IRCTC: seat booking me pessimistic (lock seat during payment).

---

<a name="s12"></a>
## 12. Classic LLD Problems — detailed approach

> Har problem ka format: **Requirements → Entities → Patterns → Key methods → Code sketch →
> Extensions**. ✅ = is repo me maujood (code padho!).

### 12.1 — 🅿️ Parking Lot ✅ (`Parking_lot_system_LLD`)
**Requirements:**
- Multiple vehicle types (car, bike, truck), multiple spot types (compact, large, handicapped, motorcycle)
- Entry/exit gates, ticket generation, fee calculation, payment
- Display board (availability)

**Entities:** `ParkingLot`, `ParkingSpot`, `Vehicle`, `Ticket`, `Gate`, `PricingStrategy`,
`PaymentProcessor`, `DisplayBoard`

**Patterns:** Strategy (pricing), Factory (payment processor), Observer (display board), Facade

**Key methods:**
```cpp
class ParkingLot {
    std::string parkVehicle(const Vehicle& v, int gateId);   // spot dhoondo, ticket do
    double unparkVehicle(const std::string& ticketId, ...);  // fee, payment
    ParkingSpot* findAvailableSpot(const Vehicle& v);        // spot-fit matching
    void notifyObservers();                                   // board update
};
```

**⭐ Spot-fit logic (interview me poocha jaata):**
```
bike  -> motorcycle spot (best), warna compact, warna large
car   -> compact (best), warna large
truck -> large only
handicapped spot -> permit chahiye
```

**Extensions to discuss:** multiple floors, EV charging spots, monthly pass, dynamic pricing,
reservation, valet parking.

---

### 12.2 — 🎬 BookMyShow / Movie Ticket Booking (similar: `IRCTC_LLD` ✅)
**Requirements:**
- Movies, shows, theatres, screens, seats
- **Concurrent booking** — do log ek seat na le lein (SABSE important challenge)
- Payment, booking confirmation

**Entities:** `Movie`, `Show`, `Theatre`, `Screen`, `Seat`, `Booking`, `User`, `Payment`

**⭐⭐ CORE CHALLENGE — seat locking (concurrency):**
```
1. User seat select kare -> seat ko TEMPORARILY LOCK karo (5-10 min timeout ke saath)
2. Payment window me seat "reserved" (koi aur nahi le sakta)
3. Payment SUCCESS -> seat CONFIRMED
4. Payment FAIL/TIMEOUT -> seat RELEASE (wapas available)
```
**Locking approach:**
- **Pessimistic:** seat pe lock lo booking ke dauran (BookMyShow aisa karta).
- Per-show lock (fine-grained) — alag shows parallel, same show serialized.
> Repo `IRCTC_LLD` me EXACTLY yahi hai: segment-based seat + per-run mutex. Wo code padho.

**Key methods:**
```cpp
bool lockSeats(showId, seatIds, userId);    // temporary hold
Booking confirmBooking(lockId, payment);    // payment ke baad
void releaseSeats(lockId);                    // timeout/fail pe
```

**Extensions:** seat types (recliner/normal), dynamic pricing, waitlist, group booking, offers.

---

### 12.3 — 💸 Splitwise (add karo — high value)
**Requirements:**
- Users, groups, expenses
- Split types: EQUAL, EXACT (custom amounts), PERCENTAGE
- Balance sheet: "kaun kisko kitna deta hai"
- Debt simplification (minimize transactions)

**Entities:** `User`, `Group`, `Expense`, `Split` (Equal/Exact/Percent), `BalanceSheet`

**Patterns:** Strategy (split type), Facade

**⭐ Balance tracking:**
```cpp
// balances[A][B] = A owes B
std::unordered_map<std::string, std::unordered_map<std::string, double>> balances_;

void addExpense(payer, amount, participants, SplitStrategy strategy) {
    auto shares = strategy->calculateShares(amount, participants);
    for (auto& [user, share] : shares) {
        if (user == payer) continue;
        balances_[user][payer] += share;   // user owes payer
        // net off: agar payer bhi user ko deta tha to adjust
    }
}
```

**⭐ Debt simplification (graph):** har user ka **net balance** nikaalo (kitna dena/lena hai).
Phir max-creditor ko max-debtor se settle karo (greedy) — transactions minimize.
```
Net: A = -30 (deta), B = +50 (leta), C = -20 (deta)
Settle: A->B 30, C->B 20  (2 transactions, minimal)
```

**Split Strategy:**
```cpp
class SplitStrategy { virtual Map<User,double> calculateShares(double amt, users) = 0; };
class EqualSplit : /* amt / n each */
class ExactSplit : /* custom amounts (sum == amt validate) */
class PercentSplit : /* % based (sum == 100 validate) */
```

**Extensions:** multiple currencies, expense categories, settlement history, recurring expenses.

---

### 12.4 — 🛗 Elevator System ✅ (`Elevator_System_LLD`)
**Requirements:** multiple elevators, floors, up/down requests (external), floor selection
(internal), scheduling, door safety, capacity limits.

**Entities:** `ElevatorSystemController`, `ElevatorCar`, `Door`, `Display`, `ElevatorScheduler`,
`FloorPanel`, `ElevatorInsidePanel`, `Request`

**Patterns:** State machine (direction IDLE/UP/DOWN, door OPEN/CLOSED), Strategy (scheduling)

**⭐ Scheduling (LOOK algorithm):** elevator ek direction me jaata hai jab tak us taraf stops
hain, phir mudta. Scheduler har elevator ko score deta:
```
IDLE elevator          -> score = distance (best)
same direction, on way -> score = distance + 1
opposite/away          -> score = distance + 10 (penalty)
minimum score wala jeetta
```

**⭐ Door safety interlock:** darwaza sirf tab khule jab elevator IDLE ho (chalte waqt block).
> Repo me poora state machine + scheduler. `design_diagram.md` dekho.

**Extensions:** express elevators, priority (fire/VIP), weight sensors, energy optimization.

---

### 12.5 — 🥤 Vending Machine ✅ (`vending_machine_LLD`)
**Requirements:** items in slots, insert cash, select item, dispense, refund, admin refill.

**Entities:** `VendingMachine`, `State` (NoMoney/MoneyInserted/Dispense), `Inventory`, `Slot`,
`Item`, `MoneyManager`

**Patterns:** **State** (textbook example)

**⭐ State transitions:**
```
NO_MONEY --insertCash--> MONEY_INSERTED
MONEY_INSERTED --dispense (paid>=price, stock)--> DISPENSE --> NO_MONEY
MONEY_INSERTED --refund/insufficient--> NO_MONEY
DISPENSE --ruko (koi aur action reject)--> DISPENSE
```
> Each state khud decide karta agla state (`return machine->getMoneyInsertedState()`).
> Repo me poora StateImpl. **Interview me State pattern ka best example.**

**Extensions:** multiple denominations + change, card payment, inventory alerts, multiple items.

---

### 12.6 — 🏧 ATM ✅ (`ATM_LLD` + `L22 ...`)
**Requirements:** card auth (PIN), balance inquiry, cash withdrawal, denomination dispensing.

**Entities:** `ATMSystem`, `Card`, `BankAccount`, `Transaction`, `AuthenticationService`,
`CashDispenser`

**Patterns:** State (session — logged in/out), Chain of Responsibility (cash denomination — L22)

**⭐ Denomination dispensing (backtracking):** greedy (bade note pehle) hamesha kaam nahi karta.
```
notes = {500, 300}, amount = 600
greedy: 500 -> baaki 100 -> 300 se nahi banta -> FAIL
backtracking: 500 chhodo -> 300+300 -> SUCCESS
```
> Repo `ATM_LLD` me backtracking, `L22` me Chain of Responsibility (dono approaches).

**Extensions:** deposit, transfer, mini-statement, multi-currency, daily limit.

---

### 12.7 — ⏱️ Rate Limiter (add karo — high value, SDE-2+)
**Requirements:** ek user/IP se limited requests per time window. Excess reject.

**Algorithms (SABSE important part):**

**1. Token Bucket** — bucket me tokens, har request ek token khaata, tokens refill rate se aate.
```cpp
class TokenBucket {
    double tokens_; double capacity_; double refillRate_; long lastRefill_;
    bool allow() {
        refill();                       // time ke hisaab se tokens add
        if (tokens_ >= 1) { tokens_--; return true; }
        return false;                    // reject
    }
    void refill() {
        long now = now_ms();
        tokens_ = min(capacity_, tokens_ + (now - lastRefill_) * refillRate_);
        lastRefill_ = now;
    }
};
```
Burst allow karta (bucket full ho to). Most common.

**2. Leaky Bucket** — requests queue me, fixed rate se process. Smooth output, burst nahi.

**3. Fixed Window** — har window (1 min) me counter. Simple par window boundary pe 2x burst possible.

**4. Sliding Window Log** — timestamps store, window ke bahar wale hata do. Accurate par memory heavy.

**5. Sliding Window Counter** — fixed + sliding ka hybrid (weighted). Practical balance.

**Comparison table:**
| Algorithm | Burst | Memory | Accuracy |
|---|---|---|---|
| Token Bucket | allows | low | good |
| Leaky Bucket | smooths | low | good |
| Fixed Window | 2x at boundary | low | poor |
| Sliding Log | no | high | best |
| Sliding Counter | limited | low | good |

**Concurrency:** distributed rate limiter me Redis (atomic INCR + TTL) use hota — HLD overlap.

**Extensions:** distributed (Redis), per-user + per-IP + global tiers, different limits per endpoint.

---

### 12.8 — 🗃️ LRU / LFU Cache ✅ (`LRU_Cache_LLD`, `LFU_Cache_LLD`)
**LRU (Least Recently Used):** capacity full → sabse purana used item hatao.
```cpp
// HashMap + Doubly Linked List
// map: key -> DLL node.  DLL: recency order (front=recent, back=oldest)
class LRUCache {
    std::unordered_map<int, Node*> map_;
    // get: node ko front pe move, return value
    // put: naya front pe; full ho to back (oldest) hatao
};   // get/put O(1)
```
**LFU (Least Frequently Used):** capacity full → sabse kam **frequency** wala hatao. Frequency
buckets (freq → list of nodes) + min-freq tracker. O(1).
> Repo me dono. Cache eviction HLD me bhi aata (`HLD_Interview.md`).

**Extensions:** TTL expiry, thread-safe, write-through/write-back, distributed (Redis).

---

### 12.9 — 🎮 Tic-Tac-Toe (add karo — warmup)
**Entities:** `Game`, `Board`, `Player`, `Cell`, `Move`, win-check strategy.
```cpp
class Board {
    std::vector<std::vector<char>> grid_;
    bool checkWin(int row, int col, char symbol);  // row/col/diagonal
    bool isFull();
};
class Game {
    Board board_; std::vector<Player> players_; int currentPlayer_;
    void play(int row, int col);   // validate, place, checkWin, switch turn
};
```
**Extensions:** N×N board, generalize to Connect-4, AI player (minimax Strategy).

---

### 12.10 — 🐍 Snake & Ladder (add karo — warmup)
**Entities:** `Game`, `Board`, `Player`, `Dice`, `Snake`, `Ladder`, `Cell`.
```cpp
class Game {
    Board board_; std::queue<Player> players_; Dice dice_;
    void play() {
        while (!won) {
            Player p = players_.front(); players_.pop();
            int roll = dice_.roll();
            int newPos = p.pos + roll;
            newPos = board_.getFinalPosition(newPos);  // snake/ladder apply
            p.pos = newPos;
            if (newPos >= 100) { won = true; break; }
            players_.push(p);   // wapas queue me (turn rotation)
        }
    }
};
```
**Extensions:** multiple dice, multiple winners, custom board size.

---

### 12.11 — 🔔 Notification System (add karo)
**Requirements:** multi-channel (email, SMS, push), templates, user preferences, retry.

**Entities:** `NotificationService`, `Channel` (Email/SMS/Push), `Notification`, `Template`,
`UserPreference`

**Patterns:** Strategy (channel), Observer (events → notify), Factory (channel), Decorator
(retry/logging wrap)
```cpp
class INotificationChannel { virtual void send(const Notification& n) = 0; };
class EmailChannel : /* SMTP */
class SmsChannel   : /* gateway */
class PushChannel  : /* FCM/APNS */

class NotificationService {
    std::vector<INotificationChannel*> channels_;
    void notify(User u, Notification n) {
        for (auto ch : getUserPreferredChannels(u)) ch->send(n);
    }
};
```
**Extensions:** rate limiting, priority queue, dedup, digest (batch), delivery tracking.

---

### 12.12 — 📁 In-memory File System (add karo — Composite)
**Requirements:** files, directories (nested), create/delete/move, size calculation, search.

**Patterns:** **Composite** (file + directory same interface)
```cpp
class FSNode { virtual int size() const = 0; virtual ~FSNode() = default; };
class File : public FSNode { int size() const override { return size_; } };
class Directory : public FSNode {
    std::unordered_map<std::string, std::shared_ptr<FSNode>> children_;
    int size() const override {
        int t = 0; for (auto& [n, c] : children_) t += c->size(); return t;  // recurse
    }
};
```
**Extensions:** permissions, symlinks, path resolution (`/a/b/c`), search by name/pattern.

---

### 12.13 — 💬 Chat Application / WhatsApp (add karo)
**Entities:** `User`, `Message`, `Conversation` (1-1 / group), `MessageQueue`, `Notification`
**Patterns:** Observer (new message → notify), Mediator (chat room), Strategy (delivery)
```cpp
class Conversation {
    std::vector<User*> participants_;
    std::vector<Message> messages_;
    void sendMessage(User* from, const std::string& text) {
        Message m{from, text, now()};
        messages_.push_back(m);
        for (auto* u : participants_) if (u != from) u->notify(m);  // deliver
    }
};
```
**⭐ Message states:** SENT → DELIVERED → READ (double tick). **Group me:** har participant ka
delivery/read status alag track. **Extensions:** online status, typing indicator, media,
encryption, message ordering.

---

### 12.14 — 🚕 Cab Booking / Uber (add karo — high value)
**Requirements:** riders, drivers, matching, pricing (surge), trip lifecycle, location tracking.

**Entities:** `Rider`, `Driver`, `Trip`, `Location`, `MatchingStrategy`, `PricingStrategy`,
`DriverMatchingService`

**Patterns:** Strategy (matching + surge pricing), State (trip: REQUESTED → ASSIGNED → STARTED →
COMPLETED), Observer (location updates)

**⭐ Driver matching:**
```cpp
class MatchingStrategy { virtual Driver* findDriver(Location pickup, drivers) = 0; };
class NearestDriverStrategy : /* min distance available driver */
// geospatial: QuadTree/Geohash se nearby drivers O(log n)
```

**⭐ Surge pricing (Strategy):**
```cpp
double fare = baseFare + perKm * distance + perMin * time;
fare *= surgeMultiplier;   // demand/supply ratio se (1.0 - 3.0x)
```
**Extensions:** pooling (shared rides), ETA prediction, ratings, cancellation policy, scheduled rides.

---

### 12.15 — 📝 Logger ✅ (`Logger_LLD`)
**Patterns:** Singleton (ek logger), Chain of Responsibility (log levels), Observer (appenders)
```cpp
// Level chain: DEBUG -> INFO -> WARN -> ERROR -> FATAL
// Appenders (observers): ConsoleAppender, FileAppender
```
**⭐ Interview point:** thread-safe logging (mutex around write), async logging (queue +
background thread), log rotation. Repo me Meyers singleton + chain.

---

### 12.16 — 📚 Other common problems (quick pointers)
| Problem | Key patterns / idea |
|---|---|
| **Coffee Vending Machine** | State (similar to vending) ya Builder (customization) |
| **Chess / Checkers** | Strategy (piece moves), State (game), board matrix |
| **Library Management** | CRUD + reservation, membership, fine calculation |
| **Hotel Booking** ✅ `OYO` | interval overlap (room availability), Observer |
| **URL Shortener** ✅ | Strategy (id gen: base62/hash), Repository, collision handling |
| **Pub-Sub / Message Queue** | Observer at scale, topics, partitions, consumer groups |
| **Meeting Scheduler** ✅ | interval overlap, Strategy (slot finding), Observer |
| **Text Editor** | Command (undo/redo) + Memento (snapshots) |
| **Food Delivery (Swiggy)** | similar to ecommerce + cab (matching delivery agent) |
| **Digital Wallet** ✅ `GPay` | Strategy (payment rail), compensating transaction |
| **Stock Exchange** | order matching engine, priority queue (bid/ask) |

---

<a name="s13"></a>
## 13. Rapid-fire Q&A (100+)

### 🧱 OOP concepts

**Q1. OOP ke 4 pillars?**
Abstraction, Encapsulation, Inheritance, Polymorphism.

**Q2. Abstraction vs Encapsulation?**
Abstraction = design level (kya dikhana — interface). Encapsulation = implementation level
(data kaise chhupana — private + methods).

**Q3. Composition vs Inheritance — kya prefer?**
Composition (has-a) prefer karo. Flexible (runtime swap), no fragile base class, no diamond problem.
Inheritance sirf true is-a rishta pe.

**Q4. Method overloading vs overriding?**
Overloading = same naam, alag parameters, compile-time (static polymorphism). Overriding =
child parent ka virtual method redefine, runtime (dynamic polymorphism).

**Q5. Interface vs abstract class?**
Interface = pure contract, no implementation, multiple inherit. Abstract class = kuch
implementation + kuch abstract, common code share.

**Q6. Multiple inheritance ka problem?**
Diamond problem — do parents ka common grandparent, ambiguity. C++ me virtual inheritance se
solve. Java me interfaces se avoid.

**Q7. Association vs Aggregation vs Composition?**
Association = general "uses" rishta. Aggregation = weak has-a (part alag survive kare — Team/Player).
Composition = strong has-a (part maalik ke saath mare — House/Room).

**Q8. Coupling vs Cohesion?**
Coupling (kam chahiye) = modules kitna ek doosre pe depend. Cohesion (zyada chahiye) = ek module
ke andar cheezein kitni related. Goal: loose coupling, high cohesion.

**Q9. "Program to interface, not implementation" ka matlab?**
Variable/parameter ka type interface rakho, concrete class nahi. Isse implementation swap ho
sakti bina caller badle.

**Q10. Static vs dynamic binding?**
Static = compile-time (non-virtual, function type se decide). Dynamic = runtime (virtual, actual
object se, vtable).

### 🎖️ SOLID

**Q11. SOLID expand karo.**
Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion.

**Q12. SRP kaise judge karein?**
"Ek class ko badalne ki ek hi wajah." Agar class print + save + calc kar rahi — 3 wajah — violation.

**Q13. Open/Closed ka example?**
Naya payment method = nayi Strategy class (existing code untouched). Enum + factory case + class.

**Q14. Liskov violation ki nishaani?**
Child method `throw "not supported"` kare, ya parent ka contract tode (Square/Rectangle problem).

**Q15. Interface Segregation kyun?**
Fat interface client ko un methods pe force karta jo wo use nahi karta. Chhote interfaces better.

**Q16. Dependency Inversion vs Dependency Injection?**
DIP = principle (abstraction pe depend). DI = technique (dependencies bahar se inject, constructor me).

**Q17. Kaunsa pattern kaunse SOLID ko support?**
Strategy → OCP. Factory → DIP. Observer → SRP. Decorator → OCP.

### 🧩 Design Patterns

**Q18. Design pattern kya hai?**
Common design problems ka reusable solution template. 3 categories: Creational, Structural, Behavioral.

**Q19. Strategy vs State?**
Strategy = client algorithm chunta, states independent. State = state khud transition decide karta.

**Q20. Factory Method vs Abstract Factory?**
Factory Method = ek product. Abstract Factory = related products ka family.

**Q21. Decorator vs Inheritance?**
Decorator = runtime, kitne bhi combo. Inheritance = compile-time, class explosion (2^n).

**Q22. Adapter vs Facade?**
Adapter = interface badalta (incompatible→compatible). Facade = simplify karta (complex→easy).

**Q23. Proxy vs Decorator?**
Decorator = behavior add. Proxy = access control (lazy/protection/cache).

**Q24. Singleton ke problems?**
Global state, testing mushkil, hidden dependencies, tight coupling. DI prefer karo jahan possible.

**Q25. Singleton thread-safe kaise (C++)?**
Meyers singleton — `static Foo instance;` local. C++11 magic statics thread-safe.

**Q26. Observer kahan use?**
Notification, event handling, MVC, pub-sub. Ek badla → dependents auto-notify.

**Q27. Command pattern ka fayda?**
Request ko object banata — undo/redo, queue, log, macro possible.

**Q28. Chain of Responsibility example?**
ATM cash dispenser (note handlers), logging levels, approval hierarchy, middleware.

**Q29. Composite kab?**
Tree structure (part-whole) — file system, org chart, UI tree. Leaf + composite same interface.

**Q30. Flyweight kya bachata?**
Memory — shared intrinsic state (lakhon objects, kuch shared types). Game trees, text fonts.

**Q31. Template Method vs Strategy?**
Template = inheritance (steps override, skeleton fix). Strategy = composition (poora algo swap).

**Q32. Bridge vs Strategy?**
Bridge = structural (2 hierarchies independent). Strategy = behavioral (algo swap).

**Q33. Kaunsa pattern most overused hai?**
Singleton (aksar galat jagah — global state banata). DI better hota.

### ⚙️ C++

**Q34. Virtual function kaise kaam karti?**
vtable (function pointers) + vptr (per object). Call: vptr → vtable → function pointer.

**Q35. Virtual destructor kyun?**
Base pointer se derived delete karo aur dtor non-virtual → sirf base dtor → derived leak + UB.

**Q36. RAII kya hai?**
Resource lifetime = object lifetime. Scope khatam → dtor → resource free. Exception-safe.

**Q37. unique_ptr vs shared_ptr?**
unique = exclusive (copy band, move only). shared = refcount (multiple owners). unique default choice.

**Q38. weak_ptr kab?**
Circular reference todne ke liye (A↔B shared_ptr → leak). Non-owning observer.

**Q39. make_shared vs shared_ptr(new)?**
make_shared = ek allocation (object + control block), exception-safe, faster.

**Q40. Rule of 3/5/0?**
3: dtor/copy-ctor/copy-assign saath. 5: + move-ctor/move-assign. 0: RAII members, koi special nahi.

**Q41. Move semantics kya?**
std::move object ko rvalue banata → data steal (copy nahi). Bade objects me saving.

**Q42. lvalue vs rvalue?**
lvalue = naam wali (address le sakte). rvalue = temporary (`a+b`, literals).

**Q43. const correctness kyun?**
const method object modify nahi karta. Getter const, params const&. Compile-time safety.

**Q44. Member init order?**
Declaration order me construct hote (init-list order me nahi). Reference members ka dhyan.

**Q45. Shallow vs deep copy?**
Shallow = pointer copy (dono same data). Deep = pointed data bhi copy (alag).

**Q46. `explicit` keyword kya?**
Implicit conversion rokta. `explicit Foo(int)` → `Foo f = 3;` compile error.

**Q47. `= default` aur `= delete`?**
default = compiler-generated implementation. delete = us function ko band (copy/move rokna).

**Q48. static member vs static local?**
static member = class ka shared (sab instances me ek). static local = function me ek baar init.

**Q49. Exception safety levels?**
Basic (no leak), Strong (all-or-nothing rollback), No-throw (never throws — dtor/swap).

**Q50. `throw;` vs `throw e;`?**
`throw;` original exception rethrow (type preserve). `throw e;` slicing kar sakta (base type).

**Q51. unordered_map vs map?**
unordered = hash, O(1) avg, no order. map = tree (RB), O(log n), sorted.

**Q52. vector vs list?**
vector = contiguous, O(1) index, cache-friendly. list = linked, O(1) insert/erase at iterator.

**Q53. emplace vs insert/push?**
emplace = in-place construct (no temporary). push/insert = copy/move existing object.

### 🧵 Concurrency

**Q54. Race condition kya?**
Shared data + no sync + result timing pe depend. Lost updates.

**Q55. Mutex vs atomic?**
Mutex = critical section (complex ops). atomic = single variable lock-free (counters/flags).

**Q56. Deadlock 4 conditions?**
Mutual exclusion, hold-and-wait, no preemption, circular wait.

**Q57. Deadlock se kaise bachein?**
Lock ordering, scoped_lock (multiple), timeout, kam locks.

**Q58. lock_guard vs unique_lock?**
lock_guard = simple RAII (scope me lock). unique_lock = flexible (defer, manual, condition_variable).

**Q59. condition_variable kab?**
Producer-consumer, thread signaling. wait me predicate (spurious wakeup guard).

**Q60. Optimistic vs pessimistic locking?**
Pessimistic = lock pehle (conflict frequent). Optimistic = version check at commit (conflict rare).

**Q61. Fine vs coarse-grained locking?**
Coarse = ek bada lock (simple, slow). Fine = per-resource (fast, complex). Repo IRCTC = fine.

**Q62. Thread pool ka fayda?**
Thread creation mehnga — pool reuse karta. Bounded concurrency, queue tasks.

### 🏗️ Design decisions

**Q63. Enum kyun (magic strings ke bajaye)?**
Type safety, limited valid values, compiler -Wswitch warning, refactor-safe.

**Q64. DTO vs Domain object?**
DTO = sirf data (struct, no logic). Domain = invariant + behavior (class, private).

**Q65. Idempotency kya?**
Same operation kai baar = result same (side-effect ek baar). Payment retry safe. clientRequestId.

**Q66. Repository pattern kya?**
Data access abstraction — save/find/delete. Storage (DB/memory) swap ho sakta bina callers badle.

**Q67. Service layer kya?**
Business logic ki layer (models aur controllers ke beech). Har service ek responsibility.

**Q68. Anemic domain model kya (anti-pattern)?**
Models sirf data (getters/setters), saara logic services me. OOP ka fayda kho jaata.

**Q69. God object anti-pattern?**
Ek class sab kuch kar rahi (SRP violation). Facade God object na ban jaaye — logic services me.

**Q70. Snapshot pattern (invoice/order)?**
Historical record me values copy karo (price/name) taaki baad me source badle to record na badle.

**Q71. Null object pattern?**
`nullptr` check ke bajaye ek "do-nothing" object return karo. NullLogger, EmptyCart.

**Q72. Immutability ka fayda?**
Thread-safe (no mutation), predictable, no accidental changes. Value objects immutable rakho.

**Q73. Defensive copy kya?**
Getter me internal collection ki copy return karo (ya const ref) taaki caller modify na kare.

**Q74. Fail-fast kya?**
Invalid input pe turant throw (silent continue nahi). Constructor me validation — always-valid object.

**Q75. Guard clause?**
Method start me invalid cases check + early return/throw. Nesting kam, readable.

### 🎯 General LLD

**Q76. LLD interview me pehla step?**
Requirements clarify (functional + non-functional + scope). Kabhi turant code nahi.

**Q77. Classes kaise nikaalein?**
Noun extraction (problem statement ke nouns = classes, verbs = methods).

**Q78. Kitne design patterns use karein?**
Jitne genuinely zaroorat ho. Thoop mat — over-engineering anti-pattern.

**Q79. Extensibility kaise dikhaayein?**
"Naya feature X aaya to sirf ye class add hogi" — OCP demonstrate karo.

**Q80. Concurrency kab discuss karein?**
Interviewer poochega, ya jab shared mutable state ho (booking, inventory, counter).

**Q81. Trade-offs kaise bataayein?**
Har design decision ka pro/con. "Abhi O(N) scan, index se O(1) — abhi simple rakha."

**Q82. In-memory vs DB kaise handle?**
Repository pattern — abstraction rakho, in-memory implement karo, "DB later swap".

**Q83. UML class diagram me kya dikhाना?**
Classes, attributes, methods (visibility), relationships (inheritance/composition/etc).

**Q84. Validation kahan — model ya service?**
Invariant (balance >= 0) = model ke andar (maalik). Business rules = service. Both layers.

**Q85. Error handling — exception ya result object?**
Exceptional cases = exception. Expected failures (payment decline) = result object (status + reason).

**Q86. Extensibility vs YAGNI balance?**
Design extensible banao (interfaces), par features abhi mat banao jo nahi chahiye.

**Q87. Circular dependency kaise todein?**
Interface introduce karo (DIP), ya event/observer, ya dependency direction reverse.

**Q88. Static method kab?**
Jab state ki zaroorat na ho (pure function). Utility, factory. Warna instance method.

**Q89. Composition over inheritance ka repo example?**
PricingService me PricingStrategy compose (inherit nahi) — runtime swap.

**Q90. Facade ka fayda ek line me?**
Client ko simple interface, subsystem complexity chhupi, loose coupling.

### 🔥 Tricky / senior

**Q91. Diamond problem C++ me kaise solve?**
Virtual inheritance (`class D : virtual public B`). Ek hi base instance.

**Q92. SFINAE / templates LLD me?**
Generic containers, policy-based design. LLD me usually zaroorat nahi, mention kar sakte.

**Q93. Dependency Injection ke types?**
Constructor injection (best — mandatory deps), setter injection (optional), interface injection.

**Q94. Law of Demeter?**
"Sirf apne dosto se baat karo" — `a.getB().getC().doX()` avoid (chained calls = tight coupling).

**Q95. CQRS kya?**
Command Query Responsibility Segregation — read aur write models alag. HLD me zyada.

**Q96. Event sourcing?**
State ko events ki sequence se derive karo (current snapshot ke bajaye). Ledger, audit.

**Q97. Saga pattern?**
Distributed transaction — compensating actions se rollback. Repo: Ecommerce reservation saga.

**Q98. Circuit breaker (LLD side)?**
External call fail hote rahein to "circuit open" karo (fail fast), thodi der baad retry. HLD overlap.

**Q99. Builder immutability ke liye kaise?**
Builder me fields set karo, `build()` immutable object return kare (final, no setters).

**Q100. Pattern galat use ki nishaani?**
Code samajhna mushkil ho gaya, ya pattern ne complexity badhayi bina fayde ke — remove karo.

**Q101. "Design X" me stuck ho to?**
Requirements dobara clarify, simplest working design se shuru, phir iterate + extend.

**Q102. Interviewer "scale kaise" poochhe (LLD me)?**
Services stateless banao, data access abstract (repository), locking strategy batao — phir HLD me jaao.

---

<a name="s14"></a>
## 14. Interview Do's & Don'ts

### ✅ DO
- **Requirements clarify karo pehle** (5-10 min sawaal) — sabse bada differentiator
- **Think out loud** — interviewer tumhari soch dekhna chahta hai, chup mat raho
- **Interfaces pehle** design karo, phir concrete classes
- **Noun/verb extraction** se entities/methods nikaalo
- **Trade-offs khud bolo** ("abhi O(N), index se O(1) ho sakta")
- **Ek flow end-to-end** dikhao (book → pay → confirm)
- **Extensibility demonstrate karo** ("naya type = nayi class")
- **Edge cases + concurrency** khud raise karo
- **Naming clean** rakho (readable > clever)
- **SOLID justify karo** jab pattern use karo

### ❌ DON'T
- Turant code likhna (requirements skip) 🚫
- Har jagah pattern thoopna (over-engineering) 🚫
- Chup rehna / interviewer ko guess karne dena 🚫
- God class banana (ek class sab kuch) 🚫
- Concurrency/edge-cases ignore karna 🚫
- Premature optimization (pehle correct, phir fast) 🚫
- Magic numbers/strings (enum/const use karo) 🚫
- Deep inheritance hierarchies (composition prefer) 🚫

### 🗣️ Interviewer ke common closing questions
1. "Ye system scale kaise karega?" → stateless services, DB, caching, sharding (HLD)
2. "Concurrency handle kaise?" → locking strategy, optimistic vs pessimistic
3. "Naya feature X add karna ho?" → design kitna extensible (OCP)
4. "Kaunse patterns use kiye aur kyun?" → justify, blindly nahi
5. "Is design ke trade-offs?" → har decision ka pro/con
6. "Testing kaise karoge?" → DI se mock, stateless services, unit test

---

<a name="s15"></a>
## 15. Is repo ka index — kaunsa concept kahan

Har folder me: detailed Hinglish comments + `design_patterns_used.md` + `design_diagram.md`.

### Patterns → repo folder
| Pattern | Folder |
|---|---|
| **Strategy** | Car_Rental, Parking_lot, GPay, Truecaller, Meeting_Scheduler, LoadBalancer |
| **Factory** | Car_Rental (Vehicle), Ecommerce (Payment), IRCTC (Booking), Meeting, Parking |
| **Facade** | ATM, GPay, Parking, Truecaller, Ecommerce, Meeting, LinkedIn (sab core) |
| **Observer** | Parking (DisplayBoard), Meeting, LinkedIn, Car_Rental, Logger |
| **Decorator** | Car_Rental (add-ons) |
| **State** | vending_machine, Elevator |
| **Chain of Responsibility** | L22 (ATM cash dispenser), Logger (levels) |
| **Singleton** | Logger |
| **Bridge** | Logger |

### Concepts → repo folder
| Concept | Folder |
|---|---|
| **Concurrency / locking** | IRCTC (per-run mutex), Concurrent_HashMap |
| **Interval overlap** | IRCTC (segments), Meeting_Scheduler, OYO |
| **Cache eviction** | LRU_Cache, LFU_Cache |
| **Compensating transaction / saga** | Ecommerce (reservation), GPay (reversal) |
| **Idempotency** | Ecommerce (clientRequestId), GPay |
| **Rule of 5 / dangling refs bug** | GPay, Ecommerce (= delete copy/move) |
| **Exception handling** | Exception_Handling, JSON_Parser |
| **Parsing** | JSON_Parser, In_Memory_SQL_Database |

### 📖 Recommended study order
1. **vending_machine** (State — clean pattern)
2. **Parking_lot** (Strategy + Factory + Observer)
3. **Car_Rental** (Decorator + 4 more patterns)
4. **IRCTC** (concurrency + interval overlap)
5. **Ecommerce_Cart_Checkout** (saga + idempotency + services)
6. **Elevator** (state machine + scheduler)
7. **GPay** (compensating transaction + bugs)

---

<a name="s16"></a>
## 16. Complete worked example — Parking Lot (full code)

Ye ek **end-to-end** example hai — jaise interview me likhna chahiye. Requirements se code tak.

### Step 1 — Requirements clarify (jo tum poochoge)
```
Q: Vehicle types? -> Car, Bike, Truck
Q: Spot types? -> Compact, Large, Motorcycle, Handicapped
Q: Pricing? -> Hourly (type ke hisaab se rate)
Q: Payment? -> Cash / Card / UPI (channel: agent / machine)
Q: Concurrency? -> abhi single-threaded, mention karenge
Q: Multiple floors/gates? -> multiple gates, single floor (scope)
```

### Step 2 — Enums
```cpp
enum class VehicleType { CAR, BIKE, TRUCK };
enum class SpotType    { COMPACT, LARGE, MOTORCYCLE, HANDICAPPED };
enum class PaymentMethod  { CASH, CARD, UPI };
enum class PaymentChannel { AGENT, MACHINE };
```

### Step 3 — Models
```cpp
class Vehicle {
    std::string number_;
    VehicleType type_;
    bool handicappedPermit_;
public:
    Vehicle(std::string num, VehicleType t, bool permit = false)
        : number_(std::move(num)), type_(t), handicappedPermit_(permit) {}
    const std::string& getNumber() const { return number_; }
    VehicleType getType() const { return type_; }
    bool hasPermit() const { return handicappedPermit_; }
};

class ParkingSpot {
    int id_;
    SpotType type_;
    bool occupied_ = false;
    std::string parkedVehicle_;
public:
    ParkingSpot(int id, SpotType t) : id_(id), type_(t) {}
    int getId() const { return id_; }
    SpotType getType() const { return type_; }
    bool isOccupied() const { return occupied_; }

    // ⭐ spot-fit logic (interview me ye poocha jaata)
    bool canFit(const Vehicle& v) const {
        if (occupied_) return false;
        if (type_ == SpotType::HANDICAPPED) return v.hasPermit();
        switch (type_) {
            case SpotType::MOTORCYCLE: return v.getType() == VehicleType::BIKE;
            case SpotType::COMPACT:    return v.getType() == VehicleType::CAR ||
                                              v.getType() == VehicleType::BIKE;
            case SpotType::LARGE:      return true;   // sab fit
            default:                   return false;
        }
    }
    void park(const std::string& num) { occupied_ = true; parkedVehicle_ = num; }
    void vacate() { occupied_ = false; parkedVehicle_.clear(); }
};

class Ticket {
    std::string id_;
    std::string vehicleNumber_;
    int spotId_;
    long long entryTime_;
public:
    Ticket(std::string id, std::string num, int spot, long long t)
        : id_(std::move(id)), vehicleNumber_(std::move(num)), spotId_(spot), entryTime_(t) {}
    const std::string& getId() const { return id_; }
    int getSpotId() const { return spotId_; }
    long long getEntryTime() const { return entryTime_; }
};
```

### Step 4 — Strategy (pricing) + Factory (payment)
```cpp
// Strategy: pricing algorithm swappable
class PricingStrategy {
public:
    virtual double calculateFee(VehicleType type, long long entry, long long exit) const = 0;
    virtual ~PricingStrategy() = default;
};
class HourlyPricing : public PricingStrategy {
public:
    double calculateFee(VehicleType type, long long entry, long long exit) const override {
        long long secs = exit - entry;
        int hours = std::max(1, (int)((secs + 3599) / 3600));   // ceil, min 1
        double rate = (type == VehicleType::TRUCK) ? 80 :
                      (type == VehicleType::CAR)   ? 40 : 20;    // bike
        return rate * hours;
    }
};

// Strategy: payment processor
class PaymentProcessor {
public:
    virtual bool pay(double amount, PaymentMethod m) = 0;
    virtual ~PaymentProcessor() = default;
};
class AgentProcessor   : public PaymentProcessor { bool pay(double a, PaymentMethod m) override { return true; } };
class MachineProcessor : public PaymentProcessor { bool pay(double a, PaymentMethod m) override { return true; } };

// Factory: channel -> processor
class PaymentFactory {
public:
    static std::unique_ptr<PaymentProcessor> create(PaymentChannel ch) {
        if (ch == PaymentChannel::AGENT) return std::make_unique<AgentProcessor>();
        return std::make_unique<MachineProcessor>();
    }
};
```

### Step 5 — Observer (display board)
```cpp
class IParkingObserver {
public:
    virtual void onUpdate(const std::map<SpotType, int>& availability, bool full) = 0;
    virtual ~IParkingObserver() = default;
};
class DisplayBoard : public IParkingObserver {
public:
    void onUpdate(const std::map<SpotType, int>& availability, bool full) override {
        // print availability / "FULL"
    }
};
```

### Step 6 — Facade (ParkingLot)
```cpp
class ParkingLot {
    std::vector<std::unique_ptr<ParkingSpot>> spots_;
    std::unordered_map<std::string, Ticket> activeTickets_;
    std::unordered_map<std::string, VehicleType> typeByTicket_;
    std::vector<IParkingObserver*> observers_;
    std::unique_ptr<PricingStrategy> pricing_;
    int ticketCounter_ = 0;

public:
    explicit ParkingLot(std::unique_ptr<PricingStrategy> p) : pricing_(std::move(p)) {}

    void addSpot(int id, SpotType t) { spots_.push_back(std::make_unique<ParkingSpot>(id, t)); }
    void addObserver(IParkingObserver* o) { observers_.push_back(o); notifyObservers(); }

    // ENTRY
    std::string parkVehicle(const Vehicle& v, int gateId) {
        if (isFull()) { notifyObservers(); throw std::runtime_error("Parking full"); }
        ParkingSpot* spot = findSpot(v);
        if (!spot) throw std::runtime_error("No compatible spot");
        spot->park(v.getNumber());
        std::string tid = "T_" + std::to_string(++ticketCounter_);
        activeTickets_.emplace(tid, Ticket(tid, v.getNumber(), spot->getId(), now()));
        typeByTicket_[tid] = v.getType();
        notifyObservers();
        return tid;
    }

    // EXIT
    double unparkVehicle(const std::string& tid, PaymentChannel ch, PaymentMethod m) {
        auto it = activeTickets_.find(tid);
        if (it == activeTickets_.end()) throw std::runtime_error("Invalid ticket");
        Ticket& t = it->second;
        double fee = pricing_->calculateFee(typeByTicket_[tid], t.getEntryTime(), now());
        auto processor = PaymentFactory::create(ch);           // Factory
        if (!processor->pay(fee, m)) throw std::runtime_error("Payment failed");
        getSpot(t.getSpotId())->vacate();
        activeTickets_.erase(it);
        typeByTicket_.erase(tid);
        notifyObservers();
        return fee;
    }

private:
    static long long now() { return /* epoch seconds */ 0; }
    bool isFull() const {
        for (auto& s : spots_) if (!s->isOccupied()) return false;
        return true;
    }
    // ⭐ preference order: chhota vehicle chhote spot me pehle (LARGE trucks ke liye bache)
    ParkingSpot* findSpot(const Vehicle& v) {
        for (auto& s : spots_) if (s->canFit(v)) return s.get();
        return nullptr;
    }
    ParkingSpot* getSpot(int id) {
        for (auto& s : spots_) if (s->getId() == id) return s.get();
        return nullptr;
    }
    std::map<SpotType, int> availability() const {
        std::map<SpotType, int> a;
        for (auto& s : spots_) if (!s->isOccupied()) a[s->getType()]++;
        return a;
    }
    void notifyObservers() {
        auto a = availability(); bool full = isFull();
        for (auto* o : observers_) o->onUpdate(a, full);
    }
};
```

### Step 7 — Demo (main)
```cpp
int main() {
    ParkingLot lot(std::make_unique<HourlyPricing>());
    lot.addSpot(1, SpotType::COMPACT);
    lot.addSpot(2, SpotType::LARGE);
    DisplayBoard board;
    lot.addObserver(&board);

    Vehicle car("KA01AB1234", VehicleType::CAR);
    std::string ticket = lot.parkVehicle(car, 1);     // spot 1 (compact)
    double fee = lot.unparkVehicle(ticket, PaymentChannel::MACHINE, PaymentMethod::UPI);
    return 0;
}
```

### Walkthrough interviewer ko kya bolna
- "ParkingLot **Facade** hai — client sirf park/unpark jaanta."
- "Pricing **Strategy** hai — weekend/flat pricing add karna aasan (OCP)."
- "Payment **Factory** — naya channel = ek case."
- "DisplayBoard **Observer** — availability badle to auto-update."
- "`unique_ptr` se ownership clear — koi manual delete nahi (Rule of Zero)."
- "Concurrency: multi-threaded me `parkVehicle`/`unparkVehicle` pe mutex chahiye (shared spots)."
- "Extensions: multiple floors (Floor class), monthly pass (different pricing strategy), EV spots."

---

## 16b. Complete worked example — Splitwise (full code)

Splitwise sabse zyada poochi jaane wali problem hai jo repo me nahi. Poora skeleton:

### Requirements
```
- Users, expenses (kaun ne pay kiya, kaun-kaun me split)
- Split types: EQUAL, EXACT (custom), PERCENTAGE
- Balance: "kaun kisko kitna deta hai"
- Simplify debts (transactions minimize) [bonus]
```

### Enums + Models
```cpp
enum class SplitType { EQUAL, EXACT, PERCENTAGE };

class User {
    std::string id_, name_, email_;
public:
    User(std::string id, std::string name) : id_(std::move(id)), name_(std::move(name)) {}
    const std::string& getId() const { return id_; }
};

// Ek user ka ek expense me hissa
struct Split {
    std::string userId;
    double amount;   // is user ne kitna owe kiya
};

class Expense {
    std::string id_;
    std::string paidBy_;      // kisne pay kiya
    double totalAmount_;
    std::vector<Split> splits_;
public:
    Expense(std::string id, std::string paidBy, double amt, std::vector<Split> s)
        : id_(std::move(id)), paidBy_(std::move(paidBy)), totalAmount_(amt), splits_(std::move(s)) {}
    const std::string& getPaidBy() const { return paidBy_; }
    const std::vector<Split>& getSplits() const { return splits_; }
};
```

### Strategy — split calculation
```cpp
class SplitStrategy {
public:
    // total + participants (+ values for exact/percent) -> har user ka share
    virtual std::vector<Split> calculate(double total,
                                         const std::vector<std::string>& users,
                                         const std::vector<double>& values) const = 0;
    virtual ~SplitStrategy() = default;
};

class EqualSplit : public SplitStrategy {
public:
    std::vector<Split> calculate(double total, const std::vector<std::string>& users,
                                 const std::vector<double>&) const override {
        std::vector<Split> result;
        double share = total / users.size();
        for (auto& u : users) result.push_back({u, share});
        return result;
    }
};

class ExactSplit : public SplitStrategy {
public:
    std::vector<Split> calculate(double total, const std::vector<std::string>& users,
                                 const std::vector<double>& values) const override {
        double sum = 0; for (double v : values) sum += v;
        if (std::abs(sum - total) > 0.01) throw std::runtime_error("Exact splits sum != total");
        std::vector<Split> result;
        for (size_t i = 0; i < users.size(); ++i) result.push_back({users[i], values[i]});
        return result;
    }
};

class PercentSplit : public SplitStrategy {
public:
    std::vector<Split> calculate(double total, const std::vector<std::string>& users,
                                 const std::vector<double>& pct) const override {
        double sum = 0; for (double p : pct) sum += p;
        if (std::abs(sum - 100.0) > 0.01) throw std::runtime_error("Percentages != 100");
        std::vector<Split> result;
        for (size_t i = 0; i < users.size(); ++i) result.push_back({users[i], total * pct[i] / 100.0});
        return result;
    }
};
```

### Facade — SplitwiseSystem
```cpp
class SplitwiseSystem {
    std::unordered_map<std::string, User> users_;
    // ⭐ balances_[A][B] = A owes B  (net)
    std::unordered_map<std::string, std::unordered_map<std::string, double>> balances_;
    int expenseCounter_ = 0;

public:
    void addUser(const std::string& id, const std::string& name) {
        users_.emplace(id, User(id, name));
    }

    void addExpense(const std::string& paidBy, double amount,
                    const std::vector<std::string>& participants,
                    SplitType type, const std::vector<double>& values = {}) {
        std::unique_ptr<SplitStrategy> strategy;
        switch (type) {
            case SplitType::EQUAL:      strategy = std::make_unique<EqualSplit>(); break;
            case SplitType::EXACT:      strategy = std::make_unique<ExactSplit>(); break;
            case SplitType::PERCENTAGE: strategy = std::make_unique<PercentSplit>(); break;
        }
        auto splits = strategy->calculate(amount, participants, values);

        // har participant paidBy ko owe karta (apne share ke hisaab se)
        for (auto& s : splits) {
            if (s.userId == paidBy) continue;   // khud ka hissa self ko nahi
            balances_[s.userId][paidBy] += s.amount;   // userId owes paidBy
            // net off reverse direction (agar paidBy pehle userId ko deta tha)
            simplifyPair(s.userId, paidBy);
        }
    }

    // "X owes Y: amount" print
    void showBalances(const std::string& userId) {
        for (auto& [other, amt] : balances_[userId])
            if (amt > 0.01) { /* userId owes other: amt */ }
        for (auto& [other, map] : balances_)
            if (auto it = map.find(userId); it != map.end() && it->second > 0.01) {
                /* other owes userId: it->second */
            }
    }

private:
    // A->B aur B->A dono ho to net off (ek direction rakho)
    void simplifyPair(const std::string& a, const std::string& b) {
        double ab = balances_[a][b], ba = balances_[b][a];
        if (ab > ba) { balances_[a][b] = ab - ba; balances_[b][a] = 0; }
        else         { balances_[b][a] = ba - ab; balances_[a][b] = 0; }
    }
};
```

### ⭐ Debt simplification (transactions minimize — bonus)
```cpp
// Har user ka NET balance nikaalo (+ve = lena hai, -ve = dena hai)
// Phir max-creditor ko max-debtor se settle karo (greedy, min transactions)
void simplifyDebts(std::unordered_map<std::string, double> net) {
    // net: {A: -30, B: +50, C: -20}
    std::priority_queue<std::pair<double,std::string>> creditors, debtors;
    for (auto& [u, amt] : net) {
        if (amt > 0) creditors.push({amt, u});
        else if (amt < 0) debtors.push({-amt, u});
    }
    while (!creditors.empty() && !debtors.empty()) {
        auto [credit, cu] = creditors.top(); creditors.pop();
        auto [debt, du]   = debtors.top();   debtors.pop();
        double settle = std::min(credit, debt);
        // du pays cu: settle  (ek transaction)
        if (credit > settle) creditors.push({credit - settle, cu});
        if (debt > settle)   debtors.push({debt - settle, du});
    }
}
```

### Walkthrough
- "Split type ke liye **Strategy** — naya split (shares/adjustment) = nayi class."
- "`balances_[A][B]` = A owes B — net off karke ek direction rakhta."
- "Debt simplification greedy hai — max creditor + max debtor settle, transactions minimize."
- "Extensions: groups, multi-currency, settlement history, recurring expenses."

---

## 17. Common anti-patterns (kya NAHI karna)

| Anti-pattern | Kya hai | Fix |
|---|---|---|
| **God Object** | ek class sab kuch karti | SRP — services me todo |
| **Anemic Domain Model** | models sirf getter/setter, logic services me | behavior models me daalo |
| **Spaghetti code** | no structure, tight coupling | layers + patterns |
| **Golden Hammer** | har jagah ek hi pattern (Singleton everywhere) | right tool for right job |
| **Premature optimization** | pehle se micro-optimize | pehle correct, phir profile |
| **Magic numbers/strings** | `if (status == 3)` | enums / named constants |
| **Deep inheritance** | 5-level inheritance tree | composition prefer |
| **Circular dependency** | A→B→A | interface / event / reverse |
| **Copy-paste (DRY violation)** | same logic multiple jagah | extract to method/class |
| **Leaky abstraction** | interface implementation detail leak kare | clean interface |
| **Primitive obsession** | sab kuch string/int | value objects (Money, Email) |
| **Long parameter list** | `func(a,b,c,d,e,f,g)` | parameter object / builder |

---

## 18. Quick revision cheat sheet (interview se 1 ghanta pehle padho)

### 23 GoF patterns — ek line each
**Creational (5):**
- Singleton — ek instance, global access
- Factory Method — ek product, creation chhupa
- Abstract Factory — related products ka family
- Builder — complex object step-by-step
- Prototype — clone karke banao

**Structural (7):**
- Facade — subsystem ka simple darwaza
- Decorator — runtime pe behavior wrap
- Adapter — incompatible interface jodo
- Composite — tree (part-whole same)
- Bridge — abstraction + impl alag
- Proxy — access control / lazy / cache
- Flyweight — shared state, memory bachao

**Behavioral (11):**
- Strategy — swappable algorithm
- Observer — badla → notify
- State — behavior state pe depend
- Chain of Responsibility — handlers chain
- Command — request ko object (undo)
- Memento — snapshot save/restore
- Template Method — algo skeleton fix
- Iterator — traverse
- Mediator — central coordinator
- Visitor — operation classes se alag
- Interpreter — grammar (rare)

### SOLID — ek line each
- **S** — ek class, ek badalne ki wajah
- **O** — extension open, modification closed
- **L** — child parent ki jagah, contract tode bina
- **I** — chhote focused interfaces
- **D** — abstraction pe depend, concrete pe nahi

### C++ must-know — ek line each
- Virtual dtor — polymorphic base me zaroori (warna leak)
- RAII — resource = object lifetime
- unique_ptr — exclusive, move-only (default)
- shared_ptr — refcount (multiple owners)
- weak_ptr — circular ref todta
- Rule of 0/3/5 — special members ka set
- Move semantics — data steal, copy nahi
- const correctness — const method modify nahi karta
- Member init order — declaration order (init-list nahi)

### Concurrency — ek line each
- Race condition — shared + no sync + timing-dependent
- mutex + lock_guard — RAII critical section
- Deadlock — circular wait; fix: lock ordering
- atomic — lock-free single variable
- condition_variable — thread signaling (predicate!)
- Optimistic (version check) vs Pessimistic (lock pehle)

### Pattern trigger words (interview me sunte hi)
| Suno | Socho |
|---|---|
| "algorithm swap / multiple ways" | Strategy |
| "object creation conditional" | Factory |
| "notify / subscribe / event" | Observer |
| "runtime behavior add" | Decorator |
| "behavior state pe depend" | State |
| "ek hi instance" | Singleton |
| "handlers chain / levels" | Chain of Responsibility |
| "simple interface to subsystem" | Facade |
| "tree / nested / part-whole" | Composite |
| "undo / redo / queue commands" | Command |
| "snapshot / restore" | Memento |

---

## 19. Glossary (jaldi reference)

| Term | Matlab |
|---|---|
| **Abstraction** | complexity chhupana, essential dikhana |
| **Encapsulation** | data + methods bundle, private state |
| **Polymorphism** | ek interface, kai roop |
| **Coupling** | modules ki inter-dependency (kam chahiye) |
| **Cohesion** | module ke andar relatedness (zyada chahiye) |
| **Invariant** | condition jo hamesha true rahe (balance >= 0) |
| **Idempotency** | same op kai baar = same result |
| **DTO** | Data Transfer Object (sirf data) |
| **DI** | Dependency Injection (deps bahar se) |
| **DIP** | Dependency Inversion Principle |
| **SRP/OCP/LSP/ISP** | SOLID ke principles |
| **YAGNI** | You Ain't Gonna Need It |
| **DRY** | Don't Repeat Yourself |
| **KISS** | Keep It Simple, Stupid |
| **RAII** | Resource Acquisition Is Initialization |
| **vtable/vptr** | virtual dispatch ka mechanism |
| **Saga** | distributed txn with compensating actions |
| **Compensating transaction** | ek step ka undo (reversal) |
| **Snapshot** | historical record (values copy) |
| **Repository** | data access abstraction |
| **Service layer** | business logic layer |
| **Facade** | subsystem ka simple entry |
| **God object** | ek class jo sab kuch karti (anti-pattern) |
| **Anemic model** | logic-less model (anti-pattern) |
| **Fail-fast** | invalid input pe turant throw |
| **Guard clause** | early return/throw for invalid |
| **Deadlock** | circular lock wait |
| **Race condition** | unsynchronized shared access |
| **Thundering herd** | ek saath sab requests (cache miss) |

---

## 20. Mock interview transcript (kaise back-and-forth hota hai)

Ek realistic 45-min LLD interview ka flow. **Dhyan do ki candidate pehle SAWAAL poochta hai.**

> **Interviewer:** "Design a parking lot system."

> **Candidate:** "Sure! Pehle kuch clarify karna chahunga.
> - Kaunse vehicle types support karne hain?
> - Multiple floors ya single?
> - Payment kaise — cash/card, aur kaha (exit gate/machine)?
> - Kya real-time availability display chahiye?
> - Concurrency — multiple gates ek saath cars park kar rahe?"

> **Interviewer:** "Car, bike, truck. Single floor. Card + UPI at exit. Display board haan. Concurrency abhi ignore karo, mention kar dena."

> **Candidate:** "Perfect. To scope: single floor, 3 vehicle types, spot types (bike ke liye
> chhota, car ke liye compact, truck ke liye large), exit pe payment, ek display board.
> Main entities socho — `ParkingLot`, `ParkingSpot`, `Vehicle`, `Ticket`, `Gate`,
> `PricingStrategy`, `PaymentProcessor`, `DisplayBoard`.
> Relationships: `ParkingLot` **has-a** list of `ParkingSpot`; ek `Vehicle` ko ek `Ticket`
> milta jab park hota."

> **Interviewer:** "Spot allocation kaise karoge? Truck kis spot me?"

> **Candidate:** "Har spot ka ek `canFit(vehicle)` method — bike motorcycle-spot me, car
> compact ya large me, truck sirf large me. Main ek **preference order** rakhunga: chhota
> vehicle pehle chhote spot me daalne ki koshish, taaki LARGE spots trucks ke liye bache rahein.
> Ye ek optimization hai, mention kar deta hu."

> **Interviewer:** "Achha. Pricing?"

> **Candidate:** "Pricing ko main **Strategy pattern** banaunga — `PricingStrategy` interface,
> `HourlyPricing` implementation. Kyun? Kyunki kal ko flat/weekend/dynamic pricing aa sakti hai —
> tab sirf nayi Strategy class, `ParkingLot` untouched. Ye Open/Closed principle hai."
> *(candidate code likhta — ParkingSpot, canFit, PricingStrategy)*

> **Interviewer:** "Payment multiple ways se ho sakta — kaise handle?"

> **Candidate:** "`PaymentProcessor` interface, aur ek `PaymentFactory` jo channel (agent/machine)
> se sahi processor banata. Factory + Strategy jodi — naya channel = ek factory case + nayi class."

> **Interviewer:** "Display board real-time update kaise?"

> **Candidate:** "**Observer pattern**. `ParkingLot` subject hai, `DisplayBoard` observer. Jab bhi
> park/unpark ho, `notifyObservers()` call hota — board ko nayi availability milti. Aur email/SMS
> alert bhi observer bana ke add kar sakte, bina `ParkingLot` badle."

> **Interviewer:** "Ab poora park flow dikhao."

> **Candidate:** *(walks through)* "`parkVehicle(vehicle, gate)`: pehle `isFull()` check → full
> to notify + throw. Warna `findSpot(vehicle)` → spot mila to `park()`, ek `Ticket` banao (entry
> time), `activeTickets_` me store, `notifyObservers()`, ticket return."

> **Interviewer:** "Concurrency ka kya — do gates ek saath last spot ko?"

> **Candidate:** "Good point. Multi-threaded me `parkVehicle`/`unparkVehicle` critical section
> hai — do threads same spot allocate kar sakte. Fix: spot allocation pe **mutex** (coarse), ya
> per-spot-type lock (fine-grained) for better parallelism. Ya optimistic — spot pe atomic
> compare-and-set. Main coarse mutex se shuru karunga, phir fine-grained me jaaunga agar
> throughput chahiye."

> **Interviewer:** "Great. Extensions?"

> **Candidate:** "Multiple floors (`Floor` class, `ParkingLot` has floors), monthly pass
> (different pricing strategy), EV charging spots (naya SpotType), reservation system,
> license-plate recognition at entry. Design already extensible hai kyunki Strategy/Factory/
> Observer use kiye."

**⭐ Ye transcript kya sikhata hai:**
1. Candidate ne **pehle 5 sawaal** poochhe (scope pin) — turant code nahi
2. Har design decision ke saath **"kyun"** bola (Strategy kyun, Observer kyun)
3. **Trade-offs khud raise** kiye (preference order, concurrency options)
4. **Extensibility** demonstrate ki (OCP)
5. Interviewer ke concurrency sawaal pe **options diye** (coarse/fine/optimistic), ek best pick

---

## 📚 Sources (web research)
- [InterviewBit — LLD Interview Questions](https://www.interviewbit.com/low-level-design-interview-questions/)
- [InterviewBit — Design Patterns Interview Questions](https://www.interviewbit.com/design-patterns-interview-questions/)
- [GeeksforGeeks — Top Design Patterns Interview Questions](https://www.geeksforgeeks.org/system-design/top-design-patterns-interview-questions/)
- [Indeed — SOLID Principles Interview Questions](https://in.indeed.com/career-advice/interviewing/solid-principles-interview-questions)
- [Indeed — Design Patterns Interview Questions](https://www.indeed.com/career-advice/interviewing/design-patterns-interview-questions)
- [techinterview — C++ Interview Questions 2025](https://www.techinterview.org/post/3233474462/cpp-interview-questions-2025-smart-pointers-move-semantics-raii-templates-stl-concurrency-memory-model-virtual-functions/)
- [Adaface — 90 C++ Interview Questions](https://www.adaface.com/blog/cpp-interview-questions/)
- [MindMajix — Top LLD Interview Questions](https://mindmajix.com/low-level-design-interview-questions)

> 💡 **Combine with `HLD_Interview.md`** — LLD (ye file) ek component ke andar ka design, HLD
> poore system ka. Dono padho, dono interviews me aate hain.

---

## 🎯 Final 60-second strategy (interview room me)

```
┌──────────────────────────────────────────────────────────────────┐
│  1. CLARIFY   (5-10 min) — functional + non-functional + scope    │
│               "Kaunse types? Scale? Concurrency? Ye feature IN?"   │
│                                                                    │
│  2. ENTITIES  — nouns nikaalo, ek class = ek responsibility (SRP)  │
│                                                                    │
│  3. RELATE    — is-a (inherit) vs has-a (compose); ownership pin   │
│                                                                    │
│  4. PATTERNS  — trigger words se (swap→Strategy, create→Factory,   │
│               notify→Observer). Thoop mat, justify karo.           │
│                                                                    │
│  5. CODE      — interfaces pehle → models → services → facade      │
│               → ek flow end-to-end walk-through                    │
│                                                                    │
│  6. EXTEND    — "naya feature = nayi class" (OCP), concurrency,    │
│               trade-offs KHUD bolo                                 │
└──────────────────────────────────────────────────────────────────┘
```

**Yaad rakho:** interviewer perfect code nahi, **clear thinking + good abstractions +
communication** dekhta hai. Bolte raho, sawaal poochte raho, trade-offs batao. All the best! 🚀

**Ab is repo ke folders ka asli code padho** — theory yahan hai, practice wahan. Har folder me
`design_patterns_used.md` (patterns) + `design_diagram.md` (UML) + detailed commented code hai.







