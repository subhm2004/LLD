# L14 Notification Engine — Design Patterns Used (Hinglish)

> Ek notification system jisme **4 design patterns ek saath** kaam karte hain — ye is repo ka sabse achha "patterns milke kaam karte hain" ka example hai! Ek message banta hai, **decorate** hota hai, aur ek event pe **kai channels** (Email/SMS/Popup) + logger tak pahunchta hai. Neeche har pattern ka role, kahan, aur kaise — detail me.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Role | Kahan (modular) |
|---|---------|------|------|-----------------|
| 1 | **Observer** | Behavioral | Ek event → kai reactions (broadcast) | [`services/NotificationEngine.h`](./notification_lld/services/NotificationEngine.h) |
| 2 | **Strategy** | Behavioral | Channel pe bhejne ka tareeka swap-able | [`services/NotificationEngine.h`](./notification_lld/services/NotificationEngine.h) |
| 3 | **Decorator** | Structural | Message pe layer-by-layer content jodo | [`services/NotificationService.h`](./notification_lld/services/NotificationService.h) |
| 4 | **Singleton** | Creational | Ek hi central hub/observable | [`services/NotificationEngine.h`](./notification_lld/services/NotificationEngine.h) (NotificationHub) |

**Bonus:** [`core/NotificationSystem.h`](./notification_lld/core/NotificationSystem.h) ek **Facade** bhi hai — client ko sirf 3 simple methods dikhta hai.

---

## Poora Flow — 4 Patterns Ek Saath

```
sendNotification("Order Update", "shipped")
   │
   ▼
1. NotificationMessage banta hai (title/body/priority)
   │
   ▼
2. DECORATOR chain:
   SimpleNotification("Order Update: shipped")
      wrapped in TimestampDecorator     -> "[ts:...] Order Update: shipped"
         wrapped in SignatureDecorator   -> "...  -- NotificationService"
   │
   ▼
3. NotificationHub (SINGLETON) → observable.setNotification(decorated)
   │
   ▼
4. OBSERVER: observable sab observers ko update() karta hai:
      ├── LoggerObserver      → "[Logger] ..."
      └── NotificationEngine  → STRATEGY: har channel pe bhejo:
              ├── EmailStrategy → "[Email:...] ..."
              ├── SMSStrategy   → "[SMS:...] ..."
              └── PopUpStrategy → "[Popup] ..."
```

**Output:** ek message **4 baar** dikhta hai (Logger + 3 channels) — bina main.cpp me koi loop likhe. Yahi patterns ka combined power!

---

## 1. Observer — "ek event, kai reactions" 📢

**Kahan:** `NotificationObservable` (subject) + `LoggerObserver`, `NotificationEngine` (observers)

**Problem jo solve hua:** Naya notification aaye to kai cheezein react karni chahiye — log ho, channels pe jaaye, shayad analytics update ho. In sabko `sendNotification()` me hard-code karte to har naye reaction pe wo function edit hota.

**Kaise:** Observable ke paas observers ki list. `setNotification()` hote hi:

```cpp
void setNotification(...) {
    currentNotification_ = notification;
    notifyObservers();   // SAB observers ka update() call
}
```

Naya reaction chahiye (jaise `AnalyticsObserver`)? Bas `addObserver()` — subject ko chhedna nahi padta. **Loose coupling:** subject observers ko concretely nahi jaanta, sirf `IObserver` interface se.

---

## 2. Strategy — channel pe bhejne ka tareeka 📡

**Kahan:** `INotificationStrategy` + `EmailStrategy`, `SMSStrategy`, `PopUpStrategy`

**Problem jo solve hua:** Notification alag channels pe alag tareeke se jaata hai (email vs SMS vs popup). Engine me ye if-else hota to naya channel = engine edit.

**Kaise:** `NotificationEngine` ke paas strategies ki list. `update()` aane pe:

```cpp
for (auto& strategy : strategies_) {
    strategy->send(content);   // har channel apna tareeka
}
```

Naya channel (WhatsApp, Push)? Nayi strategy class + `addNotificationStrategy()`. Engine untouched. OCP ✅

---

## 3. Decorator — message ko "sajao" layer-by-layer 🎁

**Kahan:** `INotification` + `SimpleNotification` (base) + `TimestampDecorator`, `SignatureDecorator`. Assembly [`NotificationService.h`](./notification_lld/services/NotificationService.h) ke `send()` me.

**Problem jo solve hua:** Message me kabhi timestamp chahiye, kabhi signature, kabhi dono, kabhi kuch nahi. Har combination ke liye subclass banate to class explosion (`TimestampSignatureNotification`...).

**Kaise:** Har decorator ek `INotification` ko WRAP karta hai aur `getContent()` me apna hissa jodta hai:

```cpp
notification = make_shared<SimpleNotification>("title: body");
notification = make_shared<TimestampDecorator>(notification);      // +timestamp
notification = make_shared<SignatureDecorator>(notification, "..."); // +signature
```

`getContent()` call andar se bahar chalta hai: Simple → Timestamp → Signature. Combination flexible, koi explosion nahi. (L13 Decorator dekho.)

---

## 4. Singleton — NotificationHub 1️⃣

**Kahan:** `NotificationHub` (modular version me — **Meyers style** ✅)

**Kyun:** Poore system me ek hi central observable hub hona chahiye — sab observers usi se attach hon, sab notifications usi se bhejein. Do hubs hote to kuch observers ek pe, kuch dusre pe — inconsistency.

**Modular vs monolith styles:**

| File | Singleton style |
|------|-----------------|
| `notification_lld/` (modular) | **Meyers** (`static local`) — simple, thread-safe by C++11 |
| `NotificationSystem.cpp` (monolith) | Simple lazy (basic) |
| `dclp_...cpp` (monolith #3) | **DCLP** (double-checked locking + mutex) — explicit thread-safe |

Teeno L10 ke Singleton lesson se seedhe aate hain — ek hi system me teen styles dekh sakte ho!

---

## Files Ka Structure

```
L14 Notification_Engine_LLD/
├── design_patterns_used.md        ← Ye file
├── C++ Code/                      ← 3 MONOLITH versions (single-file):
│   ├── NotificationSystem.cpp                        (basic)
│   ├── NotificationSystemUpdated.cpp                 (refined)
│   └── dclp_...safe_notification_system.cpp          (thread-safe DCLP)
└── notification_lld/              ← MODULAR version (folder-based):
    ├── main.cpp                   (demo driver)
    ├── core/NotificationSystem.h  (Facade)
    ├── services/                  (Engine = Observer+Strategy+Singleton+Decorator)
    ├── models/                    (Recipient, NotificationMessage)
    ├── enums/                     (ChannelType, NotificationPriority)
    └── rules/                     (NotificationRules — config)
```

**Monolith vs Modular:** Monolith seekhne/reference ke liye (sab ek jagah dikh jaata hai); modular real-project structure ka example (har concern apni file me).

---

## Design Notes & Issues (interview me bolne layak) ⭐

1. **Observer + Strategy ka combo:** `NotificationEngine` KHUD ek observer hai (event sunta hai) AUR strategies ka container (event pe har channel pe bhejta hai). Ek pattern dusre ko feed karta hai — real systems aise hi bante hain.

2. **Decorator ordering:** Simple → Timestamp → Signature. Order matters — output me timestamp pehle, signature end me aata hai.

3. **🐛 DEAD CODE (do jagah):** `models/NotificationMessage.h` aur `notification_lld.h` dono me ek DUSRA `#ifndef` block hai jo same include-guard ki wajah se **skip** ho jaata hai (dead code). Chalta sahi hai par confusing — ideally hata dena chahiye. (Comments me note kiya hai.)

4. **Singleton styles ka evolution:** modular Meyers, monolith #1 simple, monolith #3 DCLP — teen tareeke ek system me.

5. **Recipient not-found silently ignored:** `NotificationService::send()` me id na mile to chupchap return — production me log/exception chahiye.

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Dead `#ifndef` blocks hatao (NotificationMessage.h, notification_lld.h)
- `NotificationRules` ko actually use karo — priority-based routing, rate-limiting, retry
- Recipient ke `preferredChannel` ko respect karo (abhi sab channels pe jaata hai — sabko sab kuch!)
- Raw pointers (monolith versions) → smart pointers (modular already shared_ptr use karta)
- Recipient not-found pe proper error handling

---

**Related lessons is repo me:** L8 Strategy · [L10 Singleton](../L10%20Singleton_Design_Pattern/) · [L12 Observer](../L12%20Observer_Design_Pattern/) · [L13 Decorator](../L13%20Decorator_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/)
