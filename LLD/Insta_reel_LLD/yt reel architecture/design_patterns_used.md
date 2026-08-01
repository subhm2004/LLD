# Design Patterns Used — Insta/YouTube Reel LLD

Ye ek short-video platform (Reels/Shorts) ka LLD hai. Ye jaan-boojh ke **chhota aur saaf**
rakha gaya hai — iska core ek **Facade/Service Layer** aur **rich domain models** (jo apni
engagement-logic khud sambhalte hain) pe khada hai, plus ek engagement-ranked feed.

> 📌 **Honest note:** ye LLD me abhi bahut saare GoF patterns nahi hain (na Strategy, na
> Observer) — aur ye theek hai. Problem statement ka objective hai: *"modular design jahan
> recommendation strategy, moderation, ads baad me add ho sakein"* — yaani abhi clean base,
> patterns tab jab zaroorat pade. Neeche jo hai wo hai, aur jo **natural extension points**
> hain wo bhi honestly likhe hain (over-claim nahi).

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            OVERVIEW                                            │
├──────────────────────┬─────────────────────────────────┬─────────────────────┤
│  Pattern/Technique   │  Kahan (class)                  │  Kis liye            │
├──────────────────────┼─────────────────────────────────┼─────────────────────┤
│  Facade / Service ⭐ │  ReelPlatformService            │  single entry point  │
│  Rich domain models  │  Reel, User                     │  logic model ke andar│
│  Composition         │  Reel ◆ Comment                 │  ownership           │
│  Feed: filter→rank→  │  ReelPlatformService::getFeed   │  personalized feed   │
│  limit (technique)   │                                 │                      │
└──────────────────────┴─────────────────────────────────┴─────────────────────┘
```

---

## 1. Facade / Service Layer ⭐ — `ReelPlatformService`

### 📌 Concept
Poore subsystem (users, reels, follow-graph, feed) ke aage ek patla, saaf interface.

### Implementation
Client ([`main.cpp`](./main.cpp)) sirf `ReelPlatformService` jaanta hai: `registerUser`,
`follow`, `uploadReel`, `viewReel`/`likeReel`/`commentOnReel`, `getFeed`. Andar storage,
follow-graph lookups, ranking — sab hidden.

### ⭐ KYUN?
> Client aur internals decoupled. Kal ko storage badle (map → DB) ya ranking badle, `main.cpp`
> untouched. Aur validation ek jagah (`getUserOrThrow`/`getReelOrThrow` — fail-fast).

---

## 2. Rich Domain Models — logic model ke andar (SRP) ⭐

Ye is design ki sabse achhi baat hai: **service patla, models rich**.

| Model | Apna logic khud sambhalta hai |
|-------|-------------------------------|
| [`Reel`](./models/Reel.h) | `addLike` (duplicate-like guard), `removeLike`, `addView` (total vs unique) |
| [`User`](./models/User.h) | `follow` (self-follow guard), `isFollowing` O(1) |

Service sirf **validate + delegate** karti hai:
```cpp
void likeReel(userId, reelId) {
    getUserOrThrow(userId);                  // validate
    getReelOrThrow(reelId)->addLike(userId); // delegate — duplicate logic Reel me
}
```
> **⭐ KYUN?** Agar duplicate-like check service me hota, to har jagah repeat karna padta aur
> koi bhi bahar se `reel.likes++` karke invariant tod sakta. Model ke andar rakhne se
> **`totalLikes_` aur `likedBy_.size()` hamesha in-sync** rehte — encapsulation ka fayda.
> (Anemic model — jisme sirf getters/setters ho aur logic service me — ka ulta.)

---

## 3. Technique: Total vs Unique metrics (Reel)

`Reel` do alag metrics rakhta hai:
- `totalViews_` (counter) → har view, repeat bhi (watch-time/ads ke liye)
- `uniqueViewers_` (set) → kitne **alag** log (reach ke liye)

> Ek se doosra derive nahi hota, isliye dono chahiye. Aur `likedBy_` set duplicate-like ko
> structurally rok deta hai (ek banda 100 baar like dabaye to bhi count 1).

---

## 4. Technique: Feed = filter → rank → limit

[`getFeed`](./services/ReelPlatformService.h) 3 steps me:
```
1. FILTER : reels jinke owner ko main follow karta hoon, YA mere apne  (follow-graph se)
2. RANK   : views DESC, tie ho to likes DESC                            (popularity)
3. LIMIT  : top N                                                        (pagination)
```
> **⚠ Trade-off:** ye har call pe saare reels scan karta (O(N)). Real system me per-user
> precomputed timeline (fan-out on write) hota hai. Demo/interview-scale pe theek — ye
> trade-off bolna accha point hai.

---

## Object Relationships (UML samajhne ke liye)

```
ReelPlatformService  ◆──  User / Reel (maps)     (Composition: OWNS, dtor deletes)
Reel                 ◆──  Comment (vector)       (Composition: reel owns comments)
User                 ──▶  User (following_ ids)  (Association: one-way follow graph)
Reel                 ──▶  User (ownerId)         (Association: id se)
```
- `◆──` Composition (owns) · `──▶` Association (refers by id)

---

## Requirement → Implementation quick map

| Requirement | Kahan |
|-------------|-------|
| FR-1 register user | `registerUser` (duplicate reject) |
| FR-2 follow | `User::follow` (one-way, self-follow guard) |
| FR-3 upload reel | `uploadReel` (unique id + reverse index) |
| FR-4 view | `Reel::addView` (total + unique) |
| FR-5 like/unlike | `Reel::addLike/removeLike` (duplicate-safe) |
| FR-6 comment | `Reel::addComment` |
| FR-7 feed (own + following) | `getFeed` filter step |
| FR-8 top N | `getFeed` limit step |

> **Cleanup + fix:**
> 1. 🧹 Har file me `#include <bits/stdc++.h>` tha (non-portable, GCC-only) — hata diya, targeted includes.
> 2. 🐛 **Double-free fix:** `ReelPlatformService` raw `User*`/`Reel*` own karke dtor me delete
>    karta hai, par isme koi `unique_ptr` member nahi tha jo implicit copy rok le — saare members
>    copyable the. To default copy constructor pointers ko **shallow copy** karta aur do copies
>    same pointer delete karke crash karte. **Copy/move `= delete`** kar diya (Rule of Three/Five).

---

## Natural extension points (jo abhi NAHI hain — honest list)

Problem statement kehta hai "recommendation strategy, moderation, ads baad me add ho sakein".
Wo abhi nahi hain, par yahan add honge:

- **Strategy for feed ranking** — abhi ranking `getFeed` me **hardcoded** hai (views→likes sort).
  Ek `IFeedRankingStrategy` (Chronological / Engagement / ML-based) nikaal ke inject karna sabse
  natural next step hai. *(Abhi ek hi ranking hai isliye interface banana YAGNI hota — par ye
  design ka sabse obvious extension point hai.)*
- **Observer for notifications** — "tumhare reel pe like/comment aaya" abhi kuch nahi bhejta.
  Observer (subscribers: push/email) add ho sakta.
- **Factory / Builder for Reel** — abhi construction simple hai (4 fields). Fields badhein
  (music, filters, hashtags, thumbnail) to Builder worth it.
- **`unique_ptr` storage** — raw pointer + manual delete ki jagah `unordered_map<string,
  unique_ptr<Reel>>` structurally safe hota (double-free possible hi na hota). Abhi `= delete`
  se guard kiya hai.
- **Pagination cursor** — abhi `limit` se top-N; infinite scroll ke liye cursor/offset chahiye.
```
