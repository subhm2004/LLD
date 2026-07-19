# Observer Design Pattern — Deep Dive

> **Ek line me:** "Ek Subject apne kai Observers ko, state badalte hi, khud-b-khud
> khabar kar deta hai."
>
> 📄 Code: [`ObserverDesignPattern.cpp`](ObserverDesignPattern.cpp) — YouTube
> channel + subscribers ka example.

---

## Table of Contents

1. [Pehle problem samjho — POLLING](#1-pehle-problem-samjho--polling)
2. [Observer kya hai](#2-observer-kya-hai)
3. [Chaar roles (GoF)](#3-chaar-roles-gof)
4. [Code ka poora flow](#4-code-ka-poora-flow---step-by-step)
5. [Push vs Pull model](#5-push-vs-pull-model)
6. [Fayde (kyun use karein)](#6-fayde--kyun-use-karein)
7. [⚠️ Khatre (kya dhyaan rakhein)](#7-%EF%B8%8F-khatre--kya-dhyaan-rakhein)
8. [Is code ka ek asli BUG](#8-is-code-ka-ek-asli-bug)
9. [Asli duniya me kahan use hota hai](#9-asli-duniya-me-kahan-use-hota-hai)
10. [Observer vs Mediator](#10-observer-vs-mediator--interview-favourite)
11. [Interview me kya bolna](#11-interview-me-kya-bolna)

---

## 1. Pehle problem samjho — POLLING

Observer ko samajhne ke liye pehle uska **ulta** samajhna zaroori hai.

Maan lo tumhe pata karna hai ki tumhare favourite channel pe naya video aaya ya
nahi. **Bina** Observer ke tum kya karoge? Baar-baar khud check karoge:

```cpp
while (true) {
    if (channel.hasNewVideo()) {   // "naya aaya kya?"
        showNotification();
    }
    sleep(5);                      // "ab? ... ab? ... ab?"
}
```

Ise **POLLING** kehte hain. Iski teen bimariyan hain:

| Problem                | Kya hota hai                                                       |
| ---------------------- | ------------------------------------------------------------------ |
| **Bekaar ka kaam**     | 99% baar jawab "nahi" hota — poori mehnat zaya                      |
| **CPU/network barbaad** | Har check ek request. 10 lakh users × har 5 second = server dead 💀 |
| **Der ho jaati hai**   | Video aaya 1:00 baje, pata chala 1:05 pe                            |

### Observer ka ulta idea

Subscriber **poochta hi nahi**. Channel **khud bata deta hai** — aur theek us
waqt jab asal me kuch hua ho.

```
POLLING   :  Subscriber ──"naya aaya?"──> Channel   (baar-baar, bekaar)
                        <──"nahi"────────

OBSERVER  :  Channel ──"video aaya!"──> Subscriber  (sirf ek baar, jab zaroorat ho)
```

> 📌 Ise **"Hollywood Principle"** kehte hain:
> **_"Don't call us, we'll call you."_**

---

## 2. Observer kya hai

**Observer** ek **Behavioral** design pattern hai jo **one-to-many** rishta
banata hai:

> Jab ek object (**Subject**) ki state badalti hai, to uske saare dependent
> objects (**Observers**) ko apne aap khabar ho jaati hai.

Is code me:

| Pattern ka role | Is code me     | Asli duniya me                    |
| --------------- | -------------- | --------------------------------- |
| Subject         | `Channel`      | YouTube channel                   |
| Observer        | `Subscriber`   | Tum (jo subscribe karte ho)       |
| State           | `latestVideo`  | Naya video                        |
| Notify          | `uploadVideo()` | Bell icon 🔔 wali notification    |

---

## 3. Chaar roles (GoF)

```
      ┌─────────────────┐              ┌──────────────────┐
      │   IChannel      │              │   ISubscriber    │
      │  (Subject       │              │   (Observer      │
      │   interface)    │              │    interface)    │
      ├─────────────────┤   notify     ├──────────────────┤
      │ + subscribe()   │─────────────>│ + update() = 0   │
      │ + unsubscribe() │              └────────▲─────────┘
      │ + notify()      │                       │ implements
      └────────▲────────┘                       │
               │ implements              ┌──────┴───────┐
      ┌────────┴────────┐                │  Subscriber  │
      │    Channel      │<───────────────│  (Concrete   │
      │  (Concrete      │  getVideoData()│   Observer)  │
      │   Subject)      │   (PULL)       ├──────────────┤
      ├─────────────────┤                │ - name       │
      │ - subscribers[] │                │ - channel*   │
      │ - latestVideo   │                │ + update()   │
      │ + uploadVideo() │                └──────────────┘
      └─────────────────┘
```

### A. `ISubscriber` — Observer interface

```cpp
class ISubscriber {
public:
    virtual void update() = 0;
    virtual ~ISubscriber() {}
};
```

Bas ek function. Jo bhi notification lena chahta hai, use `update()` likhna
padega — compiler majboor karta hai.

> 💡 **Interface vs Abstract class:** agar **saare** methods pure virtual
> (`= 0`) hon to use **interface** kehte hain. Ek bhi method ka body ho to
> **abstract class**. C++ me dono ke liye alag keyword nahi hai (Java me
> `interface` hota hai) — bas convention hai. Isi liye naam `I` se shuru
> hota hai: `ISubscriber`, `IChannel`.

### B. `IChannel` — Subject interface

Teen kaam ka contract: `subscribe`, `unsubscribe`, `notifySubscribers`.

### C. `Channel` — Concrete Subject

**Sabse zaroori line poore code me:**

```cpp
vector<ISubscriber *> subscribers;
//     └── INTERFACE ka pointer, `Subscriber*` NAHI!
```

Isi **ek line** se poora loose coupling aata hai. Channel ko `Subscriber` class
ka **naam tak nahi pata** — usko bas itna pata hai ki "in sabme `update()` hai."

### D. `Subscriber` — Concrete Observer

`update()` implement karta hai, aur channel ka pointer rakhta hai taaki data
**kheench** sake (pull model — section 5).

---

## 4. Code ka poora flow — step by step

### Step 1: Setup

```cpp
Channel *channel = new Channel("Bhai_ki_padhai");
Subscriber *subs1 = new Subscriber("Shubham", channel);
Subscriber *subs2 = new Subscriber("Hardik", channel);
```

Channel bana — uski `subscribers` list abhi **khaali** hai.

### Step 2: Subscribe (rishta juda)

```cpp
channel->subscribe(subs1);
channel->subscribe(subs2);
```

Andar kya hua:

- `find()` se check hua ki **pehle se list me to nahi hai**
- Nahi tha → `subscribers` vector me pointer add ho gaya

> ⭐ **Duplicate check kyun?** Bina iske, koi galti se do baar subscribe kar de
> to usko har video ki notification **do baar** milegi. Asli app me user
> "subscribe" button do baar daba sakta hai — ye guard usse bachata hai.

### Step 3: Event — video upload

```cpp
channel->uploadVideo("Observer Pattern Tutorial");
```

Andar **do** kaam, aur **order zaroori hai**:

```cpp
latestVideo = title;      // 1. pehle state badlo
notifySubscribers();      // 2. PHIR sabko batao
```

> ⚠️ **Ulta karte to bug ban jaata:** pehle notify karte, phir state badalte —
> to subscribers `getVideoData()` bulate aur unhe **purana video** milta. 🐛

### Step 4: Broadcast (pattern ka dil)

```cpp
void notifySubscribers() override {
    for (ISubscriber *sub : subscribers) {
        sub->update();
    }
}
```

Ye **teen line hi poora Observer pattern** hain. Dhyaan do:

- Channel ko pata **nahi** ki `sub` asal me kaun hai
- Wo bas `update()` bulata hai aur aage badh jaata hai
- Sahi class ka `update()` **apne aap** chalta hai (virtual dispatch)

> 💡 List khaali ho to? Loop chalega hi nahi. Channel **bina kisi subscriber ke
> bhi theek chalta hai** — dono taraf se aazadi.

### Step 5: Reaction

Har subscriber ka `update()` chalta hai → wo channel se data **kheenchta** hai →
message print karta hai.

### Step 6: Unsubscribe (rishta toota)

```cpp
channel->unsubscribe(subs1);
```

⭐ Ye **runtime** pe hua — koi recompile nahi, Channel me koi `if-else` nahi.
Bas list se pointer hat gaya. Yahi **dynamic relationship** hai.

### Asli output

```
[Bhai_ki_padhai uploaded "Observer Pattern Tutorial"]
Hey Shubham,
Checkout our new Video : Observer Pattern Tutorial
Hey Hardik,                                          ← dono ko mila
Checkout our new Video : Observer Pattern Tutorial

[Bhai_ki_padhai uploaded "Decorator Pattern Tutorial"]
Hey Hardik,                                          ← ab sirf Hardik!
Checkout our new Video : Decorator Pattern Tutorial     (Shubham unsubscribe kar chuka)
```

---

## 5. Push vs Pull model

Ye interview ka favourite sawaal hai.

| | **PUSH** | **PULL** |
| --- | --- | --- |
| Signature | `update(string videoTitle)` | `update()` |
| Kaun data deta | Subject **bhej** deta hai | Observer khud **kheenchta** hai |
| Subject ko sochna padta | "kya-kya bhejun?" | kuch nahi — bas "kuch badla" |
| Coupling | thoda zyada | kam ✅ |
| Kab achha | sab observers ko **same** data chahiye | har observer ko **alag** data chahiye |

**Ye code PULL use karta hai:**

```cpp
void update() override {                              // ← koi param nahi
    cout << "Hey " << name << "," << channel->getVideoData();
    //                                 └── data KHUD kheencha
}
```

**Pull ka fayda:** kal ek `ViewCountLogger` add karo — usko video ka naam nahi,
sirf count chahiye. Push model me Channel ko **sabka** data bhejna padta
(bekaar), pull me har observer apni cheez khud le leta hai.

> 💡 Asal me ye **HYBRID** hai: "kuch hua" ki khabar **push** hui (`update()`
> call), aur data **pull** hua (`getVideoData()`). Zyadatar real systems aisa
> hi karte hain.

---

## 6. Fayde — kyun use karein

| Fayda | Matlab |
| --- | --- |
| **Loose Coupling** | Channel ko `Subscriber` ka naam tak nahi pata — bas `ISubscriber` interface se baat karta hai |
| **Broadcast** | Ek trigger, aur **kitne bhi** observers ko khabar |
| **Dynamic** | Subscribe/unsubscribe **runtime** pe — code badle bina |
| **Open/Closed** | Naya observer type (`EmailNotifier`, `MobileApp`) add karo — Channel ka code **ek line bhi nahi** badlega ✅ |

---

## 7. ⚠️ Khatre — kya dhyaan rakhein

### (a) Lapsed Listener — sabse aam galti 💀

Subscriber object **delete** ho gaya, par usne **unsubscribe nahi kiya**?
Channel ki list me uska **dangling pointer** reh gaya. Agli `notifySubscribers()`
pe seedha **crash**.

```cpp
Subscriber *s = new Subscriber("Amit", channel);
channel->subscribe(s);
delete s;                    // ❌ unsubscribe nahi kiya!
channel->uploadVideo("...");  // 💥 CRASH — mare hue object ka update() bula raha
```

**Hal:** subscriber ke destructor me khud unsubscribe karo, ya `weak_ptr` use
karo.

### (b) Notification ka order

Kaunsa observer pehle notify hoga — iski **koi guarantee nahi** honi chahiye.
Agar tumhara code "A pehle, phir B" pe depend karta hai, to design galat hai.

### (c) Performance

Hazaron observers aur state baar-baar badle? Har notify ek poora loop hai. Tab
batching ya async notification sochna padta hai.

### (d) Cascade / infinite loop

Observer ke `update()` me subject ki state badal di? Wo phir notify karega → wo
phir update → **infinite loop**. 🔁 Observer ko sirf **padhna** chahiye, badalna
nahi.

---

## 8. Is code ka ek asli BUG

`main()` me **memory leak** hai:

```cpp
Channel *channel = new Channel("Bhai_ki_padhai");   // new
Subscriber *subs1 = new Subscriber("Shubham", channel);  // new
Subscriber *subs2 = new Subscriber("Hardik", channel);   // new
...
return 0;    // ❌ ek bhi `delete` nahi!
```

Teeno objects leak ho rahe hain. Hona chahiye tha:

```cpp
delete subs1;
delete subs2;
delete channel;
```

> ⭐ **Ek dilchasp baat:** macOS ka `leaks` tool is leak ko **pakad nahi paata**
> — wo "0 leaks" bolta hai! Kyunki wo stack scan karta hai, aur ye pointers exit
> ke waqt bhi stack me pade dikhte hain → tool unhe "reachable" maan leta hai.
>
> Maine `new`/`delete` **gin ke** confirm kiya: **8 allocations, sirf 3 frees.**
>
> 📌 **Sabak:** tool ka "clean" hamesha sach nahi hota. Code padhna zaroori hai.

**Sabse saaf hal:** raw pointer ki jagah `unique_ptr` — phir delete bhoolne ka
sawaal hi nahi. (Ya objects stack pe hi bana lo — is demo me `new` ki zaroorat
hi nahi thi!)

---

## 9. Asli duniya me kahan use hota hai

| Jagah | Subject | Observers |
| --- | --- | --- |
| **GUI / Web** | Button | Click listeners (`addEventListener`) |
| **Social media** | Account | Followers (Twitter/Instagram) |
| **Stock market** | Stock price | Alert lagane wale users |
| **MVC** | Model | Views (data badle → UI update) |
| **Excel** | Cell | Formulas jo us cell pe depend karti hain |
| **Is repo me** | `Group` | `User` (L31 Splitwise), `SnakeAndLadderGame` → notifier (L34) |

---

## 10. Observer vs Mediator — interview favourite

Dono objects ko decouple karte hain, par **ulti soch se**:

| | **OBSERVER** | **MEDIATOR** |
| --- | --- | --- |
| Rishta | **one-to-many** | **many-to-many** |
| Direction | ek-tarfa (subject → observers) | do-tarfa (sab ↔ sab) |
| Kaun jaanta hai | Subject ke paas observers ki list | Mediator ke paas sabki list |
| Yaad rakhne ka tareeka | **Newsletter** — ek bhejta, sab padhte | **Group chat admin** — sab bolte, admin route karta |

> 📄 Mediator ki poori baat: [`L35 Mediator_design_pattern/`](../../L35%20Mediator_design_pattern/)

---

## 11. Interview me kya bolna

**Definition (ek line):**

> "Observer ek behavioral pattern hai jo one-to-many dependency banata hai —
> subject ki state badle to saare observers ko apne aap khabar ho jaati hai."

**Kyun use karein (teen point):**

1. **Polling se bachte hain** — subject khud batata hai, wo bhi tabhi jab
   ASAL me kuch badla ho
2. **Loose coupling** — subject sirf interface jaanta hai, concrete class nahi
3. **Runtime pe subscribe/unsubscribe** — code badle bina

**Khatra jo tum khud se bataoge (ye impress karta hai 🎯):**

> "Sabse badi galti **Lapsed Listener** hai — observer delete ho jaye par
> unsubscribe na kare, to subject ki list me dangling pointer reh jaata hai aur
> agli notify pe crash. Isi liye main ya to destructor me unsubscribe karta hu,
> ya `weak_ptr` use karta hu."

**Follow-up jo aa sakta hai:**

- _"Push ya pull?"_ → section 5 padho
- _"Observer vs Mediator?"_ → section 10
- _"Async notification kaise karoge?"_ → queue me daal do, observers ko alag
  thread pe notify karo (par tab thread-safety sambhalni padegi)

---

## Summary — methods ek nazar me

| Method | Kaam |
| --- | --- |
| `subscribe()` | Listener jodo (duplicate check ke saath) |
| `unsubscribe()` | Listener hatao — **lapsed listener** se bachne ke liye zaroori |
| `notifySubscribers()` | **Engine** — sabko `update()` bulao |
| `uploadVideo()` | **Trigger** — state badlo, phir notify karo (order zaroori!) |
| `update()` | **Reaction** — observer apna kaam kare |
| `getVideoData()` | **Pull** — observer data kheenchta hai |
