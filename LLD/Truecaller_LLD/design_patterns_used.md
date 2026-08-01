# Truecaller LLD — Design Patterns & Design Decisions

Caller identification + spam protection: **register → directory → caller ID → contact sync →
spam report → block → search → call log**.

Is LLD ka asli sabak patterns se zyada **data modelling** hai — kaunsa data GLOBAL hai,
kaunsa PERSONAL, aur kaunsa user ka apna. Interview me marks yahi dilate hain.

---

## 📁 Folder structure

```
Truecaller_LLD/
├── enums/          CallerTag, ReportReason, CallType, AccountStatus
├── utils/          PhoneUtils (normalization — poore system ki neev)
├── models/         User, PhoneProfile, ContactEntry, CallLogEntry, CallerLookupResult
├── strategies/     ISpamScoringStrategy + DefaultSpamScoringStrategy
├── services/       Lookup, Block, SpamReport, ContactSync, Search, CallLog
├── core/           TruecallerSystem  (Facade)
└── main.cpp        demo + fixed-bug proofs
```

---

## ⭐⭐ 0. Sabse important concept — `User` vs `PhoneProfile`

Ye samajh liya to poora LLD samajh me aa gaya.

| | `User` | `PhoneProfile` |
|---|---|---|
| Kya hai | jisne **app install karke account banaya** | **kisi bhi number** ki public jaankari |
| Data | userId, email, status, isBusiness | naam, tag, spamScore, reportCount |
| Key | `userId` | `phone` |
| Zaroori? | — | **`User` hona ZAROORI NAHI** |

**Har `PhoneProfile` ka `User` nahi hota.** Jo number kabhi register hi nahi hua par logon ne
spam report kiya — uska bhi profile banta hai (`ownerUserId` khaali).

```cpp
// SpamReportService — profile REGISTER ke bina ban jaata hai
if (it == directory.end()) {
    directory.emplace(normalized, PhoneProfile(normalized, "Reported Number", "", UNKNOWN));
}
```

⭐ **Yahi Truecaller ka poora business model hai.** Sirf registered users wali directory bekaar
hoti — **spammer to kabhi register hi nahi karega!** Directory **reports** se banti hai,
registrations se nahi. Crowd-sourced data = network effect.

---

## ⭐⭐ 1. Phone Normalization — poore system ki neev

`utils/PhoneUtils.h` chhoti file hai par sabse critical.

Phone yahan **har jagah ka KEY** hai: `directory_[phone]`, `phoneToUserId_[phone]`,
`blocked_[user].count(phone)`, `ContactEntry`, `CallLogEntry`.

**Bina normalization ke ek hi number ke chaar alag keys:**
```
"9876543210"   "+91 9876543210"   "098765-43210"   "(+91) 98765 43210"
```

| Kahan | Natija |
|---|---|
| Block | "+91 98765 43210" block kiya, call "9876543210" se aayi → **block kaam nahi karta** 😱 |
| Spam | reports 4 profiles me bat jaate → koi 70 tak nahi pahunchta → **spammer flag hi nahi hota** 😱 |
| Register | ek hi banda 4 baar register kar leta → **unique check bypass** 😱 |

**Fix ki jagah:** normalization **constructor ke andar** hai (`User`, `PhoneProfile`,
`ContactEntry`, `CallLogEntry` — sab me). Galat format wala object **ban hi nahi sakta**.
Services me kabhi `if (isValid...)` likhne ki zaroorat nahi.

📌 **Rule:** jo cheez KEY banegi, use ek CANONICAL FORM me lao — aur ye kaam **boundary pe**
(object banate waqt) karo, andar nahi. Isko **"always-valid object"** kehte hain.

⭐ `static_cast<unsigned char>` bhi zaroori hai — `std::isdigit`/`tolower` ko negative value
dena **undefined behaviour** hai (`char` kai platforms pe signed hota hai). Classic C++ gotcha.

---

## 2. Strategy Pattern — `ISpamScoringStrategy`

```cpp
virtual int scoreDeltaFor(ReportReason reason) const = 0;   // bas ek method
```

**Ye interface itna patla kyun?** Kyunki system ka **sabse zyada badalne wala hissa** yahi hai:

```
aaj   -> fixed weights (SCAM=35, TELEMARKETER=25, OTHER=15)
kal   -> reporter ki reputation (naya user vs 5-saal purana)
parso -> call pattern (100 calls/min = robocall), time-decay
aage  -> poora ML model
```

In sab me `SpamReportService` ka **ek line bhi nahi badlega**. ⭐ Interface jitna patla,
replace karna utna aasan.

### Weights random nahi hain

| Reason | Delta | Kitni reports se SPAM (threshold 70) |
|---|---|---|
| SCAM | +35 | **2** — fraud sabse khatarnak, ek call me paisa jaa sakta hai |
| HARASSMENT | +30 | 3 |
| TELEMARKETER | +25 | 3 |
| ROBOT_CALL | +20 | 4 |
| OTHER | +15 | **5** — reason specific nahi, bharosa kam |

📌 **Weight = "is signal pe kitna bharosa hai".** `OTHER` sabse halka isliye ki koi apne
dushman ka number report kar sakta hai 😈 — *kam information = kam impact.*

⭐ `DefaultSpamScoringStrategy` ke `switch` me **`default:` nahi hai** — jaan-boojh ke. Naya
`ReportReason` add karke case bhoolo to compiler `-Wswitch` warning dega. `default:` likh dete
to compiler chup rehta aur naya reason chup-chaap fallback score paata — bug **runtime** pe milta.

---

## 3. Facade Pattern — `TruecallerSystem`

Andar 6 services + 1 strategy. `main.cpp` ko ek ka bhi naam nahi pata.

**Karta hai:** data ownership, service wiring, cross-cutting validation, `User` ↔ `PhoneProfile`
sync, id counters.
**Nahi karta:** spam scoring, lookup logic, search, blocking.

📌 Facade **coordinator** hai, **God object** nahi.

---

## 4. Service Layer + SRP — 6 services

| Service | Zimmedari | State? |
|---|---|---|
| `LookupService` | caller ID (global + personal jodna) | **stateless** |
| `BlockService` | per-user blocklist | stateful |
| `SpamReportService` | report lena, score badhana | strategy hold karta hai |
| `ContactSyncService` | contacts enrich karna | **stateless** |
| `SearchService` | naam prefix / exact phone | **stateless** |
| `CallLogService` | call history | stateful |

⭐ **Chhah me se teen services STATELESS hain** — data unhe **parameters se** milta hai.
Fayde: test karna trivial (fake directory pass karo), thread-safety ki tension nahi,
ek object sab share kar sakte. 📌 Stateless services default choice honi chahiye.

---

## ⭐ 5. GLOBAL truth vs PERSONAL preference

Ye poore design ki reedh ki haddi hai:

```
directory_   (GLOBAL)   -> naam, tag, spamScore   — sabke liye same
blocked_     (PERSONAL) -> "ISNE block kiya hai?" — sirf is user ka
```

Isi liye `LookupService::lookup(...)` me `forUserId` chahiye. Ek hi number:
- Alice ke liye → `blocked=yes`
- Bob ke liye → `blocked=no`
- **spamScore dono ke liye SAME** (global faisla)

⭐ Aur `isBlocked` **directory lookup se PEHLE** set hota hai — soch-samajh ke.
Number directory me ho ya na ho, block to phir bhi lagna chahiye. Warna anjaan number block
karne ke baad bhi call aati rehti! 😱

📌 Ye "global truth + personal preference" pattern har communication app me hai
(mute, block, hide — sab personal).

---

## ⭐ 6. Enrich, Replace nahi — `ContactEntry` ke DO naam

```cpp
localName_     -> "Mom"            // jo TUMNE save kiya
directoryName_ -> "Sunita Sharma"  // jo DUNIYA jaanti hai
```

Dono **saath rehte hain**. `ContactSyncService` sirf `setDirectoryName()` bharti hai —
`localName_` ko **haath nahi lagati**.

Agar directory naam se local ko overwrite kar dete to user ka personal context chala jaata —
bahut kharab UX. 📌 **User ka diya hua data kabhi silently mat badlo; system ka enrichment
ALAG field me rakho.**

---

## 7. Denormalization — naam DO jagah hai

```
users_[userId].displayName      // account data
directory_[phone].displayName   // public directory
```

✅ **Fayda:** caller ID lookup **ek map se O(1)** — har incoming call pe do-hop join nahi.
Aur unregistered numbers to `users_` me hote hi nahi.
⚠ **Keemat:** `updateProfile` me **dono** saath update karna padta hai.

📌 Classic **"denormalization for read performance"** — read-heavy system (har call pe lookup!)
me bilkul sahi trade-off.

---

## 8. DTO vs Domain object — `struct` vs `class`

```cpp
struct CallerLookupResult { ... };   // sirf data dhone ka bartan
class  PhoneProfile { ... };         // invariant hai (score <-> tag ka rishta)
```

📌 **Rule:** INVARIANT ho → `class` + private + validation. Sirf DATA CARRY karna ho → `struct`.

⭐ `CallerLookupResult` ke **teen** boolean flags, teeno ka alag matlab:

| Flag | Matlab |
|---|---|
| `foundInDirectory` | "hum is number ke baare me kuch jaante hain?" |
| `isSpam` | "khatarnak hai?" (**global**) |
| `isBlocked` | "ISNE block kiya hai?" (**personal**) |

`foundInDirectory=true` + `tag=UNKNOWN` = *"naam nahi pata, magar ye number spam hai"* —
Truecaller ka sabse kaam ka jawab! Sirf tag dekh ke ye farak nahi pata chalta.

---

## 🐛 Teen bugs — mile, confirm hue, fix hue

Har ek pehle **test likh ke reproduce** kiya, phir fix, phir dobara verify.
Teeno ka **live proof `main.cpp`** ke aakhir me chalta hai.

### 🐛 1. "Recent calls" me sabse PURANI call upar aati thi

**Pehle:**
```cpp
std::sort(copy.begin(), copy.end(), [](a, b) {
    return a.getTimestampEpochSec() > b.getTimestampEpochSec();   // dekhne me sahi
});
```

Dekhne me bilkul theek — descending by timestamp = newest first. Par:

⭐ **Timestamp SECONDS me hai.** Ek hi second me hui saari calls ka timestamp **bilkul same**
hota hai. Jab saari keys barabar ho, comparator har jodi pe `false` deta hai — sort ke paas
order decide karne ka **koi aadhaar hi nahi**. Upar se `std::sort` **stable bhi nahi**.
Natija: effectively insertion order = **purani pehle**.

**Confirm** (baseline demo output me hi dikh raha tha):
```
CALL_1 INCOMING ... ts=1784667026
CALL_2 MISSED   ... ts=1784667026     <- teeno ka SAME timestamp
CALL_3 OUTGOING ... ts=1784667026
```

**Fix (do hisse):**
1. `seq` — har entry ke saath monotonic counter (`callCounter_` se). **Hamesha unique.**
2. Comparator ab tie-break karta hai: timestamp desc, barabar ho to **seq desc**.

**Verify:** `[1] history ka pehla = CALL_3 → ✅ newest pehle`

📌 **SABAK:** jab bhi TIME se sort karo, ek **unique tie-breaker** rakho. Milliseconds pe jaane
se bhi ye poori tarah nahi jaata — tez system me same ms bhi aa sakta hai. Sequence pakka hai.

---

### 🐛 2. Register karne se SPAM HISTORY mit jaati thi (spam evasion)

**Pehle** `registerUser` me:
```cpp
directory_[normalized] = PhoneProfile(normalized, name, userId, tag, 0);  // ⚠ pura overwrite
```

**Attack:** number pe reports jama ho (score 70, tag SPAM) → **spammer khud us number se
register kar le** → profile overwrite → score 0, badge saaf. Dobara wahi khel. 😱
`updateProfile` me bhi `setPrimaryTag(...)` seedha SPAM badge hata deta tha.

**Confirm:** `spamScore before-register=70 → after-register=0`

**Fix:** naya method `PhoneProfile::claimOwnership()` —
```cpp
void claimOwnership(const std::string& ownerUserId, const std::string& displayName, CallerTag tag) {
    ownerUserId_ = ownerUserId;
    displayName_ = displayName;
    if (!isSpam()) primaryTag_ = tag;   // ⭐ spam badge hataya nahi ja sakta
    // spamScore_ / reportCount_ ko haath BHI nahi lagate 🔒
}
```
Aur `TruecallerSystem` me `upsertDirectoryProfile()` — profile pehle se ho to overwrite ki
jagah `claimOwnership`. Dono jagah (`registerUser` + `updateProfile`) wired.

**Verify:** `[2] spamScore pehle=70 baad me=70 tag=SPAM → ✅ reports bache`

📌 **RULE:** user apna **DATA** badal sakta hai, **DOOSRON KA FAISLA** nahi.
Reports doosre logon ne diye hain — wo unki property hain.

---

### 🐛 3. Search results arbitrary aur non-deterministic the

**Pehle:**
```cpp
for (const auto& entry : directory) {          // unordered_map!
    if (match) { matches.push_back(...);
                 if (matches.size() >= limit) break; }   // ⚠
}
```

1. `unordered_map` ka traversal order koi guarantee nahi deta → results **random order** me
2. `break` ka matlab: limit=2 pe **jo do sabse pehle scan me mile** — koi ranking nahi.
   Yaani "top 2" **arbitrary 2** the.

**Confirm:** Alpha/Bravo/Charlie/Delta/Echo/Foxtrot register kiye, "Aman" search kiya →
```
Foxtrot Echo Delta Charlie Bravo Alpha       <- ULTA
limit=2 -> [Foxtrot] [Echo]                  <- arbitrary
```

**Fix:** pehle **saare** matches collect → **sort** (naam, phir phone tie-break) → **tab** limit.

⭐ **ORDER MATTERS:** sort **pehle**, truncate **baad me**. Ulta karte (jo pehle mile unhe sort
karo) to phir bhi arbitrary 2 hi milte — bas *sorted dikhte*. Classic
**"sort-then-limit vs limit-then-sort"** gotcha. (Wahi gotcha `CallLogService` me bhi hai.)

**Verify:** `[3] top-3: [Aman Alpha] [Aman Bravo] [Aman Charlie] → ✅ sorted + deterministic`

⚠ **Keemat:** ab hamesha poora scan hota hai. Asli fix: naam ka **trie / prefix index** →
O(prefix + k), aur popularity se ranking. Ye HLD ka scope hai.

---

### Saath me chhote fixes
- `BlockService::listBlocked` ab **sort** karke deta hai (`unordered_set` ka order random tha)
- `PhoneProfile.h` me `<algorithm>` aur `User.h` me `<stdexcept>` **explicitly** add kiye —
  pehle ye `PhoneUtils.h` se **transitively** aate the (fragile: wo include hata do to build toot jaata)

---

## ⚠ Jo jaan-boojh ke NAHI badla (gaps — interview me khud bolna)

| Gap | Detail |
|---|---|
| **Repeat spam reports** | `reporterUserId` sirf validate hota hai, **store nahi** → ek banda 2 baar SCAM report kar ke kisi ka bhi number SPAM mark kar sakta hai 😈. Fix = `set<(reporter, phone)>` dedupe. Ye **feature** hai, bug nahi — isliye behavior nahi badla |
| **No time decay** | 2 saal purani report aaj bhi utni bhaari |
| **No reputation** | naye account aur trusted user ki report ka weight same |
| **`AccountStatus` adhoori wired** | `getUserOrThrow` (write path) SUSPENDED pe throw karta hai, par `validateUser` (read path) nahi. Aur `setStatus` ko koi bulata hi nahi. Suspended user abhi bhi lookup/report kar sakta hai — ise "fix" karna **policy decision** hai |
| **Privacy** | poori phonebook upload karna asli duniya me GDPR/DPDP issue hai (contacts ne consent diya hi nahi). Isi wajah se Truecaller pe kai desh me case chale hain |
| **Block key = phone** | spammer number badal ke phir call kar sakta hai. Asli fix operator-level (HLD) |

---

## 📊 Pattern summary

| # | Pattern / Technique | Kahan | Fayda |
|---|---|---|---|
| 1 | **Strategy** | `ISpamScoringStrategy` | scoring badle, service untouched |
| 2 | **Facade** | `TruecallerSystem` | 7 components ka ek darwaza |
| 3 | **Service Layer / SRP** | 6 services | ek class = ek kaam |
| 4 | **Stateless service** | Lookup, ContactSync, Search | testable, shareable, thread-safe |
| 5 | **Always-valid object** | constructors me `normalizePhone` | galat data object ban hi nahi sakta |
| 6 | **Canonical key** | `PhoneUtils::normalizePhone` | block/spam/register — sab reliable |
| 7 | **DTO vs Domain** | `struct` result vs `class` profile | boilerplate wahin jahan invariant ho |
| 8 | **Denormalization** | naam `users_` + `directory_` | O(1) caller ID lookup |
| 9 | **Global vs Personal** | directory vs blocklist | ek number, alag-alag user ke liye alag jawab |
| 10 | **Enrich not replace** | `ContactEntry` ke 2 naam | user ka context bacha rehta hai |
| 11 | **Dependency Injection** | strategy → `SpamReportService` | swap + test |

---

## 🎯 Extend karna ho to

| Feature | Kya badlega |
|---|---|
| ML spam scoring | nayi `ISpamScoringStrategy` class — bas |
| Reputation-based scoring | interface ka signature badhana padega (`reporterId`, `currentProfile`) |
| Repeat-report dedupe | `SpamReportService` me `set<(reporter, phone)>` |
| Fast prefix search | naam ka **trie** ya inverted index (`SearchService` ke andar) |
| Real DB | services already stateless hain — repository interface add karo |
| Suspended user enforcement | `validateUser` me status check (policy pehle decide karo) |
| Concurrency | directory pe read-write lock (read-heavy → `shared_mutex` sahi rahega) |

---

## ✅ Verification

- `compile.sh` → **zero warnings** (`-std=c++17 -Wall -Wextra -pthread`)
- Demo output baseline se identical — **sirf call-log ka order badla** (wahi to fix tha)
- Teen safety checks `main.cpp` me **live pass**
- **ASan + UBSan** → clean
