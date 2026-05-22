# Low-Level Design (LLD) — Complete Master Repository Guide

<p align="center">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=24&duration=2500&pause=800&color=2F81F7&center=true&vCenter=true&width=900&lines=Master+LLD+Guide+%E2%80%94+1800%2B+Lines;40+Lessons+%7C+26+System+Projects;Patterns+%2B+Interview+Deep+Dives" alt="Typing animation" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge" alt="C++ Badge" />
  <img src="https://img.shields.io/badge/Lessons-L1%E2%80%93L40-6f42c1?style=for-the-badge" alt="Lessons badge" />
  <img src="https://img.shields.io/badge/System+Projects-25-success?style=for-the-badge" alt="Projects badge" />
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
19. [System Projects — Deep Dive & Interview Bank](#system-projects--deep-dive--interview-bank)
20. [Company-Wise Problem Mapping](#company-wise-problem-mapping)
21. [12-Week Study Calendar](#12-week-study-calendar)
22. [FAQ — Frequently Asked Questions](#faq--frequently-asked-questions)
23. [Troubleshooting — Build & IDE](#troubleshooting--build--ide)
24. [Glossary](#glossary)
25. [Changelog & Maintenance](#changelog--maintenance)

---

## Quick Start (5 Minutes)

Agar tum **abhi** start kar rahe ho, yeh minimal path follow karo:

```mermaid
flowchart LR
    A[README skim] --> B[L1-L6 Foundations]
    B --> C[L8 Strategy + L9 Factory]
    C --> D[1 System Project]
    D --> E[Whiteboard + main.cpp run]
    E --> F[SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md]
```

| Step | Action | Time |
| ---- | ------ | ---- |
| 1 | Clone repo, open in VS Code / Cursor | 2 min |
| 2 | `cd Parking_lot_system_LLD && ./compile.sh` | 1 min |
| 3 | `problem_statement.md` padho, phir `core/ParkingLot.h` | 5 min |
| 4 | Khud se ek method add karo (e.g. VIP spot) | 30–60 min |

**First compile (any system project):**

```bash
cd Parking_lot_system_LLD
./compile.sh          # preferred — C++17 + -I.
./parking_app
```

**Diagrams:** Har major system ke class + sequence diagrams → [`SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md`](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md) (26 systems, ~4300+ lines Mermaid).

---

## Repository Overview

Yeh repository **Low-Level Design (LLD)** seekhne ke liye banayi gayi hai — theory se zyada **working C++ code** par focus ke saath. Isme:

| Category               | Count        | Description                                                                |
| ---------------------- | ------------ | -------------------------------------------------------------------------- |
| **Lesson modules**     | L1 – L40     | OOP, SOLID, UML, har major design pattern, aur pattern + system hybrids    |
| **Standalone systems** | 25 projects  | Interview systems + Airline + Pub-Sub + OTP |
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
| Runnable system demos                | 26+                     |
| Projects with dedicated `README.md`  | 24+                     |
| Design patterns demonstrated         | 20+ (GoF + Null Object) |
| Projects with `problem_statement.md` | 28+                     |
| UML class + sequence diagrams        | [`SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md`](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md) |

---

## Why This Repository Exists

Zyada tar LLD preparation in points par atak jaati hai:

| Problem                                           | Is repo ka solution                                                    |
| ------------------------------------------------- | ---------------------------------------------------------------------- |
| Theory samajh aata hai, code me convert nahi hota | Har concept ke saath runnable `.cpp` / modular headers                 |
| Code likh lete hain, extensibility weak hoti hai  | SOLID + OCP-focused refactors (e.g. L7 Bad vs Good design)             |
| Sirf 1–2 problems solve kiye                      | 26+ full systems + 10+ lesson-level LLDs                               |
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
├── README.md                     # ← You are here (repo hub)
├── docs/                         # Theory + maps + UML (5 markdown files)
├── assets/                       # Cheat sheets (.webp) + images (.png)
├── books/                        # PDF references (GoF, DDIA)
├── scripts/                      # build_all_systems.sh
├── L1 Introduction/ … L40/       # 40 lessons
├── ATM_LLD/ … GPay_LLD/ …      # 26+ system projects
└── Multi_threading_C++/        # Concurrency labs + interview problems
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
| **Complete guide** | [**`OOPS_1_COMPLETE.md`**](./L2%20OOPS_1/OOPS_1_COMPLETE.md) — Encapsulation + Abstraction detail |
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
| **Complete guide** | [**`OOPS_2_COMPLETE.md`**](./L3%20OOPS_2/OOPS_2_COMPLETE.md) — Inheritance + Static/Dynamic polymorphism |
| **Files**        | `Inheritance.cpp`, `StaticPolymorphism.cpp`, `DynamicPolymorphism.cpp`, `StaticAndDynamicPolymorphism.cpp` |
| **Concepts**     | IS-A relationships, method overloading (compile-time), virtual overrides (runtime)                         |
| **Key takeaway** | Static = compile-time binding; Dynamic = vtable/runtime dispatch                                           |

---

### L4 UML_Diagrams

|                |                                                                                                              |
| -------------- | ------------------------------------------------------------------------------------------------------------ |
| **Path**       | [`L4 UML_Diagrams/`](./L4%20UML_Diagrams/)                                                                   |
| **Guides**     | [`UML_DIAGRAMS_AND_NOTATION.md`](./L4%20UML_Diagrams/UML_DIAGRAMS_AND_NOTATION.md) · [`INHERITANCE_AND_COMPOSITION.md`](./L4%20UML_Diagrams/INHERITANCE_AND_COMPOSITION.md) |
| **Cpp README** | [`L4 README`](./L4%20UML_Diagrams/README.md) — `inheritance.cpp` + `composition.cpp` deep dive |
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
| **Build**    | `./compile.sh` → `discount_coupon_app`                                   |
| **Legacy**   | `C++ Code/DiscountCoupon.cpp` (original monolith, preserved)             |

**Features:** Flat/percent/capped discounts, coupon chains, cart offers (seasonal, loyalty, bulk, banking), thread-safe coupon registry. Modular headers under `core/`, `coupons/`, `strategies/`, `models/`.

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

### File_Manager_LLD

|              |                                                                      |
| ------------ | -------------------------------------------------------------------- |
| **Path**     | [`File_Manager_LLD/`](./File_Manager_LLD/)                           |
| **Patterns** | **Composite** (dir + files), **Facade**, path/search/copy services   |
| **Commands** | `mkdir`, `createFile`, `cd`, `pwd`, `ls`, `cat`, `rm`, `mv`, `cp`, `find`, `getSize` |

```bash
cd File_Manager_LLD && ./compile.sh && ./file_manager_app
```

---

### OTP_Generation_System_LLD

|              |                                                                        |
| ------------ | ---------------------------------------------------------------------- |
| **Path**     | [`OTP_Generation_System_LLD/`](./OTP_Generation_System_LLD/)           |
| **Patterns** | **Facade**, **Strategy** (generator + SMS/Email channels), rate limit  |
| **Flow**     | `sendOtp` → SMS/Email → `verifyOtp` → resend / lock / expiry           |

```bash
cd OTP_Generation_System_LLD && ./compile.sh && ./otp_app
```

---

### Thread_Safe_Cache_with_TTL_LLD

|              |                                                                              |
| ------------ | ---------------------------------------------------------------------------- |
| **Path**     | [`Thread_Safe_Cache_with_TTL_LLD/`](./Thread_Safe_Cache_with_TTL_LLD/)       |
| **Patterns** | **Reader-Writer lock** (`shared_mutex`), lazy expiry, capacity eviction    |
| **Flow**     | `put(key, value, ttl)` → `get` → `cleanupExpired` — concurrent-safe        |

```bash
cd Thread_Safe_Cache_with_TTL_LLD && ./compile.sh && ./cache_ttl_app
```

---

### Concurrent_HashMap_LLD

|              |                                                                              |
| ------------ | ---------------------------------------------------------------------------- |
| **Path**     | [`Concurrent_HashMap_LLD/`](./Concurrent_HashMap_LLD/)                       |
| **Patterns** | **Lock striping**, coarse-grained lock, `IConcurrentMap` interface           |
| **Flow**     | `put` / `get` / `remove` — parallel ops on different hash stripes            |

```bash
cd Concurrent_HashMap_LLD && ./compile.sh && ./concurrent_hashmap_app
```

---

### GPay_LLD

|              |                                                                                         |
| ------------ | --------------------------------------------------------------------------------------- |
| **Path**     | [`GPay_LLD/`](./GPay_LLD/)                                                             |
| **Patterns** | **Facade** (`GPaySystem`), **Strategy** (bank vs wallet rail), **Factory**              |
| **Flow**     | Register UPI → link bank → P2P / QR pay / request money → transaction ledger            |
| **Note**     | Consumer UPI app (P2P); differs from [`L23 Payment_gateway_system_LLD`](./L23%20Payment_gateway_system_LLD/) (merchant gateway) |

```bash
cd GPay_LLD && ./compile.sh && ./gpay_app
```

---

### Truecaller_LLD

|              |                                                                                    |
| ------------ | ---------------------------------------------------------------------------------- |
| **Path**     | [`Truecaller_LLD/`](./Truecaller_LLD/)                                             |
| **Patterns** | **Facade**, **Strategy** (spam scoring), service layer                             |
| **Flow**     | Contact sync → caller lookup → spam report → block → call log                      |

```bash
cd Truecaller_LLD && ./compile.sh && ./truecaller_app
```

---

### Meeting_Scheduler_LLD

|              |                                                                                    |
| ------------ | ---------------------------------------------------------------------------------- |
| **Path**     | [`Meeting_Scheduler_LLD/`](./Meeting_Scheduler_LLD/)                               |
| **Patterns** | **Facade**, **Strategy** (mutual free slots), **Factory**, service layer         |
| **Flow**     | Set availability → find slots → schedule → conflict check → cancel               |

```bash
cd Meeting_Scheduler_LLD && ./compile.sh && ./meeting_scheduler_app
```

---

### Amazon_Locker_Service_LLD

|              |                                                                              |
| ------------ | ---------------------------------------------------------------------------- |
| **Path**     | [`Amazon_Locker_Service_LLD/`](./Amazon_Locker_Service_LLD/)                 |
| **Patterns** | **Facade**, **Strategy** (compartment allocation), service layer             |
| **Flow**     | Courier deposit → OTP notify → customer pickup → compartment release         |

```bash
cd Amazon_Locker_Service_LLD && ./compile.sh && ./amazon_locker_app
```

---

### Pub_Sub_System_LLD

|              |                                                                          |
| ------------ | ------------------------------------------------------------------------ |
| **Path**     | [`Pub_Sub_System_LLD/`](./Pub_Sub_System_LLD/)                           |
| **Patterns** | **Observer**, **Facade**, message **broker** (topic fan-out)             |
| **Flow**     | `createTopic` → `subscribe` → `publish` → all subscribers `onMessage`    |

```bash
cd Pub_Sub_System_LLD && ./compile.sh && ./pubsub_app
```

---

### Airline_Management_System_LLD

|              |                                                                                    |
| ------------ | ---------------------------------------------------------------------------------- |
| **Path**     | [`Airline_Management_System_LLD/`](./Airline_Management_System_LLD/)             |
| **Patterns** | **Facade**, **Strategy** (pricing), services, **`mutex`** (concurrent seats)       |
| **Features** | Search, book, pay, crew, roles, cancel/refund, flight change, baggage              |

```bash
cd Airline_Management_System_LLD && ./compile.sh && ./airline_app
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
| **Diagrams** | [Class + Sequence — Section 18](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#18-thread-safe-lru-cache) |

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
cd LFU_Cache_LLD && ./compile.sh && ./lfu_cache_app
```

---

### OYO_Hotel_Booking_LLD

| | |
|---|---|
| **Path** | [`OYO_Hotel_Booking_LLD/`](./OYO_Hotel_Booking_LLD/) |
| **Patterns** | Facade, Strategy (pricing), Service layer |
| **Features** | Search by city, date-range availability, book/cancel, check-in/out |
| **Diagrams** | [Section 21](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#21-oyo-hotel-booking) |

```bash
cd OYO_Hotel_Booking_LLD && ./compile.sh && ./oyo_hotel_app
```

---

### LeetCode_LLD

| | |
|---|---|
| **Path** | [`LeetCode_LLD/`](./LeetCode_LLD/) |
| **Patterns** | Facade (`LeetCodeSystem`), Strategy (`ICodeRunner`), Service layer |
| **Features** | Problem catalog, submit code, judge verdicts, leaderboard, acceptance rate |
| **HARD problem** | [Minimum Cost to Divide Array](./LeetCode_LLD/problems/MIN_COST_DIVIDE_ARRAY.md) — O(n²) DP |
| **Diagrams** | [Class + Sequence — Section 20](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#20-leetcode-online-judge) |

**Core flow:** `registerUser` → browse/search problems → `submitSolution` → `JudgeService` + `MockCodeRunner` → update stats & leaderboard

```bash
cd LeetCode_LLD && ./compile.sh && ./leetcode_app
```

**Seeded problems:** `TWO_SUM`, `REVERSE_STRING`, `VALID_PARENTHESES`, `MIN_COST_DIVIDE_ARRAY` (Hard)

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
cd WhatsApp_LLD && ./compile.sh && ./whatsapp_app
```

**Encryption (LLD hook — real E2E nahi):** `EncryptionService` interface + `NoOpEncryptionService` (plain text) + demo `DemoEncryptionService` in `main.cpp` (`"Hello"` → `"enc(Hello)"`). `ChatService::getEncryptedMessage()` har send par encrypt karta hai. Interview me bolo: production me Signal-style key exchange + AES — yahan **Strategy pattern** dikhane ke liye stub hai.

---

### Insta_reel_LLD

Path: [`Insta_reel_LLD/yt reel architecture/`](./Insta_reel_LLD/yt%20reel%20architecture/)

Short-video platform: upload reels, view/like/comment, follow graph, personalized feed ranked by views/likes.

```bash
cd "Insta_reel_LLD/yt reel architecture" && g++ -std=c++17 main.cpp -o reels_app && ./reels_app
```

---

## Multi-Threading Module

**Path:** [`Multi_threading_C++/`](./Multi_threading_C++/) · **[Full module README](./Multi_threading_C++/README.md)**

Educational C++ concurrency — root-level `.cpp` labs **+** structured subfolders (patterns, challenges, interview problems). **Not** a single unified LLD system (UML: see standalone §23–§24 for concurrent map / TTL cache).

### Subfolders (sab links)

| Area | Folder | README | Deep guide (COMPLETE.md) |
|------|--------|--------|--------------------------|
| **Module index** | [`Multi_threading_C++/`](./Multi_threading_C%2B%2B/) | [README](./Multi_threading_C++/README.md) | — |
| **Fundamentals** | [`01_Fundamentals/`](./Multi_threading_C++/01_Fundamentals/) | [README](./Multi_threading_C++/01_Fundamentals/README.md) | threads, mutex, locks, CV |
| **Concurrency Patterns** | [`02_Concurrency_Patterns/`](./Multi_threading_C++/02_Concurrency_Patterns/) | [README](./Multi_threading_C++/02_Concurrency_Patterns/README.md) | per pattern below |
| → Signaling | [`Signaling_Pattern/`](./Multi_threading_C++/02_Concurrency_Patterns/Signaling_Pattern/) | [README](./Multi_threading_C++/02_Concurrency_Patterns/Signaling_Pattern/README.md) | [COMPLETE](./Multi_threading_C++/02_Concurrency_Patterns/Signaling_Pattern/SIGNALING_PATTERN_COMPLETE.md) |
| → Thread Pool | [`Thread_Pool_Pattern/`](./Multi_threading_C++/02_Concurrency_Patterns/Thread_Pool_Pattern/) | [README](./Multi_threading_C++/02_Concurrency_Patterns/Thread_Pool_Pattern/README.md) | [COMPLETE](./Multi_threading_C++/02_Concurrency_Patterns/Thread_Pool_Pattern/THREAD_POOL_PATTERN_COMPLETE.md) |
| → Producer-Consumer | [`Producer_Consumer_Pattern/`](./Multi_threading_C++/02_Concurrency_Patterns/Producer_Consumer_Pattern/) | [README](./Multi_threading_C++/02_Concurrency_Patterns/Producer_Consumer_Pattern/README.md) | [COMPLETE](./Multi_threading_C++/02_Concurrency_Patterns/Producer_Consumer_Pattern/PRODUCER_CONSUMER_PATTERN_COMPLETE.md) |
| → Reader-Writer | [`Reader_Writer_Pattern/`](./Multi_threading_C++/02_Concurrency_Patterns/Reader_Writer_Pattern/) | [README](./Multi_threading_C++/02_Concurrency_Patterns/Reader_Writer_Pattern/README.md) | [COMPLETE](./Multi_threading_C++/02_Concurrency_Patterns/Reader_Writer_Pattern/READER_WRITER_PATTERN_COMPLETE.md) |
| **Lock-Free / CAS** | [`03_Lock_Free/Compare_And_Swap/`](./Multi_threading_C++/03_Lock_Free/Compare_And_Swap/) | [README](./Multi_threading_C++/03_Lock_Free/Compare_And_Swap/README.md) | [COMPLETE](./Multi_threading_C++/03_Lock_Free/Compare_And_Swap/COMPARE_AND_SWAP_COMPLETE.md) |
| **Concurrency Challenges** | [`04_Concurrency_Challenges/`](./Multi_threading_C++/04_Concurrency_Challenges/) | [README](./Multi_threading_C++/04_Concurrency_Challenges/README.md) | — |
| → Deadlock | [`Deadlock/`](./Multi_threading_C++/04_Concurrency_Challenges/Deadlock/) | [README](./Multi_threading_C++/04_Concurrency_Challenges/Deadlock/README.md) | [COMPLETE](./Multi_threading_C++/04_Concurrency_Challenges/Deadlock/DEADLOCK_COMPLETE.md) |
| → Livelock | [`Livelock/`](./Multi_threading_C++/04_Concurrency_Challenges/Livelock/) | [README](./Multi_threading_C++/04_Concurrency_Challenges/Livelock/README.md) | [COMPLETE](./Multi_threading_C++/04_Concurrency_Challenges/Livelock/LIVELOCK_COMPLETE.md) |
| **Interview Problems** | [`06_Interview_Problems/`](./Multi_threading_C++/06_Interview_Problems/) | [README](./Multi_threading_C++/06_Interview_Problems/README.md) · [INDEX](./Multi_threading_C++/06_Interview_Problems/INTERVIEW_PROBLEMS_COMPLETE.md) | LC 1114–1117, 411, barrier, queue, merge sort |
| → Fizz Buzz (LC 411) | [`Fizz_Buzz/`](./Multi_threading_C++/06_Interview_Problems/Fizz_Buzz/) | [README](./Multi_threading_C++/06_Interview_Problems/Fizz_Buzz/README.md) | [COMPLETE](./Multi_threading_C++/06_Interview_Problems/Fizz_Buzz/FIZZ_BUZZ_MULTITHREADED_COMPLETE.md) |
| → Print in Order (LC 1114) | [`Print_in_Order/`](./Multi_threading_C++/06_Interview_Problems/Print_in_Order/) | [README](./Multi_threading_C++/06_Interview_Problems/Print_in_Order/README.md) | — |
| → Print FooBar (LC 1115) | [`Print_FooBar_Alternately/`](./Multi_threading_C++/06_Interview_Problems/Print_FooBar_Alternately/) | [README](./Multi_threading_C++/06_Interview_Problems/Print_FooBar_Alternately/README.md) | — |
| → Zero Even Odd (LC 1116) | [`Print_Zero_Even_Odd/`](./Multi_threading_C++/06_Interview_Problems/Print_Zero_Even_Odd/) | [README](./Multi_threading_C++/06_Interview_Problems/Print_Zero_Even_Odd/README.md) | — |
| → Building H2O (LC 1117) | [`Building_H2O/`](./Multi_threading_C++/06_Interview_Problems/Building_H2O/) | [README](./Multi_threading_C++/06_Interview_Problems/Building_H2O/README.md) | — |
| → Barrier + Latch | [`Barrier_Synchronization/`](./Multi_threading_C++/06_Interview_Problems/Barrier_Synchronization/) | [README](./Multi_threading_C++/06_Interview_Problems/Barrier_Synchronization/README.md) | — |
| → Bounded Queue | [`Bounded_Blocking_Queue/`](./Multi_threading_C++/06_Interview_Problems/Bounded_Blocking_Queue/) | [README](./Multi_threading_C++/06_Interview_Problems/Bounded_Blocking_Queue/README.md) | — |
| → Dining Philosophers (LC 1226) | [`Dining_Philosophers_LC1226/`](./Multi_threading_C++/06_Interview_Problems/Dining_Philosophers_LC1226/) | [README](./Multi_threading_C++/06_Interview_Problems/Dining_Philosophers_LC1226/README.md) | — |
| → Web Crawler (LC 1242) | [`Web_Crawler_Multithreaded_LC1242/`](./Multi_threading_C++/06_Interview_Problems/Web_Crawler_Multithreaded_LC1242/) | [README](./Multi_threading_C++/06_Interview_Problems/Web_Crawler_Multithreaded_LC1242/README.md) | — |
| → Merge Sort | [`Merge_Sort/`](./Multi_threading_C++/06_Interview_Problems/Merge_Sort/) | [README](./Multi_threading_C++/06_Interview_Problems/Merge_Sort/README.md) | [COMPLETE](./Multi_threading_C++/06_Interview_Problems/Merge_Sort/MULTI_THREADED_MERGE_SORT_COMPLETE.md) |

### Quick build (subfolders)

```bash
# Patterns
cd Multi_threading_C++/02_Concurrency_Patterns/Signaling_Pattern && ./compile.sh && ./bin/01_condition_variable_basics
cd Multi_threading_C++/02_Concurrency_Patterns/Thread_Pool_Pattern && ./compile.sh && ./bin/01_basic_thread_pool
cd Multi_threading_C++/02_Concurrency_Patterns/Producer_Consumer_Pattern && ./compile.sh && ./bin/01_single_producer_single_consumer
cd Multi_threading_C++/02_Concurrency_Patterns/Reader_Writer_Pattern && ./compile.sh && ./bin/01_std_shared_mutex_basics

# Challenges
cd Multi_threading_C++/04_Concurrency_Challenges/Deadlock && ./compile.sh && ./bin/01_coffman_four_conditions
cd Multi_threading_C++/04_Concurrency_Challenges/Livelock && ./compile.sh && ./bin/01_what_is_livelock

# Interview problems
cd Multi_threading_C++/01_Fundamentals && ./compile.sh && ./bin/lessson_1_join
cd Multi_threading_C++/03_Lock_Free/Compare_And_Swap && ./compile.sh && ./bin/01_what_is_cas
cd Multi_threading_C++/06_Interview_Problems/Fizz_Buzz && ./compile.sh && ./bin/04_condition_variable
cd Multi_threading_C++/06_Interview_Problems/Merge_Sort && ./compile.sh && ./bin/06_compare_timings
```

### Root-level `.cpp` labs (folder root)

| File | Topic |
|------|-------|
| [`lessson_1_join.cpp`](./Multi_threading_C++/01_Fundamentals/lessson_1_join.cpp) | `std::thread`, `join` |
| [`lesson_2_locks_and_mutex.cpp`](./Multi_threading_C++/01_Fundamentals/lesson_2_locks_and_mutex.cpp) | Mutex basics |
| [`lesson_3.cpp`](./Multi_threading_C++/01_Fundamentals/lesson_3.cpp) | `condition_variable` |
| [`race_condition_and_synchronization.cpp`](./Multi_threading_C++/01_Fundamentals/race_condition_and_synchronization.cpp) | Race conditions & fixes |
| [`lock_mechanism.cpp`](./Multi_threading_C++/01_Fundamentals/lock_mechanism.cpp), [`types_of_locks.cpp`](./Multi_threading_C++/01_Fundamentals/types_of_locks.cpp) | Lock varieties |
| [`semaphor.cpp`](./Multi_threading_C++/01_Fundamentals/semaphor.cpp) | Semaphores |
| [`producer_consumer.cpp`](./Multi_threading_C++/05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp) | Classic producer-consumer |
| [`dining_philosophers.cpp`](./Multi_threading_C++/05_Classic_Problems/Dining_Philosophers/dining_philosophers.cpp) | Dining philosophers (4 solutions) |
| [`thread_pool.cpp`](./Multi_threading_C++/05_Classic_Problems/Thread_Pool_Legacy/thread_pool.cpp) | Custom thread pool |
| [`deadlock_and_protection.cpp`](./Multi_threading_C++/05_Classic_Problems/Deadlock_Legacy/deadlock_and_protection.cpp) | Deadlock scenarios |
| [`DCLP.cpp`](./Multi_threading_C++/05_Classic_Problems/Double_Checked_Locking/DCLP.cpp) | Double-checked locking |
| [`Thread_Safe_Injection.cpp`](./Multi_threading_C++/01_Fundamentals/Thread_Safe_Injection.cpp) | Thread-safe DI |
| [`execution_time_of_code.cpp`](./Multi_threading_C++/01_Fundamentals/execution_time_of_code.cpp) | Benchmarking |

```bash
cd Multi_threading_C++
g++ -std=c++17 -pthread thread_pool.cpp -o thread_pool && ./thread_pool
g++ -std=c++17 -pthread dining_philosophers.cpp -o dp && ./dp
```

> macOS/Linux par `-pthread` use karo. Kuch systems par `-lpthread` chahiye ho sakta hai.

---

## Design Pattern Coverage Matrix

### Kahan kaunsa pattern use hua

| Pattern                     | Primary lessons                  | System projects                                             |
| --------------------------- | -------------------------------- | ----------------------------------------------------------- |
| **Singleton**               | L10, L14, L23, L26               | Logger                                                      |
| **Factory**                 | L9, L11, L23, L31, L33–34, L37   | Movie Ticket, Rate Limiter, **GPay** (rail + transaction)   |
| **Strategy**                | L8, L11, L14, L18, L24, L31, L33 | Parking, Load Balancer, Rate Limiter, WhatsApp, LeetCode, **GPay** (bank/wallet), **Truecaller** (spam) |
| **Observer**                | L12, L14, L31, L33–34            | Logger (appenders)                                          |
| **Decorator**               | L13, L14                         | WhatsApp (notification decorators), LRU (`ThreadSafeLRUCache`) |
| **Adapter**                 | L16, L18                         | —                                                           |
| **Facade**                  | L11, L17, L18, L27, L31          | Most `core/` classes, `CacheService`, **GPay**, **Truecaller** |
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
| ⭐⭐     | LeetCode / Online Judge   | Submit → judge pipeline, Strategy runner |
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
| "LFU me same frequency tie?"         | LFU — LRU tie-break within frequency bucket       |
| "Parking lot multi-entry?"           | Multiple `ParkingLot` or zone id on ticket        |
| "Logger async without blocking?"     | Queue + worker thread; batch writes               |
| "URL shortener analytics?"           | `UrlEntry` click count — aggregate to DB            |
| "ATM dispense exact amount?"         | CashDispenser greedy — NP-hard general case note  |
| "Reels feed cold start?"             | Popular / trending fallback for new users         |
| "LinkedIn connection mutual?"        | Store directed edge; accept creates双向 (extension) |
| "JSON parser streaming?"             | SAX-style vs DOM composite — memory trade-off       |

### Mock interview rubric (self-score / peer)

| Criteria | 0 | 1 | 2 |
| -------- | - | - | - |
| Requirements clarified | Skipped | Partial | Assumptions listed |
| Class diagram | Missing | Incomplete | Clear entities + relations |
| Patterns named | None | Wrong pattern | Correct + justification |
| Code structure | Monolith | Some layers | core/models/services |
| Extensibility | Hard-coded | Some interfaces | OCP demonstrated |
| Trade-offs | None | Vague | Concrete (scale, consistency) |
| Time management | Over 60 min | 45–60 min | Finished core in 35 min |

**Target:** Score ≥ 8/12 before real interview loop.

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

- **Compiler:** `g++` or `clang++` with **C++17** (`-std=c++17`)
- **OS:** macOS, Linux, Windows (WSL recommended)
- **Threading:** `-pthread` for `Multi_threading_C++`, `LRU_Cache_LLD`, `LFU_Cache_LLD`

### C++17 + compile helpers (system projects)

Har standalone system project me **`#include <bits/stdc++.h>`** + **C++17** build:

| File | Purpose |
| ---- | ------- |
| `.clangd` | IDE: `-std=c++17 -pthread -I.` |
| `compile.sh` | One-command build from project folder |

**Recommended (per project):**

```bash
cd ATM_LLD
./compile.sh
./atm_app
```

**Manual equivalent:**

```bash
g++ -std=c++17 -Wall -Wextra -pthread -I. main.cpp -o atm_app
```

**Build all standalone systems from repo root:**

```bash
chmod +x scripts/build_all_systems.sh
./scripts/build_all_systems.sh
```

### Projects with non-standard entry files

| Project          | Entry file                        | Output binary        |
| ---------------- | --------------------------------- | -------------------- |
| Logger_LLD       | `Main.cpp`                        | `logger_app`         |
| Rate_Limiter_LLD | `Main.cpp`                        | `rate_limiter_app`   |
| LRU_Cache_LLD    | `main.cpp` (`-pthread` in script) | `lru_cache_app`      |
| LFU_Cache_LLD    | `main.cpp` (`-pthread`)           | `lfu_cache_app`      |
| Insta_reel_LLD   | `yt reel architecture/main.cpp` | `reels_app`          |
| LeetCode_LLD     | `main.cpp`                        | `leetcode_app`       |
| GPay_LLD         | `main.cpp`                        | `gpay_app`           |
| Truecaller_LLD   | `main.cpp`                        | `truecaller_app`     |

> Lesson folders (`L*`) me often header-only demos hain — unke liye specific `.cpp` file compile karo with `-std=c++17`.

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
| [`books/Gang of Four.pdf`](./books/Gang%20of%20Four.pdf)     | Original design patterns book                      |
| [`books/`](./books/) DDIA PDF                                | Scalability, storage, distributed systems thinking |
| [`assets/sheets/`](./assets/sheets/) `.webp` cheat sheets    | Quick visual revision                              |
| [`docs/Design_Pattern_types.md`](./docs/Design_Pattern_types.md) | Creational/Structural/Behavioral taxonomy (Hindi) |
| [`assets/images/`](./assets/images/) pattern/problem PNGs    | Interview aids                                     |
| Per-lesson `Notes.pdf`, `UML.jpeg`                           | Lesson-specific diagrams                           |
| [`Exception_Handling/`](./Exception_Handling/)               | [**Complete guide**](./Exception_Handling/EXCEPTION_HANDLING_COMPLETE.md) + 14 C++17 demos |

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
| LeetCode                  | Contests, editorial service, real sandbox `ICodeRunner` |
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
| File Manager (VFS)   | [File_Manager_LLD](./File_Manager_LLD/)                           |
| OTP Generation       | [OTP_Generation_System_LLD](./OTP_Generation_System_LLD/)         |
| Thread-Safe TTL Cache | [Thread_Safe_Cache_with_TTL_LLD](./Thread_Safe_Cache_with_TTL_LLD/) |
| Concurrent HashMap | [Concurrent_HashMap_LLD](./Concurrent_HashMap_LLD/) |
| Amazon Locker Service | [Amazon_Locker_Service_LLD](./Amazon_Locker_Service_LLD/) |
| GPay (UPI P2P)       | [GPay_LLD](./GPay_LLD/)                                           |
| Truecaller           | [Truecaller_LLD](./Truecaller_LLD/)                               |
| Meeting Scheduler    | [Meeting_Scheduler_LLD](./Meeting_Scheduler_LLD/)                 |
| Pub-Sub System       | [Pub_Sub_System_LLD](./Pub_Sub_System_LLD/)                       |
| Airline Management   | [Airline_Management_System_LLD](./Airline_Management_System_LLD/) |
| LinkedIn             | [Linkedin_LLD](./Linkedin_LLD/)                                   |
| Load Balancer        | [LoadBalancer_LLD](./LoadBalancer_LLD/)                           |
| Logger               | [Logger_LLD](./Logger_LLD/)                                       |
| Movie Ticket Booking | [Movie_Ticket_Booking_System](./Movie_Ticket_Booking_System/)     |
| Parking Lot          | [Parking_lot_system_LLD](./Parking_lot_system_LLD/)               |
| Rate Limiter         | [Rate_Limiter_LLD](./Rate_Limiter_LLD/)                           |
| LRU Cache            | [LRU_Cache_LLD](./LRU_Cache_LLD/)                                 |
| LFU Cache            | [LFU_Cache_LLD](./LFU_Cache_LLD/)                                 |
| LeetCode             | [LeetCode_LLD](./LeetCode_LLD/)                                   |
| OYO Hotel Booking    | [OYO_Hotel_Booking_LLD](./OYO_Hotel_Booking_LLD/)                 |
| Ride Sharing         | [Ride_sharing_app_LLD](./Ride_sharing_app_LLD/)                   |
| Uber                 | [Uber_LLD](./Uber_LLD/)                                           |
| URL Shortener        | [URL_Shortner_LLD](./URL_Shortner_LLD/)                           |
| Vending Machine      | [vending_machine_LLD](./vending_machine_LLD/)                     |
| WhatsApp             | [WhatsApp_LLD](./WhatsApp_LLD/)                                   |
| Insta/YouTube Reels  | [Insta_reel_LLD](./Insta_reel_LLD/yt%20reel%20architecture/)      |
| Multi-threading      | [Multi_threading_C++](./Multi_threading_C%2B%2B/) — [Patterns](./Multi_threading_C++/02_Concurrency_Patterns/) · [Deadlock](./Multi_threading_C++/04_Concurrency_Challenges/Deadlock/) · [Fizz Buzz](./Multi_threading_C++/06_Interview_Problems/Fizz_Buzz/) · [Merge Sort](./Multi_threading_C++/06_Interview_Problems/Merge_Sort/) |

---

## System Projects — Deep Dive & Interview Bank

Har system ke liye **entities**, **main APIs**, **patterns**, aur **top interview questions** — revision ke liye one-stop.

### GPay_LLD

| Entities | `User`, `BankAccount`, `Wallet`, `Transaction`, `MoneyRequest`, `Beneficiary` |
| -------- | ----------------------------------------------------------------------------- |
| Enums | `PaymentRail`, `TransactionType`, `TransactionStatus` |
| Services | `TransferService`, `PinAuthService`, `DailyLimitService`, `TransactionLedgerService`, `RequestMoneyService` |
| Strategies | `BankAccountRailStrategy`, `WalletRailStrategy` |
| Facade | `GPaySystem` |

**Flow:** `registerUser` → `linkBankAccount` → `sendMoney` / `scanAndPay` / `requestMoney` → `fulfillMoneyRequest` → `getTransactionHistory`

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Idempotent payment? | `clientRequestId` dedup set before debit |
| Bank vs wallet rail? | **Strategy** — same `TransferService`, different `IPaymentRailStrategy` |
| vs L23 Payment Gateway? | GPay = consumer UPI P2P; L23 = merchant gateway (Paytm/Razorpay) |
| Real NPCI integration? | Async callback, UPI switch, double-entry ledger (extension) |

**UML:** [§25 GPay](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#25-gpay-upi-p2p) · **Patterns:** [GPay in PROJECT_DESIGN_PATTERNS](docs/PROJECT_DESIGN_PATTERNS.md#gpay-upi-p2p)

```bash
cd GPay_LLD && ./compile.sh && ./gpay_app
```

---

### Truecaller_LLD

| Entities | `User`, `PhoneProfile`, `ContactEntry`, `CallerLookupResult`, `CallLogEntry` |
| -------- | ---------------------------------------------------------------------------- |
| Enums | `CallerTag`, `ReportReason`, `CallType`, `AccountStatus` |
| Services | `LookupService`, `ContactSyncService`, `SpamReportService`, `BlockService`, `SearchService`, `CallLogService` |
| Strategy | `ISpamScoringStrategy` → `DefaultSpamScoringStrategy` |
| Facade | `TruecallerSystem` |

**Flow:** `registerUser` → `syncContacts` → `identifyCaller` → `reportSpam` / `blockNumber` → `logCall`

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Spam score kaise? | Crowd reports + strategy; threshold → `CallerTag::SPAM` |
| Privacy / PII? | Phone normalize `+91…`; production me consent + encryption |
| Global directory scale? | Sharded phone index, CDN for profile cache |

**UML:** [§26 Truecaller](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#26-truecaller) · **Patterns:** [Truecaller in PROJECT_DESIGN_PATTERNS](docs/PROJECT_DESIGN_PATTERNS.md#truecaller)

```bash
cd Truecaller_LLD && ./compile.sh && ./truecaller_app
```

---

### Meeting_Scheduler_LLD

| Entities | `User`, `Meeting`, `AvailabilityWindow`, `TimeSlot` |
| -------- | --------------------------------------------------- |
| Services | `AvailabilityService`, `ConflictDetectionService`, `BookingService`, `SlotFinderService` |
| Strategy | `EarliestMutualSlotStrategy` |
| Facade | `MeetingSchedulerSystem` |

**Flow:** `registerUser` → `setAvailability` → `findMutualFreeSlots` → `scheduleMeeting` → `cancelMeeting`

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Double booking? | `ConflictDetectionService` — overlap on active meetings per participant |
| Outside 9–5? | Slot must fit inside an `AvailabilityWindow` for every participant |
| Calendly vs this? | Same core; add timezone + recurring + reminders in HLD |

**UML:** [§27 Meeting Scheduler](./docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#27-meeting-scheduler)

```bash
cd Meeting_Scheduler_LLD && ./compile.sh && ./meeting_scheduler_app
```

---

### ATM_LLD

| Entities | `BankAccount`, `Card`, `Transaction` |
| -------- | ------------------------------------ |
| Enums | `TransactionType`, `TransactionStatus` |
| Services | `AuthenticationService`, `CashDispenser` (greedy notes) |
| Facade | `ATMSystem` |

**Flow:** `addAccount` → `addCard` → `login(card, pin)` → `getBalance` / `withdraw` → `logout`

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Invalid PIN kitni baar try? | Extension — lock card after N attempts |
| Concurrent withdraw same account? | Mutex per account / DB transaction |
| Cash dispenser notes khatam? | Greedy fail → partial dispense policy |

```bash
cd ATM_LLD && ./compile.sh && ./atm_app
```

---

### Parking_lot_system_LLD

| Entities | `Vehicle`, `ParkingSpot`, `Ticket` |
| -------- | ---------------------------------- |
| Strategy | `PricingStrategy` → `HourlyPricingStrategy` |
| Facade | `ParkingLot` |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Multiple floors? | `ParkingLot` per floor or composite floor manager |
| Electric vehicle spots? | New `SpotType` + strategy pricing |
| Full lot behavior? | Return error / waitlist enum |

---

### Movie_Ticket_Booking_System

| Managers | `CatalogManager`, `ShowManager`, `BookingManager` |
| -------- | --------------------------------------------------- |
| Patterns | Factory (`BookingFactory`), Strategy (`PricingStrategy`) |
| Device | `KioskTerminal` |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Two users same seat? | `mutex` on `Show` seat map / optimistic locking |
| Dynamic pricing? | Strategy per show/time slot |
| Refund on cancel? | State on `Booking` + payment reversal service |

---

### Rate_Limiter_LLD

| Algorithms | Token Bucket, Fixed Window, Sliding Window Log |
| ---------- | ---------------------------------------------- |
| Patterns | Strategy + Factory |
| Concurrency | `mutex` + demo `Barrier` / `Latch` in `Main.cpp` |

| Algorithm | Pros | Cons |
| --------- | ---- | ---- |
| Token Bucket | Smooth burst | State per user |
| Fixed Window | Simple | Boundary spike at window edge |
| Sliding Window Log | Accurate | Memory per request timestamp |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Distributed rate limit? | Redis + Lua atomic incr + TTL |
| Premium vs Free tier? | `UserTier` → different `RateLimitConfig` |
| Thread-safe `allowRequest`? | Lock map entry or shard by userId hash |

```bash
cd Rate_Limiter_LLD && ./compile.sh && ./rate_limiter_app
```

---

### LRU_Cache_LLD vs LFU_Cache_LLD

| Aspect | LRU | LFU |
| ------ | --- | --- |
| Eviction | Least recently **used** | Least frequently **used** |
| DS | `unordered_map` + `list` + `splice` | Freq buckets + `minFreq` |
| Thread safety | `ThreadSafeLRUCache` mutex | `ThreadSafeLFUCache` mutex |
| Use case | Temporal locality | Hot key protection (CDN, API cache) |
| Tie-break | N/A | LRU within same frequency |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| `get()` updates order? | Haan — LRU me get = touch = move to front |
| O(1) guarantee? | Map + list pointers; LFU bucket lists |
| Sharded cache? | `hash(key) % N` → N independent caches |

```bash
cd LRU_Cache_LLD && ./compile.sh && ./lru_cache_app
cd LFU_Cache_LLD && ./compile.sh && ./lfu_cache_app
```

---

### LeetCode_LLD

| Entities | `User`, `Problem`, `Submission`, `TestCase` |
| -------- | ------------------------------------------- |
| Enums | `Difficulty`, `SubmissionStatus`, `ProgrammingLanguage` |
| Services | `ProblemCatalogService`, `SubmissionService`, `JudgeService`, `LeaderboardService` |
| Strategy | `ICodeRunner` → `MockCodeRunner` |
| Algorithms | `MinCostDivideArraySolver` (HARD — O(n²) DP) |
| Facade | `LeetCodeSystem` |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Real code execution? | Sandbox workers + isolate (HLD); LLD uses `MockCodeRunner` |
| Distributed judge? | Queue (Kafka/SQS) + worker pool + result callback |
| Partial test cases? | Stop at first WA/TLE; return failed `testCaseId` |
| Contest support? | `ContestService` + separate leaderboard (extension) |
| Hard DP problem? | Min cost divide array — optimized `dp[i]` formula |

```bash
cd LeetCode_LLD && ./compile.sh && ./leetcode_app
```

---

### LoadBalancer_LLD

| Strategies | `RoundRobinStrategy`, `LeastConnectionsStrategy` |
| ---------- | ------------------------------------------------ |
| Server state | `ServerStatus` UP/DOWN, connection counts |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Sticky sessions? | Hash client IP → fixed server index |
| Health checks? | Background thread marks DOWN; skip in strategy |
| Weighted round robin? | Extend strategy with weight vector |

---

### Logger_LLD

| Patterns | Singleton, Chain of Responsibility, Strategy (formatters), Observer |
| -------- | --------------------------------------------------------------------- |
| Chain | Debug → Info → Warn → Error → Fatal |
| Output | `ConsoleAppender`, `FileAppender` |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Async logging? | Producer queue + background writer thread |
| Log level runtime change? | Handler chain reconfiguration |
| Structured logs? | `JsonFormatter` implements formatter interface |

```bash
cd Logger_LLD && ./compile.sh && ./logger_app
```

---

### WhatsApp_LLD

| Feature | Implementation |
| ------- | ---------------- |
| 1:1 chat | `Chat` in `ChatService` |
| Groups | `Group` + admin |
| Message status | `MessageStatus` enum |
| Encryption | `EncryptionService` Strategy (demo stub) |
| Notifications | `NotificationEngine` + decorators |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Message ordering? | Sequence number per chat (extension) |
| Read receipts? | Observer on message status |
| Real E2E? | Key exchange + encrypt at client — not in this LLD |
| Group add member? | Admin check on `Group` |

---

### Uber_LLD vs Ride_sharing_app_LLD

| Feature | Ride_sharing | Uber_LLD |
| ------- | ------------ | -------- |
| Matching | Nearest driver | `DriverMatchingService` |
| OTP | — | `OtpService` |
| Payment | Basic fare | `PaymentService` + status |
| Cancel ride | — | Supported |

Start with **Ride_sharing** (simpler), then **Uber** for richer lifecycle.

---

### JSON_Parser_LLD

| Pattern | **Composite** — `JsonObject`, `JsonArray`, primitives |
| ------- | ----------------------------------------------------- |
| Parser | Recursive descent `JsonParser::parse()` |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Invalid JSON? | Exception + line/col (extension) |
| Pretty print? | Visitor on composite tree |

---

### URL_Shortner_LLD

| Component | `UrlShortnerService`, `Base62Encoder` |
| --------- | -------------------------------------- |
| Features | Shorten, resolve, dedupe long URL, click count |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Collision on short code? | Retry / longer code |
| Custom alias? | Map alias → validate unique |

---

### Elevator_System_LLD

| Components | `ElevatorCar`, `ElevatorScheduler`, `ElevatorSystemController` |
| ---------- | -------------------------------------------------------------- |
| Requests | External (UP/DOWN) + internal floor buttons |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| SCAN vs nearest car? | Trade-off: fairness vs energy |
| Multiple elevators? | Scheduler picks idle + direction match |

---

### Linkedin_LLD

| Features | Connections (pending/accepted), posts, likes, feed |
| -------- | -------------------------------------------------- |
| Feed | `FeedService` — posts from connections |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Feed ranking? | Score = likes + recency (extension) |
| Connection limit? | Enum reject reason |

---

### Insta_reel_LLD (YouTube-style)

| Path | `Insta_reel_LLD/yt reel architecture/` |
| ---- | -------------------------------------- |
| Features | Upload reel, view/like/comment, follow graph, ranked feed |

| Interview Q | Answer sketch |
| ----------- | ------------- |
| Feed algorithm? | Views + likes weighted score |
| CDN for video? | HLD — object storage + edge (out of LLD scope) |

---

### Vending_machine_LLD

| State flow | Select → pay → dispense OR cancel/refund |
| ---------- | ---------------------------------------- |
| Components | `Inventory`, `MoneyManager`, `VendingMachine` |

Pair with **L32 State** pattern lesson for interview synergy.

---

### Library_Management_System_LLD

| Services | `CatalogService`, `FineService` |
| -------- | ------------------------------- |
| Rules | Borrow limits, late fines on return |

---

### Car_Rental_System_LLD

| Lifecycle | RESERVED → RENTED → AVAILABLE |
| --------- | ----------------------------- |
| Pricing | `days × dailyRate` via `PricingService` |

---

## Company-Wise Problem Mapping

| Company tier | Commonly asked LLD | Repo path |
| ------------ | ------------------ | --------- |
| FAANG / Big Tech | LRU Cache, Rate Limiter, Logger, Online Judge | `LRU_Cache_LLD`, `Rate_Limiter_LLD`, `Logger_LLD`, `LeetCode_LLD` |
| Amazon | Parking Lot, Locker (extension), Load Balancer | `Parking_lot_system_LLD`, `LoadBalancer_LLD` |
| Microsoft | Elevator, Logger, Cache | `Elevator_System_LLD`, `Logger_LLD` |
| Uber / Swiggy / Zomato | Ride, Food (L11), Movie Ticket | `Uber_LLD`, `L11 Food_Delivery`, `Movie_Ticket_Booking_System` |
| Meta / Social | WhatsApp, LinkedIn, Reels | `WhatsApp_LLD`, `Linkedin_LLD`, `Insta_reel_LLD` |
| Flipkart / E-commerce lessons | Payment Gateway L23, Coupon L24 | `L23`, `L24` |
| Fintech | ATM, Payment Gateway, UPI P2P, Splitwise | `ATM_LLD`, `L23`, `GPay_LLD`, `L31 Splitwise` |
| India mobile apps | UPI wallet, Caller ID / spam | `GPay_LLD`, `Truecaller_LLD` |
| Startups (general) | URL Shortener, Vending Machine | `URL_Shortner_LLD`, `vending_machine_LLD` |

> Yeh mapping **heuristic** hai — actual interviews company/team par vary karte hain. Repo se **pattern reuse** seekho, exact company list yaad karne ki zarurat nahi.

---

## 12-Week Study Calendar

| Week | Focus | Deliverable |
| ---- | ----- | ----------- |
| 1 | L1–L4 OOP + UML | Draw 3 class diagrams from memory |
| 2 | L5–L6 SOLID | Refactor one violated example verbally |
| 3 | L8–L10 Strategy, Factory, Singleton | Implement Strategy for new domain (5 min) |
| 4 | L11–L14 Food + Notification | Explain Facade vs God class |
| 5 | L15–L20 Command → Template | One pattern per day code trace |
| 6 | L21–L26 Proxy → Blinkit | Payment Gateway whiteboard |
| 7 | **Parking Lot** + **Movie Ticket** | Run + 10 interview Q answers written |
| 8 | **LRU** + **Rate Limiter** + **Logger** | Compare algorithms table from memory |
| 9 | **Uber** + **WhatsApp** + **Load Balancer** | Sequence diagram explain karo |
| 10 | L31 Splitwise + L37 Chess | Debt simplify algorithm walkthrough |
| 11 | Remaining systems (ATM, JSON, URL, Elevator…) | `./compile.sh` har project |
| 12 | Mock interviews + **Multi_threading_C++** | 2 full LLD mocks timed 45 min |

**Daily habit (45 min):** 15 min read `problem_statement.md` → 15 min code trace → 15 min extensions list.

---

## FAQ — Frequently Asked Questions

### General

**Q: Kya yeh repo HLD cover karti hai?**  
A: Partially via DDIA pdf aur extension discussions. Primary focus **LLD** (classes, patterns, in-memory).

**Q: Database kyun nahi hai?**  
A: Interview LLD usually in-memory + interfaces. Persistence = Repository pattern extension.

**Q: `bits/stdc++.h` kyun use hota hai?**  
A: Competitive / interview speed ke liye. Compile with `-std=c++17`. Production me explicit headers prefer karo.

**Q: CMake kahan hai?**  
A: Kuch projects (`LFU_Cache_LLD`) me hai; zyada tar `./compile.sh` ya single-file `g++` compile.

### Technical

**Q: Smart pointers vs raw pointers?**  
A: L18 Spotify notes intentional mix. New code me `unique_ptr` / `shared_ptr` prefer karo.

**Q: Thread safety har project me?**  
A: Nahi — sirf jahan demo hai (Rate Limiter, LRU/LFU, Multi_threading). Baaki me extension exercise.

**Q: WhatsApp me real encryption hai?**  
A: Nahi — `EncryptionService` Strategy stub; `enc(text)` demo. Real E2E = keys + Signal protocol (HLD/Security).

**Q: LRU vs LFU interview me kaun puchega?**  
A: Dono — LRU zyada common; LFU CDN / cache warming scenarios me.

### Preparation

**Q: Kitne projects enough?**  
A: **8–10 deep** + baaki skim. Priority: Parking, Movie, LRU, Rate Limiter, Splitwise, Logger, Uber/WhatsApp.

**Q: Diagrams banana zaroori?**  
A: Haan — [`SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md`](docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md) se practice, phir blank paper.

**Q: Mock interview kaise?**  
A: 45 min timer → requirements → classes → 2 patterns → code skeleton → trade-offs. Record yourself.

---

## Troubleshooting — Build & IDE

| Problem | Fix |
| ------- | --- |
| `bits/stdc++.h` not found (macOS) | Install GCC: `brew install gcc`, use `g++-14` or create bits header in include path |
| `optional` / `variant` errors | Add `-std=c++17` — use `./compile.sh` |
| Undefined reference pthread | Add `-pthread` to compile command |
| clangd red squiggles on headers | Project `.clangd` has `-std=c++17 -pthread -I.` — restart clangd |
| `Main.cpp` vs `main.cpp` | Logger & Rate Limiter use capital `Main.cpp` |
| Insta Reels path has space | `cd "Insta_reel_LLD/yt reel architecture"` |
| Permission denied `compile.sh` | `chmod +x compile.sh` |

**Clean rebuild:**

```bash
rm -f *_app atm_app parking_app  # project binaries
./compile.sh
```

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
| **Repository**| Abstraction over persistence — swap in-memory vs SQL              |
| **DDD-lite**  | `models/` + `services/` + `core/` folder convention in this repo  |
| **Cohesion**  | Classes that change together stay together                        |
| **Coupling**  | Minimize dependencies between modules — DIP helps                 |
| **Idempotent**| Same request twice = same effect (payments, bookings)           |
| **CAP**       | Consistency / Availability / Partition — HLD; mention in LLD scale Q |
| **Token Bucket** | Rate limit algo — tokens refill at fixed rate                |
| **Sliding Window** | Rate limit — count events in rolling time window           |
| **Composite** | Tree where leaf and container share interface (`JsonValue`)     |
| **Mediator**  | Central hub reduces N² connections (Chess chat, L35)            |
| **Flyweight** | Share intrinsic state across many objects (L30)                   |
| **Memento**   | Snapshot state for undo (L39, Chess extension)                    |
| **Anti-pattern** | God class, shotgun surgery, spaghetti — L40 notes            |

### Pattern selection cheat sheet

```
Need interchangeable algorithm?     → Strategy
Need one instance globally?         → Singleton (careful)
Need create family of objects?      → Factory / Abstract Factory
Need add behavior without subclass? → Decorator
Need notify many on change?         → Observer
Need tree structure uniform API?    → Composite
Need simplify complex subsystem?    → Facade
Need pass request along handlers?   → Chain of Responsibility
Need undo?                          → Command / Memento
```

---

## Changelog & Maintenance

| Date | Change |
| ---- | ------ |
| 2025 | Initial 40 lessons + 18 system projects |
| 2025 | Added `LRU_Cache_LLD`, `LFU_Cache_LLD`, `Multi_threading_C++/05_Classic_Problems/Dining_Philosophers/dining_philosophers.cpp` |
| 2025 | Added `SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md` (Mermaid UML) |
| 2025 | Per-project `compile.sh` + `.clangd` (C++17) |
| 2025 | Master README expanded to 1500+ lines — deep dive + interview bank |
| 2025 | Added `LeetCode_LLD` — online judge + HARD min-cost divide array DP |
| 2026 | Added `GPay_LLD` (UPI P2P) + `Truecaller_LLD` — §25–§26 in diagrams, pattern map updated |
| 2026 | Repo cleanup — `docs/`, `assets/`, `books/` (root ab sirf README + project folders) |
| 2026 | Added `Meeting_Scheduler_LLD` — §27 UML + pattern map |

**Maintainer checklist:**

- [ ] Har naye project me `problem_statement.md`, `requirements.md`, `compile.sh`
- [ ] Root README me project table update
- [x] Diagram file me naya section add (§22–§26 incl. GPay, Truecaller)
- [ ] `./scripts/build_all_systems.sh` green before push

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
