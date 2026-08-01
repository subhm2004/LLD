# Decorator Design Pattern — Deep Dive

> **Ek line me:** "Object ke upar ek parat chadha do jo usme naya feature jod de
> — bina uski class chhue, aur runtime pe." 🧅
>
> 📄 Code: [`DecoratorPattern.cpp`](DecoratorPattern.cpp) — Mario + power-ups

---

## Table of Contents

1. [Pehle problem samjho — Class Explosion](#1-pehle-problem-samjho--class-explosion)
2. [Decorator ka hal](#2-decorator-ka-hal)
3. [⭐ IS-A + HAS-A — pattern ka dil](#3--is-a--has-a--pattern-ka-dil)
4. [Chaar roles (GoF)](#4-chaar-roles-gof)
5. [Code ka poora flow](#5-code-ka-poora-flow--pyaaz-ki-parat)
6. [Recursion kaise chalti hai](#6-recursion-kaise-chalti-hai)
7. [🐛 Is code ka asli BUG](#7--is-code-ka-asli-bug--6-objects-leak)
8. [Kab use karein, kab nahi](#8-kab-use-karein-kab-nahi)
9. [Asli duniya me kahan](#9-asli-duniya-me-kahan-use-hota-hai)
10. [Decorator vs doosre patterns](#10-decorator-vs-doosre-patterns)
11. [Interview me kya bolna](#11-interview-me-kya-bolna)

---

## 1. Pehle problem samjho — Class Explosion

Maan lo Mario game bana rahe ho. Mario ko power-ups mil sakte hain: **Height**,
**Gun**, **Star**.

**Bina** Decorator ke, sirf inheritance se karoge to kitni classes chahiye?

```
MarioWithHeight
MarioWithGun
MarioWithStar
MarioWithHeightAndGun
MarioWithHeightAndStar
MarioWithGunAndStar
MarioWithHeightAndGunAndStar
```

**7 classes** — sirf 3 power-ups ke liye! Kyunki har power-up "hai ya nahi" →
2 × 2 × 2 − 1 = 7.

### Ye kitni tezi se bigadta hai

| Power-ups | Classes chahiye (2ⁿ − 1) | Decorator se (n + 1) |
| --------- | ------------------------ | -------------------- |
| 3         | **7**                    | 4                    |
| 5         | **31**                   | 6                    |
| 10        | **1023** 😱              | 11                   |
| 20        | **10,48,575** 💀         | 21                   |

Ise **CLASS EXPLOSION** kehte hain. Aur asli dard sirf ginti nahi hai:

- Naya power-up (`Fire`) add karo → **poori list dobara** banani padegi
- `MarioWithGun` aur `MarioWithGunAndStar` — dono me gun ka code **copy-paste**
- Runtime pe power-up **add nahi kar sakte** — class compile time pe fix ho jaati hai

> 📌 **Yaad rakhne ka formula:** Decorator **2ⁿ** ko **n+1** me badal deta hai —
> yaani **GUNA ko JOD** me. (L34 ka Bridge bhi aisa hi karta hai — M×N → M+N.
> Idea alag, fayda ek jaisa.)

---

## 2. Decorator ka hal

Har power-up ke liye ek **alag class** banao jo Mario ko **wrap** kar le:

```
    ┌─────────────────────────────────────┐
    │ StarPowerUp                         │  ← sabse bahar wali parat
    │   ┌─────────────────────────────┐   │
    │   │ GunPowerUp                  │   │
    │   │   ┌─────────────────────┐   │   │
    │   │   │ HeightUp            │   │   │
    │   │   │   ┌─────────────┐   │   │   │
    │   │   │   │   Mario     │   │   │   │  ← core (pyaaz ka andar)
    │   │   │   └─────────────┘   │   │   │
    │   │   └─────────────────────┘   │   │
    │   └─────────────────────────────┘   │
    └─────────────────────────────────────┘
```

Bilkul **pyaaz** ki tarah 🧅 — har parat andar wali ko lapet leti hai, aur apna
kuch jod deti hai.

**Sirf 4 classes:** `Mario`, `HeightUp`, `GunPowerUp`, `StarPowerUp`.
Aur inse **saare 7 combinations** ban jaate hain!

---

## 3. ⭐ IS-A + HAS-A — pattern ka dil

Ye is poore pattern ki sabse zaroori baat hai:

```cpp
class Character_Decorator : public Character   // ← IS-A  (inheritance)
{
protected:
    Character *character;                      // ← HAS-A (composition)
};
```

**Dono ek saath kyun?** Dono ka apna alag kaam hai:

| | Kaam | Iske bina kya hota |
| --- | --- | --- |
| **IS-A** (inherit) | Decorator **khud** ek `Character` hai | Uske upar doosra decorator **chadh hi nahi sakta** |
| **HAS-A** (wrap) | Decorator ke **andar** ek `Character` hai | Wrap kar hi nahi paate — wahi inheritance ka jaal |

> 📌 **DONO milke hi "parat pe parat" (stacking) possible banate hain.**
> Ye Decorator ki pehchaan hai — interview me ye line bol dena.

### Aur ek chhoti par bahut zaroori baat

```cpp
Character *character;   // ← INTERFACE ka pointer, `Mario*` NAHI!
```

Agar ye `Mario*` hota, to decorator sirf **Mario** ko wrap kar paata. Doosre
decorator ko nahi. Stacking khatam, pattern bekaar.

---

## 4. Chaar roles (GoF)

```
        ┌──────────────────────┐
        │     Character        │   ← 1. COMPONENT (interface)
        │  + getAbilities() =0 │
        └──────────▲───────────┘
                   │ implements
        ┌──────────┴───────────┬────────────────────────┐
        │                      │                        │
┌───────┴────────┐   ┌─────────┴──────────────┐         │
│     Mario      │   │  Character_Decorator   │         │
│ (2. CONCRETE   │   │  (3. DECORATOR, abstract)        │
│   COMPONENT)   │   ├────────────────────────┤         │
├────────────────┤   │ # Character* character │─────────┘
│ +getAbilities()│   └───────────▲────────────┘  HAS-A (wrap)
│  -> "Mario"    │               │ implements
└────────────────┘   ┌───────────┼───────────┐
                     │           │           │
              ┌──────┴───┐ ┌─────┴────┐ ┌────┴──────┐
              │ HeightUp │ │GunPowerUp│ │StarPowerUp│  ← 4. CONCRETE
              └──────────┘ └──────────┘ └───────────┘     DECORATORS
```

| # | Role | Class | Kaam |
| - | --- | --- | --- |
| 1 | Component | `Character` | Common contract — `getAbilities()` |
| 2 | ConcreteComponent | `Mario` | Asli basic object (recursion ka **base case**) |
| 3 | Decorator (abstract) | `Character_Decorator` | IS-A + HAS-A wala **base** |
| 4 | ConcreteDecorator | `HeightUp`, `GunPowerUp`, `StarPowerUp` | Asli power-ups |

---

## 5. Code ka poora flow — pyaaz ki parat

### Parat 1: basic Mario

```cpp
Character *mario = new Mario();
// -> "Mario"
```

### Parat 2: HeightUp chadha do

```cpp
mario = new HeightUp(mario);
//      └── naya object   └── purana mario ANDAR chala gaya
// -> "Mario with HeightUp"
```

⭐ Dhyaan do — `mario` **reassign** ho raha hai. Ab wo `HeightUp` ko point karta
hai, aur purana `Mario` uske **andar** hai. Ye chalti hai kyunki **dono
`Character*` hain** (IS-A ka fayda).

### Parat 3 aur 4

```cpp
mario = new GunPowerUp(mario);   // -> "Mario with HeightUp with Gun"
mario = new StarPowerUp(mario);  // -> "Mario with HeightUp with Gun with Star Power"
```

⭐ **Har baar Mario ki class bilkul nahi badli.** Sirf uske upar parat chadhti
gayi. Aur ye sab **runtime** pe hua — inheritance me ye namumkin hai.

### Asli output

```
Basic Character: Mario
After HeightUp: Mario with HeightUp
After GunPowerUp: Mario with HeightUp with Gun
After StarPowerUp: Mario with HeightUp with Gun with Star Power (Limited Time)

Just for learning :Mario with HeightUp with Gun with Star Power (Limited Time) with HeightUp with Gun with Star Power (Limited Time)
Destroying StarPowerUp Decorator
```

> 💡 Aakhri wale me "with HeightUp with Gun with Star Power" **do baar** kyun
> aaya? Kyunki `mario` me pehle se hi 4 parat thi, aur uske upar 3 aur chadha
> di — chain 7 objects ki ho gayi. Ye galti nahi, code me aisa hi likha hai.

---

## 6. Recursion kaise chalti hai

Har decorator ka `getAbilities()` bilkul ek jaisa hai:

```cpp
return character->getAbilities()  +  " with Gun";
//     └── (1) andar wale se poochho ─┘  └─ (2) apna jodo ─┘
```

Ye **recursion** hai. Sabse bahar se andar tak jaata hai, phir wapas bahar aate
hue har parat apna hissa jodti hai:

```
StarPowerUp.getAbilities()
  └─> GunPowerUp.getAbilities()
        └─> HeightUp.getAbilities()
              └─> Mario.getAbilities() = "Mario"     ← BASE CASE (chain rukti hai)
        <── "Mario with HeightUp"
  <── "Mario with HeightUp with Gun"
<── "Mario with HeightUp with Gun with Star Power (Limited Time)"
```

`Mario` **base case** hai — wo `character->getAbilities()` nahi bulata, seedha
`"Mario"` return karta hai. Isi liye recursion wahan ruk jaati hai.

---

## 7. 🐛 Ek asli BUG jo is code me tha (ab FIX hai)

Ye section isliye rakha hai kyunki ye Decorator ki **sabse aam galti** hai — aur
iska pakadna bahut seekhne layak tha.

### Bug ka saboot output me hi tha! 🔍

`main()` me **do** `StarPowerUp` bante hain. Uska destructor print karta hai:

```cpp
~StarPowerUp() { cout << "Destroying StarPowerUp Decorator" << endl; }
```

To message **do baar** aana chahiye, na? Par pehle **sirf ek baar** aata tha. 👀

### Jad kya thi

`Character_Decorator` me **destructor hi nahi tha**:

```cpp
class Character_Decorator : public Character {
protected:
    Character *character;
public:
    Character_Decorator(Character *c) { this->character = c; }
    // ❌ ~Character_Decorator() kahan hai?? `character` kabhi delete nahi hota!
};
```

To `delete mario` sirf **sabse bahar** wale decorator ko maarta tha. Andar ki
poori chain zinda reh jaati thi.

**Gin ke confirm kiya tha:**

```
Chain me objects: 7  (Mario + 6 decorators)
new    chala: 7 baar
delete chala: 1 baar      ← sirf ek!
--> LEAK: 6 objects 💀
```

### ✅ Fix — sirf ek line

```cpp
class Character_Decorator : public Character {
protected:
    Character *character;
public:
    Character_Decorator(Character *c) { this->character = c; }

    ~Character_Decorator() override { delete character; }   // ← ye ek line
};
```

**Aur sabse khoobsurat baat — ye khud RECURSIVE hai!** Ek line poori chain saaf
kar deti hai:

```
delete mario  (StarPowerUp)
  └─> ~Character_Decorator -> delete GunPowerUp
        └─> ~Character_Decorator -> delete HeightUp
              └─> ~Character_Decorator -> delete Mario   ← chain khatam ✅
```

Har parat apne **andar** wali parat ko maarti hai — pyaaz ulta chhilte hue. 🧅
Client ko sirf `delete mario` likhna hai, baaki sab apne aap.

### Fix ke baad — verify

```
new    chala: 7 baar
delete chala: 7 baar      ← ab saare!
--> LEAK: 0 ✅

ASan: clean (koi double-free / use-after-free nahi)
```

Aur output me ab `"Destroying StarPowerUp Decorator"` **do baar** aata hai —
jaisa hona chahiye tha.

> 📌 **Rule:** agar class raw pointer ki **maalik** hai, to uska destructor use
> delete kare. (Ya `unique_ptr<Character>` use karo — tab ye jhanjhat hi khatam.)
>
> 📌 **Sabak:** ek chhota sa `cout` destructor me daal do — memory bugs **turant**
> pakde jaate hain, bina kisi tool ke. Yahan wahi hua: output khud bug bata raha tha.

---

## 8. Kab use karein, kab nahi

### ✅ Use karo jab

- **Runtime** pe features add/remove karne hon
- Inheritance se **class explosion** ho raha ho (2ⁿ wala)
- Features ko **alag-alag combination** me lagana ho
- Base class ko **chhue bina** naye feature chahiye (Open/Closed)

### ❌ Mat use karo jab

- Sirf **1-2 fixed** variations hain → seedha inheritance kaafi hai
- Order **matter nahi** karta aur combos kam hain → over-engineering ho jaayega
- Bahut zyada parat → **debugging mushkil** ho jaati hai (stack trace me 10 layer!)

### ⚠️ Dhyaan rakhne wali baatein

| Problem | Kya hota hai |
| --- | --- |
| **Order matter karta hai** | `Gun(Star(mario))` aur `Star(Gun(mario))` ka output **alag** aayega |
| **Bahut layers = slow** | Har call poori chain se guzarti hai |
| **Debugging mushkil** | Stack trace me 10 nested `getAbilities()` |
| **Identity check tootta** | Wrapped object `==` se compare karo to fail hoga |
| **Memory** | Har parat ek object — aur delete karna yaad rakhna (section 7!) |

---

## 9. Asli duniya me kahan use hota hai

| Jagah | Component | Decorators |
| --- | --- | --- |
| **Java I/O** | `InputStream` | `BufferedInputStream`, `GZIPInputStream` |
| **C++ streams** | `ostream` | manipulators (`setw`, `fixed`) |
| **Coffee shop** ☕ | `Coffee` | `Milk`, `Sugar`, `Whipped Cream` (classic example) |
| **Pizza** 🍕 | `Pizza` | `ExtraCheese`, `Olives`, `Paneer` |
| **Web** | HTTP handler | Auth, Logging, Compression middleware |
| **Python** | function | `@decorator` (naam hi wahi hai!) |
| **Text editor** | `Text` | `Bold`, `Italic`, `Underline` |

> 💡 Java I/O sabse mashhoor example hai:
> `new BufferedReader(new InputStreamReader(new FileInputStream("f.txt")))`
> — bilkul wahi wrapping! 🧅

---

## 10. Decorator vs doosre patterns

Ye interview me poocha jaata hai — **structure milta-julta hai, niyat alag**:

| Pattern | Structure | **Niyat (intent)** |
| --- | --- | --- |
| **Decorator** | wrap karta hai, **same** interface | Naya **feature jodo** |
| **Adapter** | wrap karta hai, **alag** interface | Interface **badlo** (fit karo) |
| **Proxy** | wrap karta hai, **same** interface | **Control** karo (lazy load, permission, cache) |
| **Composite** | tree banata hai | Ek object aur group ko **ek jaisa** treat karo |

**Yaad rakhne ka tareeka:**

- **Decorator** → "aur bhi kuch **jodo**" (Mario + gun)
- **Adapter** → "ye plug fit nahi ho raha, **converter** lagao"
- **Proxy** → "seedha jaane nahi dunga, pehle **check** karunga"

> 📌 Decorator aur Proxy ka code **lagbhag ek jaisa** dikhta hai! Farak sirf
> WAJAH ka hai. Interview me bolo: _"Structure same hai — farak intent ka hai.
> Feature jodna Decorator, access control Proxy."_
>
> Proxy ki poori baat: [`L21 Proxy_Design_Pattern/`](../../L21%20Proxy_Design_Pattern/)

---

## 11. Interview me kya bolna

**Definition (ek line):**

> "Decorator ek structural pattern hai jo object pe **runtime** pe naye features
> wrap karke jodta hai — bina uski class badle."

**Kyun use karein (teen point):**

1. **Class explosion se bachte hain** — 2ⁿ classes → n+1
2. **Runtime pe** features add/remove ho sakte hain (inheritance me namumkin)
3. **Open/Closed** — base class chhue bina naye features

**Pattern ki pehchaan (ye bol dena impress karta hai 🎯):**

> "Decorator ki khaas baat ye hai ki wo **IS-A aur HAS-A dono** ek saath use
> karta hai. IS-A se wo khud Component banta hai (isi liye uske upar doosra
> decorator chadh sakta hai), aur HAS-A se wo Component ko wrap karta hai.
> Dono milke hi stacking possible banati hai."

**Follow-up jo aa sakte hain:**

- _"Decorator vs Proxy?"_ → section 10 (structure same, intent alag)
- _"Order matter karta hai?"_ → **Haan!** `Gun(Star(x)) ≠ Star(Gun(x))`
- _"Kya nuksan hai?"_ → bahut layers = debugging mushkil, aur **memory
  management** (section 7 wala leak!)
- _"Java me example?"_ → `BufferedReader(new InputStreamReader(...))`

---

## Summary — ek nazar me

| Cheez | Jawab |
| --- | --- |
| **Type** | Structural |
| **Ek line** | Runtime pe object pe feature wrap karo |
| **Solves** | Class explosion (2ⁿ → n+1) |
| **Key trick** | **IS-A + HAS-A** dono |
| **Base case** | `Mario` — recursion yahan rukti hai |
| **Principles** | Open/Closed, Single Responsibility |
| **Memory** | `~Character_Decorator()` recursive cleanup karta hai — ek `delete`, poori chain saaf ✅ |
