# L27 Tinder (Dating App) — Design Patterns Used (Hinglish)

> Tinder-style dating app ka LLD — users, location-based discovery, swipe, mutual-match, chat. Is folder me **do versions** hain: ek **modular** (simple, mainly Facade) aur ek **monolith** (`C++ Code/Tinder_LLD.cpp` — feature-rich, **5 patterns**). Dono ko honestly cover karta hoon.

---

## Do Versions Ka Farq (pehle ye samajh lo)

| Aspect | Modular (`core/`, `models/`...) | Monolith (`C++ Code/Tinder_LLD.cpp`) |
|--------|--------------------------------|--------------------------------------|
| Matching | Simple distance-based | **Strategy** — 3 matchers (Basic/Interests/Location) |
| Notifications | Nahi | **Observer** pattern |
| Matcher creation | N/A | **Factory** pattern |
| Managers | Plain classes | **Singleton** (NotificationService/LocationService/DatingApp) |
| Entry point | Facade (TinderSystem) | Facade (DatingApp) |
| Best for | Clean structure samajhne | Pattern-density seekhne |

---

## Patterns — Ek Nazar Me

| # | Pattern | Version | Kahan |
|---|---------|---------|-------|
| 1 | **Facade** | Both | [`core/TinderSystem.h`](./core/TinderSystem.h) / monolith `DatingApp` |
| 2 | **Strategy** | Monolith | `Matcher` (3 matchers) + `LocationStrategy` |
| 3 | **Observer** | Monolith | `NotificationService` + `NotificationObserver` |
| 4 | **Factory** | Monolith | `MatcherFactory` |
| 5 | **Singleton** | Monolith | NotificationService, LocationService, DatingApp |

---

## 1. Facade — System ka orchestrator 🚪 (dono versions)

**Kahan:** [`core/TinderSystem.h`](./core/TinderSystem.h) (modular) / `DatingApp` (monolith)

**Problem jo solve hua:** Client ko agar khud users, matching, chat rooms, blocks, swipe limits — sab manage karna padta, to har jagah wahi complex sequence.

**Kaise:** TinderSystem ek clean API deta — `registerUser`, `discoverProfiles`, `swipe`, `sendMessage`. Har method ke peeche multiple steps chhupe:

```
swipe("user1", "user2", RIGHT)   // client ko sirf ye dikhta
   │
   ├── self-swipe check
   ├── block check (isBlockedEitherWay)
   ├── daily limit check (consumeSwipe: 10 swipe/2 super-like)
   ├── swipe record (user history)
   └── MUTUAL like? -> ChatRoom banao (MATCH!)
```

**Match detection ka core:** jab A swipe kare B pe, system check karta `B->liked(A)?`. Agar dono ne RIGHT/SUPER_LIKE kiya = **mutual = MATCH** = ChatRoom. (L17 Facade dekho.)

---

## 2. Strategy — Matching algorithm 🔀 (monolith)

**Kahan:** `Matcher` interface + 3 concrete matchers

Matching ka tareeka swap-able hai — 3 matchers, **cumulative scoring** ke saath:

```
BasicMatcher          -> gender + age + distance preferences match? -> 0.5 base
   │ (InterestsBasedMatcher isse call karta hai)
   ▼
InterestsBasedMatcher -> BasicMatcher score + shared interests bonus (+0.5 max)
   │ (LocationBasedMatcher isse call karta hai)
   ▼
LocationBasedMatcher  -> InterestsMatcher score + proximity bonus (+0.2 max)
```

**Smart design:** har matcher pichle wale ko call karke score BUILD karta (Basic → Interests → Location). Isse har level apna criteria ADD karta previous ke upar. `MatcherType` se choose hota kaunsa use karna.

`LocationStrategy` bhi Strategy — nearby users kaise dhundhein (swap-able location logic).

---

## 3. Observer — Notifications 📢 (monolith)

**Kahan:** `NotificationService` (subject) + `NotificationObserver` / `UserNotificationObserver`

**Problem jo solve hua:** Match ya message hone pe users ko notify karna — agar ye har jagah hard-code hota to naya notification type = har jagah edit.

**Kaise:** `NotificationService` (observable) ke paas observers ki list. Event hone pe sab observers ko notify:

```
match hua -> NotificationService.notify() -> sab observers ka update()
                                             -> UserNotificationObserver: user ko batao
```

Naya reaction (email, push) = naya observer, subject untouched. (L12 Observer dekho.)

---

## 4. Factory — Matcher creation 🏭 (monolith)

**Kahan:** `MatcherFactory`

`MatcherType` (BASIC/INTERESTS_BASED/LOCATION_BASED) do, sahi matcher object lo. Client ko concrete matcher class ka naam nahi pata — factory decide karta. Naya matcher = ek case + nayi class. (L9 Factory dekho.)

---

## 5. Singleton — Central services 1️⃣ (monolith)

`NotificationService`, `LocationService`, `DatingApp` — sab singletons (ek-ek instance poore app me). Do notification services ya do dating apps = confusion. (L10 Singleton dekho.)

---

## Domain Highlights (dono versions me achhe) ⭐

1. **🌍 Haversine distance** ([`models/Location.h`](./models/Location.h)): do GPS points ke beech REAL distance — Earth ki curvature account karke (flat Pythagoras nahi!). Location-based apps (Tinder/Uber/food) ka standard.

2. **Chat room key trick** ([`core/TinderSystem.h`](./core/TinderSystem.h)): "CHAT_a_b" key, par `getChatRoom` DONO orders check karta (a_b aur b_a) — warna "user1->user2" aur "user2->user1" alag rooms ban jaate.

3. **Read receipt logic** ([`models/ChatRoom.h`](./models/ChatRoom.h)): `markMessagesReadFor(reader)` sirf DUSRE ke messages read marks (apne nahi) — WhatsApp jaisa blue-tick behavior.

4. **swipeHistory = match engine** ([`models/User.h`](./models/User.h)): `liked()` + `interacted()` se mutual-match detection aur "already seen" (dobara nahi dikhana) dono.

5. **Rate limiting**: daily swipe (10) + super-like (2) limits — Tinder ka free-tier feel.

---

## Design Limitations (interview me bolne layak) ⚠️

1. **Daily limits reset nahi hote:** `dailySwipeCount_` counters kabhi reset nahi (koi "naya din" logic nahi). Real me midnight reset ya timestamp-based chahiye.

2. **Modular version simple hai:** sirf distance-based matching (no preferences/interests). Monolith zyada realistic (Strategy-based multi-criteria matching).

3. **In-memory only:** sab data RAM me (restart pe gayab). Real me DB persistence chahiye.

---

## Files Structure

```
L27 Tinder_LLD/
├── design_patterns_used.md      ← Ye file
├── main.cpp                     ← modular demo driver
├── core/TinderSystem.h          ← FACADE (orchestrator)
├── services/MatchingService.h   ← nearby discovery (service layer)
├── models/
│   ├── User.h                   ← user + swipe history (match engine)
│   ├── UserProfile.h            ← profile data
│   ├── Location.h               ← Haversine distance
│   ├── Message.h                ← chat message + read flag
│   └── ChatRoom.h               ← matched-users chat
├── enums/                       ← Gender, SwipeAction
└── C++ Code/Tinder_LLD.cpp      ← MONOLITH (5 patterns — feature-rich)
```

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Daily limits ko timestamp-based reset (midnight)
- Modular version me bhi Strategy-based matching laao (monolith se port)
- Observer for match/message notifications (modular me nahi hai)
- Raw pointers → smart pointers
- Preference filtering (gender/age range) modular version me add karo

---

**Related lessons:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L10 Singleton](../L10%20Singleton_Design_Pattern/) · [L12 Observer](../L12%20Observer_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/)
