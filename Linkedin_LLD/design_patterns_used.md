# Design Patterns Used — LinkedIn LLD

Is system me 13 features hain (profile, connections, follow, posts, engagement, feed,
messaging, notifications, search, analytics, recommendations, company pages/jobs, groups).
Itna sab **clean, decoupled aur extensible** rahe iske liye ek main structural pattern
(Facade), ek behavioural pattern (Observer), ek creational technique (fluent Builder), plus
ek layered service architecture aur ek chhota state machine use hua hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis requirement ke liye │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade          │  LinkedinSystem                 │  Poore system ka entry   │
│  Observer ⭐     │  NotificationService            │  R10 notifications       │
│  Builder (fluent)│  UserProfile add*() chaining    │  R1 profile customization│
│  Service Layer   │  Feed/Search/Analytics/Message/ │  R2,R5,R6,R9,FR6         │
│  (SRP)           │  Notification/Recommendation    │  decoupled logic         │
│  State machine   │  ConnectionRequest + status     │  R3 request lifecycle    │
│  Composition     │  CompanyPage◆Job, Post◆Comment  │  R8,R11,R12,R13 ownership │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade Design Pattern (system ka single entry point)

### 📌 Concept
Facade ek complex subsystem (bahut saari classes/services) ke aage ek **patla, saaf
interface** rakh deta hai — client ko andar ke moving parts se matlab hi nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`LinkedinSystem`](./core/LinkedinSystem.h)
- **Subsystem** → 6 services (Feed, Search, Analytics, Notification, Messaging, Recommendation) + ~10 models

### Implementation
Client ([`main.cpp`](./main.cpp)) me se kitni classes dikhti hain? Bas `LinkedinSystem`,
`UserProfile`, aur kuch enums. Kisi service ka naam tak nahi aata. Client bas:
```cpp
system.sendConnectionRequest("U1", "U2");   // andar: validation + notify (Observer)
system.reactToPost("U1", post, ReactionType::CELEBRATE); // andar: Post + notify author
auto a = system.getAnalytics("U1");         // andar: AnalyticsService 4 jagah se jodta hai
```

### ⭐ KYUN? (Why Facade)
> Facade khud **data ka maalik** hai (users, connections, posts, pages, groups) aur services
> ko wo data pass karta hai. Isse zyadatar services **stateless** reh paati hain (single
> source of truth ek jagah). Client aur subsystem poore decoupled — kal ko koi service
> badle, client ka ek line nahi badlega.

---

## 2. Observer Design Pattern ⭐ (R10 ka dil)

### 📌 Concept
One-to-Many: jab **Subject** me koi event hota hai, uske saare registered **Observers** ko
automatically khabar kar di jaati hai — Subject ko ye tak nahi pata observer kaun hai aur kya
karega.

### PATTERN KE ROLES
- **Subject** → [`NotificationService`](./services/NotificationService.h) — events aate hain, sabko batata hai + inbox me store karta hai
- **Observer interface** → `INotificationObserver` (`onNotification`)
- **Concrete Observer** → `ConsoleNotificationObserver` (console pe 🔔 print)

### Implementation flow (R10 ke teen kaaran, bilkul image jaise)
```
sendConnectionRequest ─┐
sendMessage           ─┼─►  NotificationService.notify(recipient, type, text)
react/comment/share   ─┘          │  (1) inbox_[recipient] me STORE
                                  │  (2) sab observers ko BROADCAST
                                  ▼
                     ConsoleNotificationObserver ──►  🔔 print
```

### ⭐ KYUN? (Why Observer)
> Aaj sirf console pe print hota hai. Kal ko **Email / SMS / Push** chahiye? Bas ek naya
> `EmailNotificationObserver : INotificationObserver` bana ke `registerObserver()` kar do —
> `NotificationService` ka ek line bhi nahi badlega. **Open/Closed Principle.** Notification
> "kaise" bheji jaaye wo channels se decouple ho gaya.

---

## 3. Builder Pattern — fluent interface (R1 profile customization)

### 📌 Concept
Jab ek object ke bahut saare **optional** fields hon, to unhe ek-ek karke set karne ka saaf
tareeka — har setter apna hi reference laut aaye taaki calls "chain" ho sakein.

### Implementation
[`UserProfile`](./models/UserProfile.h) ke `add*` methods `UserProfile&` lautate hain:
```cpp
UserProfile("U1", "Shubham", "Backend Engineer")
    .addEducation({"IIT Delhi", "B.Tech CSE", "2021"})
    .addExperience({"SDE-II", "Google", "2022-Present"})
    .addSkill("C++").addSkill("System Design")
    .addAchievement("Built a scalable booking engine");
```

### ⭐ KYUN? (Why fluent Builder)
> R1 me profile ke bahut saare optional hisse hain (education, experience, skills,
> achievements — kisi ke paas 0, kisi ke 5). Constructor me sab daalte to 8+ parameter ka
> bhayanak constructor banta. Fluent chaining se ek saaf, padhne-layak line me jitna chahiye
> utna set ho jaata hai — aur zaroori (id/name/headline) constructor me, optional chaining se.

---

## 4. Service Layer / Single Responsibility (SRP)

### 📌 Concept
Business logic ko chhoti, **ek-kaam-wali** services me tod do. Facade orchestrate karta hai;
har service apna ek hi domain sambhalti hai.

| Service | Ek hi zimmedari | Requirement | State? |
|---------|-----------------|-------------|--------|
| [`FeedService`](./services/FeedService.h) | Personalized feed banao + rank karo | FR6 | stateless |
| [`SearchService`](./services/SearchService.h) | Users/pages/groups dhoondho | R2 | stateless |
| [`AnalyticsService`](./services/AnalyticsService.h) | 4 insight numbers jodo | R5 | stateless |
| [`NotificationService`](./services/NotificationService.h) | Events broadcast + inbox | R10 | owns inbox |
| [`MessagingService`](./services/MessagingService.h) | Messages store/retrieve | R9 | owns messages |
| [`RecommendationService`](./services/RecommendationService.h) | Request + provide recos | R6 | owns recos |

### ⭐ KYUN? (Why Service Layer)
> Har feature apni class me → **test karna, badalna, padhna aasaan.** Stateless services
> (Feed/Search/Analytics) ko facade data deta hai, wo hisaab laut deti hain — koi hidden
> state nahi. `FeedService` ka ranking logic ek hi jagah hai, to kal ko **ML ranking** chahiye
> to bas wahi service badlegi (ye ek natural **Strategy** extension point hai).

---

## 5. State Machine — ConnectionRequest lifecycle (R3)

### 📌 Concept
Ek entity kuch fixed states me hi ho sakti hai, aur transitions ke fixed rules hote hain.

### Implementation
[`ConnectionRequest`](./models/ConnectionRequest.h) ka status ([`ConnectionStatus`](./enums/ConnectionStatus.h)):
```
                 accept ─►  ACCEPTED   (dono connected)
                 reject ─►  REJECTED
   PENDING ──────ignore ─►  IGNORED    (receiver ne andekha kiya)
                 cancel ─►  CANCELLED  (sender ne wapas li)
```
> **⭐ Rule:** transition SIRF `PENDING` se hota hai. Terminal state me pahunchne ke baad
> request kabhi nahi badalti. Ye guard (`requirePending`) facade me ek jagah hai — isi se
> "already processed" wale duplicate/invalid actions apne aap block ho jaate hain.

### Kaun action kar sakta hai (authorization bhi state ka hissa)
- `cancel` → sirf **FROM** user (jisne bheji)
- `accept` / `reject` / `ignore` → sirf **TO** user (jise mili)

---

## Object Relationships (UML samajhne ke liye)

```
LinkedinSystem   ◆──  6 services              (Composition: facade owns)
LinkedinSystem   ◆──  users/connections/posts (Composition: single source of truth)
CompanyPage      ◆──  JobListing (vector)     (Composition: job page ke bina exist nahi karti)
Post             ◆──  Comment (vector)        (Composition)
Group            ◆──  members (set)           (Composition)
UserProfile      ◆──  Education / Experience  (Composition: value members)
NotificationSvc  ◇──  INotificationObserver   (Aggregation: register kiye observers)
FeedService      ··▶  Post                    (Dependency: pointer se kaam, own nahi karta)
```
- `◆──` **Composition** (owns-a, lifetime bandha) · `◇──` **Aggregation** (has-a, injected) · `··▶` **Dependency** (uses temporarily)

---

## Requirement → Pattern quick map

| Requirement | Pattern / Mechanism |
|-------------|---------------------|
| R1 profile customization | **Builder** (fluent UserProfile) |
| R2 search | **Service Layer** (SearchService) |
| R3 connection mgmt | **State machine** (ConnectionRequest) |
| R4 follow | one-way graph in Facade |
| R5 analytics | **Service Layer** (AnalyticsService, stateless aggregator) |
| R6 recommendations | **Service Layer** (RecommendationService) |
| R7 content | Facade + `Post` model |
| R8 engagement | `Post`/`Comment` (reactions map, composition) |
| R9 messaging | **Service Layer** (MessagingService) |
| R10 notifications | **Observer** (NotificationService) |
| R11 company pages | Facade + `CompanyPage` |
| R12 job listings | **Composition** (Page ◆ JobListing) |
| R13 groups | Facade + `Group` |
| Sab kuch ek jagah se | **Facade** (LinkedinSystem) |

---

## "Ye pattern kyun NAHI use kiya" (interview me poocha jaata hai)

- **Feed ko poora Strategy kyun nahi banaya (IFeedStrategy interface)?** Abhi ek hi ranking hai
  (engagement + recency), to interface banana over-engineering hota. `FeedService` ise ek
  jagah band rakhti hai — jis din doosri ranking chahiye, tab `IFeedStrategy` nikaal ke
  Strategy bana denge. **YAGNI** (You Aren't Gonna Need It) — abhi zaroorat nahi.
- **Comment aur Post ka common base class (`Reactable`) kyun nahi?** Dono pe react hota hai
  (thoda common code), par baaki sab alag hai. Early abstraction aksar baad me dard deta hai —
  "2 baar dikhe to notice, 3 baar dikhe to abstract." Abhi 2 hi hain.
- **Singleton (ek hi LinkedinSystem)?** Ek instance banana `main()` me `LinkedinSystem system;`
  se ho jaata hai. Singleton global state deta jo testing mushkil karta — zaroorat na ho to mat lagao.
