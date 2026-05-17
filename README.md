# Low-Level Design (LLD) — Complete Master Repository Guide

<p align="center">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=24&duration=2500&pause=800&color=2F81F7&center=true&vCenter=true&width=900&lines=Master+LLD+with+Code-First+Practice;40+Lessons+%7C+18+System+Projects;Design+Patterns+%2B+Interview+Case+Studies" alt="Typing animation" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge" alt="C++ Badge" />
  <img src="https://img.shields.io/badge/Lessons-L1%E2%80%93L40-6f42c1?style=for-the-badge" alt="Lessons badge" />
  <img src="https://img.shields.io/badge/System+Projects-18%2B-success?style=for-the-badge" alt="Projects badge" />
  <img src="https://img.shields.io/badge/Focus-Interview%20Ready-orange?style=for-the-badge" alt="Interview badge" />
</p>

> Har folder, har lesson, har system project — structure, concepts, patterns, compile commands, aur interview tips — sab yahan detail me documented hai.

---

## Table of Contents

1. [Repository Overview](#repository-overview)
2. [Repository Statistics](#repository-statistics)
3. [Why This Repository Exists](#why-this-repository-exists)
4. [How To Use This Repository](#how-to-use-this-repository)
5. [Recommended Learning Roadmap](#recommended-learning-roadmap)
6. [Repository Layout At Root](#repository-layout-at-root)
7. [Foundation Track — L1 to L6](#foundation-track--l1-to-l6)
8. [Pattern & System Track — L7 to L20](#pattern--system-track--l7-to-l20)
9. [Advanced Patterns & Systems — L21 to L40](#advanced-patterns--systems--l21-to-l40)
10. [Standalone System Implementations](#standalone-system-implementations)
11. [Multi-Threading Module](#multi-threading-module)
12. [Design Pattern Coverage Matrix](#design-pattern-coverage-matrix)
13. [Interview Preparation Playbook](#interview-preparation-playbook)
14. [Compilation & Run Guide](#compilation--run-guide)
15. [Standard Project Structure Convention](#standard-project-structure-convention)
16. [Reference Materials In Repo](#reference-materials-in-repo)
17. [Extension Exercises](#extension-exercises)
18. [Quick Navigation Index](#quick-navigation-index)
19. [Glossary](#glossary)

---

## Repository Overview

Yeh repository **Low-Level Design (LLD)** seekhne ke liye banayi gayi hai — theory se zyada **working C++ code** par focus ke saath. Isme:

| Category               | Count        | Description                                                                |
| ---------------------- | ------------ | -------------------------------------------------------------------------- |
| **Lesson modules**     | L1 – L40     | OOP, SOLID, UML, har major design pattern, aur pattern + system hybrids    |
| **Standalone systems** | 19 projects  | Interview-favourite systems + LRU/LFU caches |
| **Concurrency labs**   | 1 folder     | `Multi_threading_C++` — threads, mutex, semaphores, thread pool, DCLP      |
| **Reference assets**   | PDFs, images | Gang of Four, DDIA, design pattern cheat sheets                            |

**Core philosophy:** Pehle problem statement padho → requirements samjho → classes/interfaces design karo → code likho → phir 2–3 extensions khud try karo.

**Primary language:** C++17 (header-heavy style, in-memory storage, demo `main.cpp` drivers).

---

## Repository Statistics

| Metric                               | Approximate value       |
| ------------------------------------ | ----------------------- |
| Total C++/header source files        | ~490+                   |
| Lesson folders (L1–L40)              | 40                      |
| Runnable system demos                | 19+                     |
| Design patterns demonstrated         | 20+ (GoF + Null Object) |
| Projects with `problem_statement.md` | 26+                     |
| Projects with dedicated `README.md`  | 19                      |
| UML class + sequence diagrams        | [`SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md`](./SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md) |

---

## Why This Repository Exists

Zyada tar LLD preparation in points par atak jaati hai:

| Problem                                           | Is repo ka solution                                                    |
| ------------------------------------------------- | ---------------------------------------------------------------------- |
| Theory samajh aata hai, code me convert nahi hota | Har concept ke saath runnable `.cpp` / modular headers                 |
| Code likh lete hain, extensibility weak hoti hai  | SOLID + OCP-focused refactors (e.g. L7 Bad vs Good design)             |
| Sirf 1–2 problems solve kiye                      | 18+ full systems + 10+ lesson-level LLDs                               |
| Patterns yaad hain, use-case nahi                 | Har pattern ke saath real domain (ATM, Food Delivery, Payment Gateway) |
| Interview me sirf code, design explain nahi       | Har project me assumptions, trade-offs, extension hooks                |

**4 learning layers:**

```
┌─────────────────────────────────────────────────────────────┐
│  Layer 4: Interview Layer    — trade-offs, scale, extensions │
├─────────────────────────────────────────────────────────────┤
│  Layer 3: System Layer       — Parking, Uber, WhatsApp...   │
├─────────────────────────────────────────────────────────────┤
│  Layer 2: Pattern Layer      — Strategy, Factory, Observer.. │
├─────────────────────────────────────────────────────────────┤
│  Layer 1: Foundation Layer   — OOP, UML, SOLID               │
└─────────────────────────────────────────────────────────────┘
```

---

## How To Use This Repository

### Per-lesson workflow (recommended)

1. **`problem_statement.md` / `requirements.md`** padho (agar hai)
2. **5–10 min** whiteboard par classes draw karo (UML optional — L4 se help milegi)
3. **Code explore karo** — pehle `core/`, phir `models/`, phir `services/`
4. **`main.cpp` run karo** — output se flow samjho
5. **3 interview questions** khud ke words me answer likho:
   - Kya assumptions liye?
   - Kaunsa pattern sabse zyada value de raha hai?
   - Agar 10x users aaye to kya break hoga?

### Per-system-project workflow

1. Requirements → entities list → enums → services → facade/core
2. Pattern identify karo (Strategy? Factory? Observer?)
3. Ek feature **khud add** karo (e.g. Parking Lot me monthly pass pricing)

### Code style notes (repo-wide)

- Namespaces per project (`namespace ParkingLot`, etc.)
- `std::unordered_map` for in-memory DB simulation
- Validation via `std::runtime_error` / exceptions
- Smart pointers kuch projects me (`unique_ptr`), kuch me raw pointers (L18 notes mention this intentionally)

---

## Recommended Learning Roadmap

### Phase 1 — Foundations (Week 1–2)

| Order | Module                              | Focus                           |
| ----- | ----------------------------------- | ------------------------------- |
| 1     | [L1 Introduction](#l1-introduction) | LLD mindset, course orientation |
| 2     | [L2 OOPS_1](#l2-oops_1)             | Encapsulation, Abstraction      |
| 3     | [L3 OOPS_2](#l3-oops_2)             | Inheritance, Polymorphism       |
| 4     | [L4 UML_Diagrams](#l4-uml_diagrams) | Class diagrams, relationships   |
| 5     | [L5 SOLID_1](#l5-solid_1)           | SRP, OCP, LSP                   |
| 6     | [L6 SOLID_2](#l6-solid_2)           | ISP, DIP, LSP rules deep-dive   |

### Phase 2 — Core Patterns (Week 3–5)

| Order | Module                  | Pattern / Topic                             |
| ----- | ----------------------- | ------------------------------------------- |
| 7     | L7 Document Editor      | Strategy + Composite elements               |
| 8     | L8 Strategy             | Strategy (Robot behaviors)                  |
| 9     | L9 Factory              | Simple / Factory Method / Abstract Factory  |
| 10    | L10 Singleton           | Thread-safe variants                        |
| 11    | L11 Food Delivery       | Facade + Factory + Strategy (full LLD)      |
| 12    | L12 Observer            | Observer                                    |
| 13    | L13 Decorator           | Decorator                                   |
| 14    | L14 Notification Engine | Singleton + Decorator + Observer + Strategy |

### Phase 3 — Structural & Behavioral (Week 6–8)

L15 Command → L21 Proxy → L31 Splitwise → L37 Chess (see full list below)

### Phase 4 — System Projects (Week 9–12)

Priority interview list:

1. [Parking Lot](#parking_lot_system_lld)
2. [BookMyShow / Movie Ticket](#movie_ticket_booking_system)
3. [LRU Cache](#lru_cache_lld) + [Rate Limiter](#rate_limiter_lld)
4. [Elevator](#elevator_system_lld)
5. [Splitwise](#l31-splitwise_lld) (lesson) + [Uber](#uber_lld)
6. [WhatsApp](#whatsapp_lld)
7. [Load Balancer](#loadbalancer_lld)
8. [Logger](#logger_lld)

---

## Repository Layout At Root

```
LLD/
├── L1 Introduction/ … L40 Null_object_pattern_and_Antipatterns/   # 40 lessons
├── ATM_LLD/ … WhatsApp_LLD/ … LRU_Cache_LLD/                      # 18 systems
├── Multi_threading_C++/                                             # Concurrency labs
├── Design_Pattern_types.md                                          # Pattern taxonomy (Hindi)
├── Design_Pattern_Sheet_1.webp, Design_Pattern_Sheet_2.webp         # Cheat sheets
├── Gang of Four.pdf                                                 # GoF reference
├── Designing Data-Intensive Applications…pdf                        # DDIA (systems thinking)
├── imp_design_patterns.png, imp_problems.png, Some_patterns.png     # Visual aids
└── README.md                                                        # ← You are here
```

---

## Foundation Track — L1 to L6

### L1 Introduction

|                   |                                                                    |
| ----------------- | ------------------------------------------------------------------ |
| **Path**          | [`L1 Introduction/`](./L1%20Introduction/)                         |
| **Files**         | `Notes.pdf`                                                        |
| **Concepts**      | LLD introduction, interview expectations, design thinking overview |
| **Code**          | None — theory/notes only                                           |
| **Start here if** | Bilkul naye ho LLD me                                              |

---

### L2 OOPS_1

|                    |                                                                        |
| ------------------ | ---------------------------------------------------------------------- |
| **Path**           | [`L2 OOPS_1/`](./L2%20OOPS_1/)                                         |
| **Files**          | `C++ Code/Abstraction.cpp`, `Encapsulation.cpp`                        |
| **Concepts**       | Data hiding, getters/setters, abstract classes, pure virtual functions |
| **Example domain** | `Car` / `SportsCar` hierarchy                                          |
| **Key takeaway**   | Abstraction = _what_; implementation = _how_ (hidden)                  |

**Encapsulation demo:** `SportsCar` with private `brand`, `speed`, `tyreCompany` — public methods se controlled access.

**Abstraction demo:** Abstract `Car` with `startEngine()`, `accelerate()` — `SportsCar` implements; base pointer se polymorphic use.

---

### L3 OOPS_2

|                  |                                                                                                            |
| ---------------- | ---------------------------------------------------------------------------------------------------------- |
| **Path**         | [`L3 OOPS_2/`](./L3%20OOPS_2/)                                                                             |
| **Files**        | `Inheritance.cpp`, `StaticPolymorphism.cpp`, `DynamicPolymorphism.cpp`, `StaticAndDynamicPolymorphism.cpp` |
| **Concepts**     | IS-A relationships, method overloading (compile-time), virtual overrides (runtime)                         |
| **Key takeaway** | Static = compile-time binding; Dynamic = vtable/runtime dispatch                                           |

---

### L4 UML_Diagrams

|                |                                                                                                              |
| -------------- | ------------------------------------------------------------------------------------------------------------ |
| **Path**       | [`L4 UML_Diagrams/`](./L4%20UML_Diagrams/)                                                                   |
| **Files**      | `4.txt`, `inheritance.cpp`, `composition.cpp`, `composition_old_style_ptr.cpp`, `example_of_composition.cpp` |
| **Concepts**   | Structural vs behavioral UML; `+` `#` `-` visibility; association, aggregation, composition, inheritance     |
| **Modern C++** | Composition with `std::unique_ptr` (RAII, exception safety)                                                  |
| **Examples**   | `Chair` composed of `Seat`, `Arms`, `Wheels`; multiple inheritance types in `inheritance.cpp`                |

**Interview tip:** Composition > inheritance jab "has-a" ho aur lifetime dependent ho.

---

### L5 SOLID_1

|                |                                  |
| -------------- | -------------------------------- |
| **Path**       | [`L5 SOLID_1/`](./L5%20SOLID_1/) |
| **Principles** | SRP, OCP, LSP (part 1)           |
| **Domain**     | E-commerce shopping cart         |

| Principle | Violated file                                          | Followed file                              | Lesson                         |
| --------- | ------------------------------------------------------ | ------------------------------------------ | ------------------------------ |
| **SRP**   | `SRP_violated.cpp` — cart + DB + invoice               | `SRP_followed.cpp` — split classes         | Ek class = ek reason to change |
| **OCP**   | `OCP_violated.cpp` — if/else discounts                 | `OCP_followed.cpp` — `Discount` hierarchy  | Extend without modifying cart  |
| **LSP**   | `LSP_Violated.cpp` — `FixedDeposit::withdraw()` throws | `LSP_followed.cpp` — segregated interfaces | Subtypes must be substitutable |

Also: `LSP_followed_wrongly.cpp` — subtle LSP mistake (worth studying).

---

### L6 SOLID_2

|                |                                                                                     |
| -------------- | ----------------------------------------------------------------------------------- |
| **Path**       | [`L6 SOLID_2/`](./L6%20SOLID_2/)                                                    |
| **Principles** | ISP, DIP, LSP formal rules                                                          |
| **ISP**        | Fat `Shape` with `volume()` → 2D shapes throw; fix: `Shape2D` / `Shape3D`           |
| **DIP**        | `UserService` depends on `IDatabase`, not `MySQL` directly                          |
| **LSP Rules**  | `PreConditions.cpp`, `PostConditions.cpp`, `HistoryConstraint.cpp`, signature rules |

**DIP files:** `DIP_violated.cpp`, `DIP_followed.cpp`, `DIP_followed_new_pointer_style.cpp`

---

## Pattern & System Track — L7 to L20

### L7 Document_Editor_LLD

|               |                                                          |
| ------------- | -------------------------------------------------------- |
| **Path**      | [`L7 Document_Editor_LLD/`](./L7%20Document_Editor_LLD/) |
| **Patterns**  | **Strategy** (persistence), **Composite-like** elements  |
| **Structure** | `BadDesign/` vs `GoodDesign/` vs smart-pointer variant   |

**Problem:** Multiple document elements (text, image, bold, tab); multiple storage backends (file, DB); avoid OCP violations.

**Good design classes:**

- `DocumentElement` — `render()`
- `TextElement`, `ImageElement`, `BoldTextElement`, `NewLineElement`, `TabElement`
- `Persistence` — `FileStorage`, `DBStorage`
- `DocumentEditor` — orchestrates render + save

**Workflow:** Build elements → `render()` loop → inject storage → `save()`

**Interview angle:** Bad design uses string suffix checks (`.jpg`) — why that's fragile.

---

### L8 Strategy_Design_Patterns

|             |                                                                    |
| ----------- | ------------------------------------------------------------------ |
| **Path**    | [`L8 Strategy_Design_Patterns/`](./L8%20Strategy_Design_Patterns/) |
| **Pattern** | **Strategy** (behavioral)                                          |
| **File**    | `C++ Code/StrategyDesignPattern.cpp`, `problem.md`                 |
| **Domain**  | Robots with pluggable walk/talk/fly behaviors                      |

**Classes:** `WalkableRobot`, `TalkableRobot`, `FlyableRobot` strategies; `CompanionRobot`, `WorkerRobot` contexts.

**Key idea:** Composition over inheritance — runtime `setFlyBehavior()` possible.

---

### L9 Factory_Design_Pattern

|              |                                                                                |
| ------------ | ------------------------------------------------------------------------------ |
| **Path**     | [`L9 Factory_Design_Pattern/`](./L9%20Factory_Design_Pattern/)                 |
| **Variants** | Simple Factory, Factory Method, Abstract Factory                               |
| **Files**    | `SimpleFactory.cpp`, `FactoryMethod.cpp`, `AbstractFactory.cpp`, `Markdown.md` |
| **Domain**   | Burger shop — `SinghBurger` vs `KingBurger`, wheat vs normal combos            |

| Type             | When to use                  | OCP    |
| ---------------- | ---------------------------- | ------ |
| Simple Factory   | Small, centralized creation  | Weak   |
| Factory Method   | Subclass decides product     | Strong |
| Abstract Factory | Coordinated product families | Strong |

---

### L10 Singleton_Design_Pattern

|           |                                                                                                                                                    |
| --------- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Path**  | [`L10 Singleton_Design_Pattern/`](./L10%20Singleton_Design_Pattern/)                                                                               |
| **Files** | `NoSingleton.cpp`, `SimpleSingleton.cpp`, `ThreadSafeEagerSingleton.cpp`, `ThreadSafeLockingSingleton.cpp`, `ThreadSafeDoubleLockingSingleton.cpp` |

| Variant                | Trade-off                                       |
| ---------------------- | ----------------------------------------------- |
| Eager                  | Simple, always initialized at startup           |
| Mutex on every call    | Safe, slower                                    |
| Double-checked locking | Optimized lazy init — subtle correctness issues |

**Interview warning:** Singleton = global state; testability suffers; use when truly one instance needed (config, logger).

---

### L11 Food_Delivery_LLD

|              |                                                        |
| ------------ | ------------------------------------------------------ |
| **Path**     | [`L11 Food_Delivery_LLD/`](./L11%20Food_Delivery_LLD/) |
| **App name** | "Tomato" — Zomato/Swiggy-style                         |
| **Patterns** | **Facade**, **Factory**, **Strategy**                  |

**Folder structure:**

```
C++ Code/Tomato/
├── TomatoApp.h          # Facade
├── models/              # User, Restaurant, Cart, Order, DeliveryOrder, PickupOrder
├── managers/            # RestaurantManager, OrderManager
├── strategies/          # PaymentStrategy (UPI, CreditCard)
├── factories/           # NowOrderFactory, ScheduledOrderFactory
├── services/            # NotificationService
└── utils/               # TimeUtils
```

**Functional requirements:** Delivery/pickup, now/scheduled orders, multiple payment methods, restaurant search, cart, notifications.

---

### L12 Observer_Design_Pattern

|             |                                                                    |
| ----------- | ------------------------------------------------------------------ |
| **Path**    | [`L12 Observer_Design_Pattern/`](./L12%20Observer_Design_Pattern/) |
| **Pattern** | **Observer**                                                       |
| **Domain**  | YouTube channel — subscribe, unsubscribe, notify on new video      |
| **Classes** | `IChannel`, `Channel`, `ISubscriber`, concrete subscribers         |

**vs Polling:** Observer push model avoids wasted checks.

---

### L13 Decorator_Design_Pattern

|                         |                                                                              |
| ----------------------- | ---------------------------------------------------------------------------- |
| **Path**                | [`L13 Decorator_Design_Pattern/`](./L13%20Decorator_Design_Pattern/)         |
| **Pattern**             | **Decorator**                                                                |
| **Domain**              | Mario power-ups — `HeightUp`, `GunPowerUp`, `StarPowerUp` stacked on `Mario` |
| **Why not inheritance** | Combinatorial explosion — 2^n subclasses for n features                      |

---

### L14 Notification_Engine_LLD

|                         |                                                                                                           |
| ----------------------- | --------------------------------------------------------------------------------------------------------- |
| **Path**                | [`L14 Notification_Engine_LLD/`](./L14%20Notification_Engine_LLD/)                                        |
| **Patterns**            | **Singleton**, **Decorator**, **Observer**, **Strategy**                                                  |
| **Dual implementation** | Monolithic `C++ Code/` + modular [`notification_lld/`](./L14%20Notification_Engine_LLD/notification_lld/) |

**Channels:** Email, SMS, Popup  
**Decorators:** Timestamp, Signature  
**Modular structure:**

```
notification_lld/
├── core/NotificationSystem.h
├── enums/ChannelType.h
├── models/, services/, rules/
├── main.cpp
└── README.md
```

**Thread-safe variant:** `dclp_multithreading_safe_notification_system.cpp`

**Requirements summary:** Pluggable channels, decorate messages, strategy-based delivery, SOLID, extensible.

---

### L15 Command_Design_Pattern

|             |                                                                                   |
| ----------- | --------------------------------------------------------------------------------- |
| **Path**    | [`L15 Command_Design_Pattern/`](./L15%20Command_Design_Pattern/)                  |
| **Pattern** | **Command**                                                                       |
| **Domain**  | Home automation remote — `LightCommand`, `FanCommand` with `execute()` / `undo()` |
| **Roles**   | Command, Receiver, Invoker (`RemoteControl`)                                      |

**Use cases:** Undo/redo, macro commands, queue requests.

---

### L16 Adapter_Design_Pattern

|             |                                                                              |
| ----------- | ---------------------------------------------------------------------------- |
| **Path**    | [`L16 Adapter_Design_Pattern/`](./L16%20Adapter_Design_Pattern/)             |
| **Pattern** | **Adapter** (object adapter)                                                 |
| **Domain**  | `XmlDataProvider` (legacy) → `XmlDataProviderAdapter` exposes JSON to client |
| **File**    | `AdpaterPattern.cpp` (note spelling in filename)                             |

---

### L17 Facade_Design_Pattern

|             |                                                                                                   |
| ----------- | ------------------------------------------------------------------------------------------------- |
| **Path**    | [`L17 Facade_Design_Pattern/`](./L17%20Facade_Design_Pattern/)                                    |
| **Pattern** | **Facade**                                                                                        |
| **Domain**  | Computer boot — `PowerSupply`, `CPU`, `Memory`, `BIOS`, `HardDrive` via `ComputerFacade::start()` |
| **Also**    | `Principle_of_least_knowledge.md` — Law of Demeter                                                |

---

### L18 Spotify_LLD

|              |                                                                   |
| ------------ | ----------------------------------------------------------------- |
| **Path**     | [`L18 Spotify_LLD/`](./L18%20Spotify_LLD/)                        |
| **Patterns** | **Facade**, **Singleton**, **Strategy**, **Adapter**, **Factory** |

**Architecture:**

```
MusicPlayerApplication/
├── MusicPlayerFacade.h
├── core/AudioEngine.h
├── managers/PlaylistManager, DeviceManager, StrategyManager
├── strategies/Sequential, Random, CustomQueue
├── device/*Adapter.h  →  external/*API.h
└── factories/DeviceFactory.h
```

**Features:** Songs, playlists, play/pause/resume, device switching (Bluetooth/wired/headphones), play order strategies.

**Known constraints (from docs):** Raw pointers in demo; persistence not implemented — good extension exercise.

---

### L19 Composite_Design_Pattern

|             |                                                                                             |
| ----------- | ------------------------------------------------------------------------------------------- |
| **Path**    | [`L19 Composite_Design_Pattern/`](./L19%20Composite_Design_Pattern/)                        |
| **Pattern** | **Composite**                                                                               |
| **Domain**  | File system — `File` (leaf), `Folder` (composite), uniform `ls()`, `getSize()`, `openAll()` |

---

### L20 Template_Method_Pattern

|                |                                                                                                                     |
| -------------- | ------------------------------------------------------------------------------------------------------------------- |
| **Path**       | [`L20 Template_Method_Pattern/`](./L20%20Template_Method_Pattern/)                                                  |
| **Pattern**    | **Template Method**                                                                                                 |
| **Domain**     | ML training pipeline — `ModelTrainer::trainPipeline()` fixed; subclasses override `trainModel()`, `evaluateModel()` |
| **Subclasses** | `NeuralNetworkTrainer`, `RandomForestTrainer`                                                                       |

---

## Advanced Patterns & Systems — L21 to L40

### L21 Proxy_Design_Pattern

| Variant          | File                  | Use case                     |
| ---------------- | --------------------- | ---------------------------- |
| Virtual Proxy    | `VirtualProxy.cpp`    | Lazy image loading           |
| Protection Proxy | `ProtectionProxy.cpp` | Premium document access      |
| Remote Proxy     | `RemoteProxy.cpp`     | Remote data service stand-in |

---

### L22 Chain_of_responsiblity_patten(ATM LLD)

|             |                                                           |
| ----------- | --------------------------------------------------------- |
| **Pattern** | **Chain of Responsibility**                               |
| **Domain**  | ATM cash dispensing — ₹1000 → ₹500 → ₹200 → ₹100 handlers |
| **File**    | `C++ Code/COR.cpp`                                        |

Note: Yeh full ATM LLD nahi hai — pattern demo hai. Full ATM ke liye [`ATM_LLD/`](#atm_lld).

---

### L23 Payment_gateway_system_LLD

|              |                                                                          |
| ------------ | ------------------------------------------------------------------------ |
| **Path**     | [`L23 Payment_gateway_system_LLD/`](./L23%20Payment_gateway_system_LLD/) |
| **Patterns** | Template Method, Strategy, Proxy, Factory, Singleton                     |

**Flow:** Validate → initiate → confirm payment

| Component                         | Role                                          |
| --------------------------------- | --------------------------------------------- |
| `PaymentGateway`                  | Template Method skeleton — `processPayment()` |
| `PaytmGateway`, `RazorpayGateway` | Provider-specific steps                       |
| `BankingSystem`                   | Strategy — different banking backends         |
| `PaymentGatewayProxy`             | Retry on failure                              |
| `GatewayFactory`                  | Creates proxied gateways                      |
| `PaymentController`               | Single entry `handlePayment()`                |

---

### L24 Discount_coupon_engine_LLD

|              |                                                                          |
| ------------ | ------------------------------------------------------------------------ |
| **Path**     | [`L24 Discount_coupon_engine_LLD/`](./L24%20Discount_coupon_engine_LLD/) |
| **Patterns** | Strategy, Chain of Responsibility, Singleton                             |
| **File**     | `C++ Code/DiscountCoupon.cpp` (monolithic but complete)                  |

**Features:** Flat/percent/capped discounts, coupon chains, cart offers (seasonal, loyalty, bulk, banking), thread-safe coupon registry.

---

### L25 Bridge_design_pattern

|             |                                                                                |
| ----------- | ------------------------------------------------------------------------------ |
| **Pattern** | **Bridge**                                                                     |
| **Domain**  | `Car` abstraction × `Engine` implementation — avoid `SedanPetrolCar` explosion |
| **File**    | `C++ Code/BridgePattern.cpp`                                                   |

---

### L26 Blinkit_LLD

|          |                                            |
| -------- | ------------------------------------------ |
| **Path** | [`L26 Blinkit_LLD/`](./L26%20Blinkit_LLD/) |
| **Type** | Quick commerce / dark store delivery       |

**Components:**

- `DarkStore`, `DarkStoreManager` — geo-based store selection
- `InventoryManager`, `InMemoryInventoryStore` — reserve/deduct stock
- `OrderManager` — state machine: PLACED → … → DELIVERED / CANCELLED
- `ProductFactory`, dynamic delivery fee (base + distance + surge)

**Also:** `C++ Code/ZeptoClone.cpp` — alternate implementation

---

### L27 Tinder_LLD

|              |                                          |
| ------------ | ---------------------------------------- |
| **Path**     | [`L27 Tinder_LLD/`](./L27%20Tinder_LLD/) |
| **Patterns** | Facade (`TinderSystem`), service layer   |

**Features:** Profile discovery by distance, swipes (left/right/super-like), mutual match → chat, daily swipe limits, block, messaging.

**Key classes:** `TinderSystem`, `MatchingService`, `User`, `ChatRoom`, `Message`

---

### L28 Builder_design_pattern

| Files                     | Topic                               |
| ------------------------- | ----------------------------------- |
| `WithoutBuilder.cpp`      | Telescoping constructor problem     |
| `BuilderPattern.cpp`      | Fluent `HttpRequestBuilder`         |
| `BuilderWithDirector.cpp` | Director orchestrates steps         |
| `StepBuilder.cpp`         | Type-state builder (enforced order) |

---

### L29 Iterator_design_pattern

Custom iterators for linked list, binary tree (in-order), playlist — uniform `Iterator<T>` / `Iterable<T>` interface.

---

### L30 Flyweight_design_pattern

|             |                                                                              |
| ----------- | ---------------------------------------------------------------------------- |
| **Compare** | `WithFlyWeight.cpp` vs `WithoutFlyWeight.cpp`                                |
| **Domain**  | Asteroids — shared intrinsic state (texture), extrinsic (position, velocity) |

---

### L31 Splitwise_LLD

|              |                                                |
| ------------ | ---------------------------------------------- |
| **Path**     | [`L31 Splitwise_LLD/`](./L31%20Splitwise_LLD/) |
| **Patterns** | Facade, Singleton, Strategy, Factory, Observer |

**Problem statement summary:**

- Users, groups, expenses
- Split types: equal, exact, percentage
- Balance sheets, settlements
- **Debt simplification** — minimize transactions (`DebtSimplifier`)
- Observer notifications to group members

**Key classes:** `Splitwise`, `Group`, `Expense`, `Split`, `SplitStrategy`, `SplitFactory`

---

### L32 State_design_pattern

Formal **State pattern** — vending machine: `NoCoin` → `HasCoin` → `Dispense` → `SoldOut`.  
(Compare with Blinkit L26 which uses order **state enum** — different abstraction level.)

---

### L33 Tic_Tac_Toe_LLD

|              |                                                                |
| ------------ | -------------------------------------------------------------- |
| **Patterns** | Strategy (rules), Observer (notifications), Factory            |
| **Features** | Configurable board, pluggable win/draw validation, game events |

**Classes:** `TicTacToeGame`, `StandardTicTacToeRules`, `TicTacToeGameFactory`, `ConsoleNotifier`

---

### L34 Snake_ladder_LLD

|              |                                                                                            |
| ------------ | ------------------------------------------------------------------------------------------ |
| **Patterns** | Strategy, **Bridge**, Factory, Observer                                                    |
| **Features** | N×N board, snakes/ladders, dice, exact-roll win, setup strategies (standard/random/custom) |

**Bridge:** `BoardSetupBridge` — `Board::setupBoard(strategy)`

---

### L35 Mediator_design_pattern

Chat room mediator — broadcast, private message, mute. `WithoutMediator.cpp` shows tightly coupled alternative.

---

### L36 Prototype_design_pattern

Clone expensive `NPC` templates instead of reconstructing — `PrototypePattern.cpp` vs `WithoutPrototype.cpp`.

---

### L37 Chess_LLD

|              |                                        |
| ------------ | -------------------------------------- |
| **Path**     | [`L37 Chess_LLD/`](./L37%20Chess_LLD/) |
| **Patterns** | Singleton, Strategy, Mediator, Factory |

**Features:** 8×8 board, all pieces, legal moves, check/checkmate/stalemate, matchmaking by score, in-game chat.

| Class              | Role                                    |
| ------------------ | --------------------------------------- |
| `GameManager`      | Singleton — queue users, create matches |
| `Match`            | Gameplay + `ChatMediator`               |
| `ChessRules`       | Move validation, checkmate detection    |
| `PieceFactory`     | Piece creation                          |
| `MatchingStrategy` | Score-based opponent pairing            |

---

### L38 Visitor_design_pattern

File system elements (`TextFile`, `ImageFile`, `VideoFile`) + visitors for operations like size calculation — add ops without changing elements.

---

### L39 Memento_design_pattern

Database transaction metaphor — `Database` (originator), `DatabaseMemento`, `TransactionManager` (caretaker) — commit/rollback.

---

### L40 Null_object_pattern_and_Antipatterns

|             |                                          |
| ----------- | ---------------------------------------- |
| **Content** | `Notes.pdf` only — no C++ code in repo   |
| **Topics**  | Null Object pattern, common antipatterns |

---

## Standalone System Implementations

Har system project typically contain karta hai:

```
ProjectName/
├── core/           # Orchestrator / Facade
├── models/         # Domain entities
├── services/       # Business logic
├── enums/          # Type-safe constants
├── strategies/     # (optional) pluggable algorithms
├── factories/      # (optional) object creation
├── main.cpp        # Runnable demo
├── problem_statement.md
└── requirements.md
```

---

### ATM_LLD

|              |                                                                                |
| ------------ | ------------------------------------------------------------------------------ |
| **Path**     | [`ATM_LLD/`](./ATM_LLD/)                                                       |
| **Patterns** | Facade, Service layer                                                          |
| **Flow**     | `addAccount` → `addCard` → `login(card, pin)` → balance/withdraw → `logout()`  |
| **Services** | `AuthenticationService`, `CashDispenser` (greedy note dispensing: 500/200/100) |

```bash
cd ATM_LLD && g++ -std=c++17 main.cpp -o atm_app && ./atm_app
```

---

### Car_Rental_System_LLD

Vehicle booking lifecycle: RESERVED → RENTED → AVAILABLE. `PricingService` calculates `days × dailyRate`.

```bash
cd Car_Rental_System_LLD && g++ -std=c++17 main.cpp -o car_rental_app && ./car_rental_app
```

---

### Elevator_System_LLD

Multi-elevator simulation with external (UP/DOWN) and internal floor requests. `ElevatorScheduler` assigns nearest car; `runOneTick()` advances simulation.

```bash
cd Elevator_System_LLD && g++ -std=c++17 main.cpp -o elevator_app && ./elevator_app
```

---

### JSON_Parser_LLD

Recursive-descent JSON parser building **Composite** tree: `JsonObject`, `JsonArray`, primitives. `JsonParser::parse()` → `JsonValue*` → `print()`.

```bash
cd JSON_Parser_LLD && g++ -std=c++17 main.cpp -o json_parser_app && ./json_parser_app
```

---

### Library_Management_System_LLD

Catalog, members, issue/return, borrow limits, late fines via `FineService`.

```bash
cd Library_Management_System_LLD && g++ -std=c++17 main.cpp -o library_app && ./library_app
```

---

### Linkedin_LLD

Users, connection requests (pending/accepted), posts, likes, personalized feed from connections via `FeedService`.

```bash
cd Linkedin_LLD && g++ -std=c++17 main.cpp -o linkedin_app && ./linkedin_app
```

---

### LoadBalancer_LLD

|              |                                                                     |
| ------------ | ------------------------------------------------------------------- |
| **Pattern**  | **Strategy** — `RoundRobinStrategy`, `LeastConnectionsStrategy`     |
| **Features** | Server health (UP/DOWN), connection tracking, runtime strategy swap |

```bash
cd LoadBalancer_LLD && g++ -std=c++17 main.cpp -o load_balancer_app && ./load_balancer_app
```

---

### Logger_LLD

|                 |                                                                                     |
| --------------- | ----------------------------------------------------------------------------------- |
| **Patterns**    | **Singleton**, **Chain of Responsibility**, **Observer**, **Strategy** (formatters) |
| **Entry point** | `Main.cpp` (capital M)                                                              |

**Chain:** `DebugHandler` → `InfoHandler` → … → `FatalHandler`  
**Appenders:** Console, File  
**Formatters:** Plain text, JSON

```bash
cd Logger_LLD && g++ -std=c++17 Main.cpp -o logger_app && ./logger_app
```

---

### Movie_Ticket_Booking_System

BookMyShow-style: movies, shows, seat selection, pricing by seat type, payment gateway, notifications.

| Component                                         | Role              |
| ------------------------------------------------- | ----------------- |
| `MovieTicketSystem`                               | Facade            |
| `CatalogManager`, `ShowManager`, `BookingManager` | Domain managers   |
| `PricingStrategy`                                 | Strategy          |
| `BookingFactory`                                  | Factory           |
| `KioskTerminal`                                   | Device entry demo |

```bash
cd Movie_Ticket_Booking_System && g++ -std=c++17 main.cpp -o movie_ticket_app && ./movie_ticket_app
```

---

### Parking_lot_system_LLD

Multi-vehicle-type parking, ticket on entry, hourly pricing on exit via `HourlyPricingStrategy`.

```bash
cd Parking_lot_system_LLD && g++ -std=c++17 main.cpp -o parking_app && ./parking_app
```

---

### Rate_Limiter_LLD

|                |                                                |
| -------------- | ---------------------------------------------- |
| **Patterns**   | Strategy, Factory                              |
| **Algorithms** | Token Bucket, Fixed Window, Sliding Window Log |
| **Tiers**      | FREE, PREMIUM — thread-safe with mutex         |

See also: `rate_limiter_Algo.md` for algorithm explanations.

```bash
cd Rate_Limiter_LLD && g++ -std=c++17 Main.cpp -o rate_limiter_app && ./rate_limiter_app
```

---

### LRU_Cache_LLD

| | |
|---|---|
| **Path** | [`LRU_Cache_LLD/`](./LRU_Cache_LLD/) |
| **Patterns** | Facade (`CacheService`), Decorator (`ThreadSafeLRUCache`), `ICache` interface |
| **Data structures** | `unordered_map` + `std::list` + `list::splice` for O(1) LRU |
| **Thread safety** | `std::mutex` on all operations (get mutates recency) |
| **Diagrams** | [Class + Sequence — Section 18](./SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#18-thread-safe-lru-cache) |

```bash
cd LRU_Cache_LLD && g++ -std=c++17 -pthread main.cpp -o lru_cache_app && ./lru_cache_app
```

---

### LFU_Cache_LLD

| | |
|---|---|
| **Path** | [`LFU_Cache_LLD/`](./LFU_Cache_LLD/) |
| **Patterns** | Facade (`CacheService`), Decorator (`ThreadSafeLFUCache`), `ICache` interface |
| **Data structures** | `unordered_map` + frequency buckets + `minFreq` |
| **Eviction** | Least frequently used; tie → LRU within bucket |
| **Compare** | [LRU Cache](./LRU_Cache_LLD/) |

```bash
cd LFU_Cache_LLD && g++ -std=c++17 -pthread main.cpp -o lfu_cache_app && ./lfu_cache_app
```

---

### Ride_sharing_app_LLD

Uber/Ola-lite: rider/driver registration, nearest driver matching, ride lifecycle, fare via `PricingService` + `GeoUtils`.

```bash
cd Ride_sharing_app_LLD && g++ -std=c++17 main.cpp -o ride_app && ./ride_app
```

---

### Uber_LLD

**Richer than Ride_sharing:** OTP verification, payment service, fare breakdown, ride cancellation.

| Service                 | Role                        |
| ----------------------- | --------------------------- |
| `DriverMatchingService` | Nearest available driver    |
| `FareService`           | Base + per-km breakdown     |
| `OtpService`            | Generate/verify ride OTP    |
| `PaymentService`        | Process payment on complete |

```bash
cd Uber_LLD && g++ -std=c++17 main.cpp -o uber_app && ./uber_app
```

---

### URL_Shortner_LLD

TinyURL-style: shorten, resolve, dedupe same long URL, click analytics. `Base62Encoder` for short codes.

```bash
cd URL_Shortner_LLD && g++ -std=c++17 main.cpp -o url_shortner_app && ./url_shortner_app
```

---

### vending_machine_LLD

Select item → insert money → dispense with change OR cancel/refund. `Inventory` + `MoneyManager` composition.

```bash
cd vending_machine_LLD && g++ -std=c++17 main.cpp -o vending_app && ./vending_app
```

---

### WhatsApp_LLD

|              |                                                                         |
| ------------ | ----------------------------------------------------------------------- |
| **Patterns** | Strategy (encryption), Null Object, Decorator, Observer                 |
| **Features** | 1:1 + group chats, message types/status, encryption hook, notifications |

**Services:** `ChatService`, `EncryptionService`, `NotificationService`, `NotificationEngine` (pattern demo)

```bash
cd WhatsApp_LLD && g++ -std=c++17 main.cpp -o whatsapp && ./whatsapp
```

---

### Insta_reel_LLD

Path: [`Insta_reel_LLD/yt reel architecture/`](./Insta_reel_LLD/yt%20reel%20architecture/)

Short-video platform: upload reels, view/like/comment, follow graph, personalized feed ranked by views/likes.

```bash
cd "Insta_reel_LLD/yt reel architecture" && g++ -std=c++17 main.cpp -o reels_app && ./reels_app
```

---

## Multi-Threading Module

**Path:** [`Multi_threading_C++/`](./Multi_threading_C++/) · **[Full README](./Multi_threading_C++/README.md)**

Educational snippets — har file alag topic. **Not** a unified system.

| File                                       | Topic                     |
| ------------------------------------------ | ------------------------- |
| `lessson_1_join.cpp`                       | `std::thread`, `join`     |
| `lesson_2_locks_and_mutex.cpp`             | Mutex basics              |
| `lesson_3.cpp`                             | Additional threading      |
| `race_condition_and_synchronization.cpp`   | Race conditions & fixes   |
| `lock_mechanism.cpp`, `types_of_locks.cpp` | Lock varieties            |
| `semaphor.cpp`                             | Semaphores                |
| `producer_consumer.cpp`                    | Classic producer-consumer |
| `dining_philosophers.cpp`                  | Dining philosophers (4 solutions) |
| `thread_pool.cpp`                          | Custom thread pool        |
| `deadlock_and_protection.cpp`              | Deadlock scenarios        |
| `DCLP.cpp`                                 | Double-checked locking    |
| `Thread_Safe_Injection.cpp`                | Thread-safe DI patterns   |
| `execution_time_of_code.cpp`               | Benchmarking              |

```bash
cd Multi_threading_C++
g++ -std=c++17 -pthread thread_pool.cpp -o thread_pool && ./thread_pool
```

> macOS/Linux par `-pthread` use karo. Kuch systems par `-lpthread` chahiye ho sakta hai.

---

## Design Pattern Coverage Matrix

### Kahan kaunsa pattern use hua

| Pattern                     | Primary lessons                  | System projects                                             |
| --------------------------- | -------------------------------- | ----------------------------------------------------------- |
| **Singleton**               | L10, L14, L23, L26               | Logger                                                      |
| **Factory**                 | L9, L11, L23, L31, L33–34, L37   | Movie Ticket, Rate Limiter                                  |
| **Strategy**                | L8, L11, L14, L18, L24, L31, L33 | Parking, Load Balancer, Rate Limiter, WhatsApp (encryption) |
| **Observer**                | L12, L14, L31, L33–34            | Logger (appenders)                                          |
| **Decorator**               | L13, L14                         | WhatsApp (notification decorators), LRU (`ThreadSafeLRUCache`) |
| **Adapter**                 | L16, L18                         | —                                                           |
| **Facade**                  | L11, L17, L18, L27, L31          | Most `core/` classes, `CacheService`                          |
| **Command**                 | L15                              | —                                                           |
| **Template Method**         | L20, L23                         | Rate Limiter (base)                                         |
| **Chain of Responsibility** | L22, L24                         | Logger                                                      |
| **State**                   | L32                              | (enum states in Blinkit, Car Rental)                        |
| **Bridge**                  | L25, L34                         | —                                                           |
| **Composite**               | L19, L7                          | JSON Parser                                                 |
| **Proxy**                   | L21, L23                         | —                                                           |
| **Builder**                 | L28                              | —                                                           |
| **Iterator**                | L29                              | —                                                           |
| **Flyweight**               | L30                              | —                                                           |
| **Mediator**                | L35, L37                         | —                                                           |
| **Prototype**               | L36                              | —                                                           |
| **Visitor**                 | L38                              | —                                                           |
| **Memento**                 | L39                              | —                                                           |
| **Null Object**             | L40 (notes)                      | WhatsApp (`NoOpEncryptionService`)                          |

### GoF taxonomy (from `Design_Pattern_types.md`)

**Creational:** Singleton, Factory Method, Abstract Factory, Builder, Prototype  
**Structural:** Adapter, Bridge, Composite, Decorator, Facade, Flyweight, Proxy  
**Behavioral:** Chain of Responsibility, Command, Iterator, Mediator, Memento, Observer, State, Strategy, Template Method, Visitor

---

## Interview Preparation Playbook

### Har project ke liye 5-minute pitch template

```
1. Problem (30 sec)     — "Design X with features A, B, C"
2. Entities (1 min)     — User, Order, … + key enums
3. APIs (1 min)         — 4-5 main methods on facade/service
4. Patterns (1 min)     — "Strategy for pricing because …"
5. Extensions (1 min)     — "I'd add persistence via Repository pattern"
6. Trade-offs (30 sec)  — in-memory vs DB, consistency, scale limits
```

### High-frequency interview systems (priority order)

| Priority | System                    | Why asked                            |
| -------- | ------------------------- | ------------------------------------ |
| ⭐⭐⭐   | Parking Lot               | OOP basics, Strategy, state of spots |
| ⭐⭐⭐   | BookMyShow / Movie Ticket | Concurrency on seats, complex domain |
| ⭐⭐⭐   | Splitwise                 | Graph-like balances, algorithms      |
| ⭐⭐⭐   | LRU Cache                 | Hash map + DLL, `list::splice`, mutex decorator |
| ⭐⭐     | LFU Cache                 | Frequency buckets + `minFreq`, tie-break LRU      |
| ⭐⭐⭐   | Rate Limiter              | Token bucket, sliding window, concurrency       |
| ⭐⭐     | Elevator                  | Scheduling, state machines           |
| ⭐⭐     | Chess / Tic-Tac-Toe       | Game rules, extensibility            |
| ⭐⭐     | Uber / Ride Sharing       | Matching, lifecycle                  |
| ⭐⭐     | WhatsApp / Chat           | Real-time patterns, message states   |
| ⭐⭐     | Load Balancer             | Strategy pattern classic             |
| ⭐⭐     | Logger                    | Chain of Responsibility classic      |
| ⭐       | URL Shortener             | Encoding, deduplication              |
| ⭐       | Vending Machine           | State pattern (also L32)             |
| ⭐       | JSON Parser               | Composite + parsing                  |

### Common follow-up questions

| Question                             | Hint — kahan dekho                              |
| ------------------------------------ | ----------------------------------------------- |
| "Seat booking me 2 users same seat?" | Movie Ticket — locking/transactions (extension) |
| "Rate limiter distributed ho to?"    | Rate Limiter — Redis + sliding window           |
| "Elevator algorithm optimize?"       | Elevator — SCAN/LOOK vs nearest                 |
| "Splitwise debt simplify proof?"     | L31 — `DebtSimplifier` greedy approach          |
| "WhatsApp message ordering?"         | WhatsApp — sequence numbers (extension)         |
| "LRU cache sharded / distributed?" | LRU Cache — consistent hashing + per-shard LRU    |
| "LRU me get() thread-safe kaise?"    | LRU Cache — mutex; get mutates order via splice   |

### SOLID quick recall (interview)

| Letter | One-liner                                    |
| ------ | -------------------------------------------- |
| **S**  | One class, one job                           |
| **O**  | Extend via new classes, not editing old ones |
| **L**  | Child must work wherever parent works        |
| **I**  | Small interfaces > fat interfaces            |
| **D**  | Depend on abstractions, not concretions      |

---

## Compilation & Run Guide

### Prerequisites

- **Compiler:** `g++` with C++17 support (GCC 7+ / Clang 5+)
- **OS:** macOS, Linux, Windows (WSL recommended)
- **Threading:** `-pthread` flag for `Multi_threading_C++` and thread-safe lessons

### Universal compile command

```bash
g++ -std=c++17 main.cpp -o app_name
./app_name
```

### Projects with non-standard entry files

| Project          | Entry file                      | Output binary      |
| ---------------- | ------------------------------- | ------------------ |
| Logger_LLD       | `Main.cpp`                      | `logger_app`       |
| Rate_Limiter_LLD | `Main.cpp`                      | `rate_limiter_app` |
| LRU_Cache_LLD    | `main.cpp` (use `-pthread`)     | `lru_cache_app`    |
| Insta_reel_LLD   | `yt reel architecture/main.cpp` | `reels_app`        |

### Batch compile script (optional — run from repo root)

```bash
#!/bin/bash
# save as build_all.sh — compiles all projects with main.cpp
for dir in ATM_LLD Car_Rental_System_LLD Elevator_System_LLD JSON_Parser_LLD \
  Library_Management_System_LLD Linkedin_LLD LoadBalancer_LLD \
  Parking_lot_system_LLD Ride_sharing_app_LLD Uber_LLD URL_Shortner_LLD \
  vending_machine_LLD WhatsApp_LLD; do
  if [ -f "$dir/main.cpp" ]; then
    echo "Building $dir..."
    (cd "$dir" && g++ -std=c++17 main.cpp -o "${dir##*/}_app" && echo "OK: $dir") || echo "FAIL: $dir"
  fi
done
```

> Lesson folders (`L*`) me often header-only demos hain — unke liye specific `.cpp` file compile karo.

---

## Standard Project Structure Convention

```
<ProjectName>/
├── core/              # Facade / orchestrator — single entry for client
├── models/            # Plain domain objects (User, Order, …)
├── services/          # Business logic (stateless where possible)
├── enums/             # BookingStatus, VehicleType, …
├── strategies/        # Pluggable algorithms (IStrategy interface)
├── factories/         # Object creation encapsulation
├── observers/         # Event listeners (optional)
├── managers/          # Sub-domain orchestrators (optional)
├── utils/             # Helpers (GeoUtils, Base62Encoder, …)
├── external/          # Third-party API simulations (optional)
├── main.cpp           # Demo driver — NOT production entry
├── problem_statement.md
├── requirements.md
└── README.md          # (some projects)
```

### Naming conventions (repo-wide)

- **Classes:** PascalCase (`ParkingLot`, `RideSharingService`)
- **Interfaces:** `I` prefix (`IObserver`, `IAudioOutputDevice`) or abstract base
- **Enums:** PascalCase type, UPPER_SNAKE values
- **Files:** Match class name — one primary class per header

---

## Reference Materials In Repo

| Asset                                                        | Purpose                                            |
| ------------------------------------------------------------ | -------------------------------------------------- |
| `Gang of Four.pdf`                                           | Original design patterns book                      |
| `Designing Data-Intensive Applications…pdf`                  | Scalability, storage, distributed systems thinking |
| `Design_Pattern_Sheet_1.webp`, `Design_Pattern_Sheet_2.webp` | Quick visual revision                              |
| `Design_Pattern_types.md`                                    | Creational/Structural/Behavioral taxonomy (Hindi)  |
| `imp_design_patterns.png`, `imp_problems.png`                | Important patterns & problems cheat sheet          |
| `Some_patterns.png`, `Commonly Asked Questions.png`          | Interview aids                                     |
| Per-lesson `Notes.pdf`, `UML.jpeg`                           | Lesson-specific diagrams                           |

**External reference:** [Refactoring Guru — Design Patterns](https://refactoring.guru/design-patterns)

---

## Extension Exercises

Har project complete karne ke baad yeh try karo:

| Project                   | Extension ideas                                     |
| ------------------------- | --------------------------------------------------- |
| Parking Lot               | Multi-floor, reserved spots, monthly pass           |
| Movie Ticket              | Concurrent seat booking (mutex), waitlist           |
| Rate Limiter              | Distributed Redis backend, per-API limits           |
| LRU Cache                 | TTL expiry, sharded locks, LFU eviction policy        |
| LFU Cache                 | Frequency aging, window LFU, unified cache factory    |
| Splitwise                 | Currency conversion, recurring expenses             |
| WhatsApp                  | End-to-end encryption interface, read receipts sync |
| Elevator                  | SCAN algorithm, weight capacity                     |
| Food Delivery (L11)       | Rating system, restaurant filters                   |
| Chess (L37)               | Undo move (Memento), AI player (Strategy)           |
| Notification Engine (L14) | Retry with exponential backoff, priority queue      |
| JSON Parser               | Pretty-print, schema validation                     |
| Tic-Tac-Toe (L33)         | N×N board, online multiplayer stub                  |

---

## Quick Navigation Index

### All lesson modules (L1–L40)

| #   | Module                        | Link                                                         |
| --- | ----------------------------- | ------------------------------------------------------------ |
| L1  | Introduction                  | [L1 Introduction](./L1%20Introduction/)                      |
| L2  | OOPS_1                        | [L2 OOPS_1](./L2%20OOPS_1/)                                  |
| L3  | OOPS_2                        | [L3 OOPS_2](./L3%20OOPS_2/)                                  |
| L4  | UML_Diagrams                  | [L4 UML_Diagrams](./L4%20UML_Diagrams/)                      |
| L5  | SOLID_1                       | [L5 SOLID_1](./L5%20SOLID_1/)                                |
| L6  | SOLID_2                       | [L6 SOLID_2](./L6%20SOLID_2/)                                |
| L7  | Document_Editor_LLD           | [L7](./L7%20Document_Editor_LLD/)                            |
| L8  | Strategy                      | [L8](./L8%20Strategy_Design_Patterns/)                       |
| L9  | Factory                       | [L9](./L9%20Factory_Design_Pattern/)                         |
| L10 | Singleton                     | [L10](./L10%20Singleton_Design_Pattern/)                     |
| L11 | Food_Delivery_LLD             | [L11](./L11%20Food_Delivery_LLD/)                            |
| L12 | Observer                      | [L12](./L12%20Observer_Design_Pattern/)                      |
| L13 | Decorator                     | [L13](./L13%20Decorator_Design_Pattern/)                     |
| L14 | Notification_Engine_LLD       | [L14](./L14%20Notification_Engine_LLD/)                      |
| L15 | Command                       | [L15](./L15%20Command_Design_Pattern/)                       |
| L16 | Adapter                       | [L16](./L16%20Adapter_Design_Pattern/)                       |
| L17 | Facade                        | [L17](./L17%20Facade_Design_Pattern/)                        |
| L18 | Spotify_LLD                   | [L18](./L18%20Spotify_LLD/)                                  |
| L19 | Composite                     | [L19](./L19%20Composite_Design_Pattern/)                     |
| L20 | Template Method               | [L20](./L20%20Template_Method_Pattern/)                      |
| L21 | Proxy                         | [L21](./L21%20Proxy_Design_Pattern/)                         |
| L22 | Chain of Responsibility (ATM) | [L22](./L22%20Chain_of_responsiblity_patten%28ATM%20LLD%29/) |
| L23 | Payment Gateway LLD           | [L23](./L23%20Payment_gateway_system_LLD/)                   |
| L24 | Discount Coupon Engine        | [L24](./L24%20Discount_coupon_engine_LLD/)                   |
| L25 | Bridge                        | [L25](./L25%20Bridge_design_pattern/)                        |
| L26 | Blinkit_LLD                   | [L26](./L26%20Blinkit_LLD/)                                  |
| L27 | Tinder_LLD                    | [L27](./L27%20Tinder_LLD/)                                   |
| L28 | Builder                       | [L28](./L28%20Builder_design_pattern/)                       |
| L29 | Iterator                      | [L29](./L29%20Iterator_design_pattern/)                      |
| L30 | Flyweight                     | [L30](./L30%20Flyweight_design_pattern/)                     |
| L31 | Splitwise_LLD                 | [L31](./L31%20Splitwise_LLD/)                                |
| L32 | State                         | [L32](./L32%20State_design_pattern/)                         |
| L33 | Tic_Tac_Toe_LLD               | [L33](./L33%20Tic_Tac_Toe_LLD/)                              |
| L34 | Snake_ladder_LLD              | [L34](./L34%20Snake_ladder_LLD/)                             |
| L35 | Mediator                      | [L35](./L35%20Mediator_design_pattern/)                      |
| L36 | Prototype                     | [L36](./L36%20Prototype_design_pattern/)                     |
| L37 | Chess_LLD                     | [L37](./L37%20Chess_LLD/)                                    |
| L38 | Visitor                       | [L38](./L38%20Visitor_design_pattern/)                       |
| L39 | Memento                       | [L39](./L39%20Memento_design_pattern/)                       |
| L40 | Null Object & Antipatterns    | [L40](./L40%20Null_object_pattern_and_Antipatterns/)         |

### All standalone systems

| System               | Link                                                              |
| -------------------- | ----------------------------------------------------------------- |
| ATM                  | [ATM_LLD](./ATM_LLD/)                                             |
| Car Rental           | [Car_Rental_System_LLD](./Car_Rental_System_LLD/)                 |
| Elevator             | [Elevator_System_LLD](./Elevator_System_LLD/)                     |
| JSON Parser          | [JSON_Parser_LLD](./JSON_Parser_LLD/)                             |
| Library Management   | [Library_Management_System_LLD](./Library_Management_System_LLD/) |
| LinkedIn             | [Linkedin_LLD](./Linkedin_LLD/)                                   |
| Load Balancer        | [LoadBalancer_LLD](./LoadBalancer_LLD/)                           |
| Logger               | [Logger_LLD](./Logger_LLD/)                                       |
| Movie Ticket Booking | [Movie_Ticket_Booking_System](./Movie_Ticket_Booking_System/)     |
| Parking Lot          | [Parking_lot_system_LLD](./Parking_lot_system_LLD/)               |
| Rate Limiter         | [Rate_Limiter_LLD](./Rate_Limiter_LLD/)                           |
| LRU Cache            | [LRU_Cache_LLD](./LRU_Cache_LLD/)                                 |
| LFU Cache            | [LFU_Cache_LLD](./LFU_Cache_LLD/)                                 |
| Ride Sharing         | [Ride_sharing_app_LLD](./Ride_sharing_app_LLD/)                   |
| Uber                 | [Uber_LLD](./Uber_LLD/)                                           |
| URL Shortener        | [URL_Shortner_LLD](./URL_Shortner_LLD/)                           |
| Vending Machine      | [vending_machine_LLD](./vending_machine_LLD/)                     |
| WhatsApp             | [WhatsApp_LLD](./WhatsApp_LLD/)                                   |
| Insta/YouTube Reels  | [Insta_reel_LLD](./Insta_reel_LLD/yt%20reel%20architecture/)      |
| Multi-threading      | [Multi_threading_C++](./Multi_threading_C%2B%2B/)                 |

---

## Glossary

| Term          | Meaning                                                         |
| ------------- | --------------------------------------------------------------- |
| **LLD**       | Low-Level Design — classes, interfaces, relationships, patterns |
| **HLD**       | High-Level Design — services, databases, queues (see DDIA pdf)  |
| **Facade**    | Simple API hiding complex subsystems                            |
| **Strategy**  | Interchangeable algorithms behind common interface              |
| **Factory**   | Centralized object creation                                     |
| **Observer**  | One-to-many notification on state change                        |
| **Decorator** | Add behavior dynamically by wrapping                            |
| **Composite** | Tree structures with uniform interface                          |
| **CoR**       | Chain of Responsibility — pass request along handler chain      |
| **DIP**       | Depend on abstractions, inject implementations                  |
| **OCP**       | Open for extension, closed for modification                     |
| **DCLP**      | Double-Checked Locking Pattern                                  |
| **RAII**      | Resource Acquisition Is Initialization — C++ cleanup idiom      |

---

## Final Notes

- **Yeh repo interview preparation ke liye hai** — production deployment (DB, API gateway, k8s) in-memory demos me intentionally nahi hai; woh HLD/DDIA layer hai.
- **Har folder me `problem_statement.md` padhna mat bhoolo** — README summary hai, ground truth woh files hain.
- **Code khud likhna > sirf padhna** — ek pattern demo (L8) aur ek full LLD (L11) ke baad khud se Parking Lot banao bina dekhe.

---

<p align="center">
  <b>Happy Learning — Code First, Design Always.</b><br/>
  <sub>Repository maintained as a personal LLD mastery vault.</sub>
</p>
