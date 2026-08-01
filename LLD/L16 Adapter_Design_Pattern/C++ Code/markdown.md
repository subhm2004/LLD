# 🚀 Adapter Design Pattern — Deep Dive

> **Ek line me:** _"Purane (legacy) code ko bina chede, naye system ke layak
> banana."_ 🔌
>
> 📄 Code: [`AdpaterPattern.cpp`](AdpaterPattern.cpp) — XML → JSON translator

---

## Table of Contents

1. [Pehle problem samjho](#1-pehle-problem-samjho)
2. [Adapter ka hal — plug converter](#2-adapter-ka-hal--plug-converter-)
3. [Chaar roles (GoF)](#3-chaar-roles-gof)
4. [⭐ IS-A + HAS-A](#4--is-a--has-a)
5. [Code ka poora flow](#5-code-ka-poora-flow)
6. [🐛 Do bugs jo mile (fix ho chuke)](#6--do-bugs-jo-mile-fix-ho-chuke)
7. [🧹 Memory & Ownership](#7--memory--ownership)
8. [Kab use karein / kab nahi](#8-kab-use-karein--kab-nahi)
9. [Fayde aur nuksan](#9-fayde-aur-nuksan)
10. [Adapter vs Decorator vs Proxy](#10-adapter-vs-decorator-vs-proxy--interview-favourite)
11. [Interview me kya bolna](#11-interview-me-kya-bolna)

---

## 1. Pehle problem samjho

Socho tumne ek **third-party library** use ki jo data **XML** me deti hai. Par
tumhara poora app **JSON** pe chalta hai.

Do "seedhe" hal hain — aur **dono bekaar**:

| Hal | Kyun nahi chalega |
| --- | --- |
| ❌ Library ka code badal do | Kar hi nahi sakte — tumhari hai hi nahi! Aur kar bhi lo to **agla update sab uda dega** |
| ❌ Poora app XML pe le aao | **50 jagah** code badalna padega. Aur kal nayi library aayi jo CSV deti hai? Phir se 50 jagah! 😩 |

**✅ Teesra hal — ADAPTER:** ek chhoti class beech me. **Dono taraf ka code
chhuna hi nahi padta.**

---

## 2. Adapter ka hal — plug converter 🔌

Asli duniya ka example: tumhara laptop ka charger **3-pin** ka hai, deewar me
socket **2-pin** ka.

- Charger badal sakte ho? ❌ (mehnga, aur warranty jaayegi)
- Socket badal sakte ho? ❌ (deewar todni padegi)
- **Beech me converter laga do?** ✅ **Bas! Yahi Adapter hai.**

```
   Client                Adapter                    Adaptee
  (JSON chahiye)      (translator)              (XML deta hai)
       │                    │                          │
       │──getJsonData()────>│                          │
       │                    │──getXmlData()───────────>│
       │                    │<─"<user><name>...</name>"│
       │                    │                          │
       │                    │  [XML parse karo]        │
       │                    │  [JSON banao]            │
       │<─{"name":"..."}────│                          │
       │                    │                          │
   ✅ khush! Usko pata bhi nahi ki peeche XML tha 🎯
```

---

## 3. Chaar roles (GoF)

```
        ┌──────────────────────┐
        │     IReports         │  ← 1. TARGET (client ko JO chahiye)
        │ + getJsonData() = 0  │
        └──────────▲───────────┘
                   │ implements (IS-A)
        ┌──────────┴────────────────┐
        │ XmlDataProviderAdapter    │  ← 3. ADAPTER (translator)
        ├───────────────────────────┤
        │ - XmlDataProvider* xmlProv│──┐ HAS-A (wrap)
        │ + getJsonData() override  │  │
        └───────────────────────────┘  │
                                       ▼
                            ┌──────────────────────┐
                            │  XmlDataProvider     │  ← 2. ADAPTEE (legacy)
                            │  + getXmlData()      │     ⚠ ISKO CHHU NAHI SAKTE
                            └──────────────────────┘

        ┌──────────────┐
        │   Client     │  ← 4. CLIENT — sirf IReports jaanta hai
        │ +getReport() │     (XML ka naam tak nahi pata!)
        └──────────────┘
```

| # | Role | Class | Kaam |
| - | --- | --- | --- |
| 1 | **Target** | `IReports` | Client ko **jo chahiye** (JSON) |
| 2 | **Adaptee** | `XmlDataProvider` | Jo **hai** par fit nahi hota (XML). **Legacy — chhu nahi sakte!** |
| 3 | **Adapter** | `XmlDataProviderAdapter` | Beech ka **translator** |
| 4 | **Client** | `Client` | Sirf Target se baat karta hai |

> ⭐ **Sabse zaroori:** `Client` class me `XmlDataProvider` ka **naam tak nahi
> hai**. "XML" shabd tak nahi! Poori translation chhup gayi.

---

## 4. ⭐ IS-A + HAS-A

```cpp
class XmlDataProviderAdapter : public IReports   // ← IS-A  (Target)
{
    XmlDataProvider* xmlProvider;                // ← HAS-A (Adaptee)
};
```

| | Kaam | Iske bina |
| --- | --- | --- |
| **IS-A** | Adapter **khud** ek `IReports` hai — client use apna hi samajhta hai | Client use accept hi nahi karta |
| **HAS-A** | Adapter ke **andar** legacy class hai, jisse asli kaam karwata hai | Translate kis cheez ko karega? |

> 💡 **Decorator (L13) me bhi IS-A + HAS-A dono the!** To farak? **Niyat ka** —
> section 10 dekho.

---

## 5. Code ka poora flow

### Input

```cpp
string rawData = "Shubham:124";
client->getReport(adapter, rawData);
```

### Andar kya hota hai

```
1. Client:  report->getJsonData("Shubham:124")
                  │
2. Adapter: ✅ validate karo — ':' hai kya? (haan)
                  │
3. Adapter: xmlProvider->getXmlData("Shubham:124")
                  │
4. Adaptee: return "<user><name>Shubham</name><id>124</id></user>"
                  │
5. Adapter: XML parse karo → name="Shubham", id="124"
                  │
6. Adapter: JSON banao → {"name":"Shubham", "id":124}
                  │
7. Client:  print → Processed JSON: {"name":"Shubham", "id":124}
```

### Parsing ka chhota detail

```cpp
size_t startName = xml.find("<name>") + 6;   // "<name>" = 6 character
```

```
"<user><name>Shubham</name>..."
       ^     ^
       |     +-- startName (yahan se naam shuru)
       +-- find() yahan point karta hai
```

`+ 6` isliye — hume tag ke **baad** wali jagah chahiye, tag ki shuruaat nahi.

> ⚠️ Ye **naive parsing** hai, asli XML parser nahi. Agar naam me hi `<name>`
> likha ho to toot jaayega. Yahan safe hai kyunki XML **hamesha adaptee ne khud
> banaya** hai. Asli project me proper library (pugixml) use karo.

### Asli output

```
Processed JSON: {"name":"Shubham", "id":124}

--- Galat format try karte hain (bina ':' ke) ---
✅ Reject: Galat format! 'name:id' chahiye (jaise "Shubham:124"), mila: "BinaColon"
```

---

## 6. 🐛 Do bugs jo mile (fix ho chuke)

### (a) `Client` object leak ho raha tha

```cpp
Client* client = new Client();   // ← new
...
delete adapter;
delete xmlProv;
// ❌ delete client; kahan hai??
```

**3 `new`, sirf 2 `delete`.** Gin ke pakda tha.

**✅ Fix:** `delete client;` add kar diya.

### (b) Bina `:` ke — chup-chaap INVALID JSON ⚠️

Ye zyada khatarnak tha, kyunki **crash nahi** hota tha — bas galat data aage
chala jaata tha:

```
Input : "ShubhamNoColon"      (koi ':' nahi)
Output: {"name":"ShubhamNoColon", "id":ShubhamNoColon}
                                        └── naam! aur bina quotes ke!
```

Ye **invalid JSON** hai. Aisa bug production me **hafton chhupa** reh sakta hai.

**Wajah:**

```cpp
size_t sep = data.find(':');       // ':' nahi mila → npos (vishaal number)
string name = data.substr(0, sep); // substr(0, npos) → POORI string
string id   = data.substr(sep + 1);// npos+1 wrap hoke 0 → PHIR poori string!
```

**✅ Fix — validation ADAPTER me:**

```cpp
if (data.find(':') == string::npos) {
    throw runtime_error("Galat format! 'name:id' chahiye ...");
}
```

> ⭐ **Validation adapter me kyun, adaptee me kyun nahi?**
>
> Kyunki adaptee **LEGACY** hai — hum use badal hi nahi sakte (yahi to poore
> pattern ki wajah hai!). Adapter **hamara** code hai, to input saaf karna uska
> kaam hai.
>
> 📌 Ye Adapter ki ek **asli zimmedari** hai: legacy ka contract lagu karwana,
> taaki usko galat input **jaaye hi na**.

### (c) Bonus — aadha-adhoora output

Ye bhi theek kiya:

```cpp
// ❌ Pehle — "Processed JSON: " print ho jaata, PHIR error aata
cout << "Processed JSON: " << report->getJsonData(rawData) << endl;
// Output: Processed JSON: ✅ Reject: Galat format! ...   🤦

// ✅ Ab — pehle data nikalo, phir print
string json = report->getJsonData(rawData);
cout << "Processed JSON: " << json << endl;
```

> 📌 **Sabak:** agar kaam **fail ho sakta hai**, to uska output tabhi likho jab
> wo **poora ho jaye**. Aadha print karke phir fail hona sabse ganda hai.

### Verify

```
Build : zero warnings (-Wall -Wextra)
Output: saaf (aadha-adhoora nahi)
Memory: 6 new, 6 delete → 0 leak ✅
ASan  : clean
```

---

## 7. 🧹 Memory & Ownership

> 📌 **Golden rule:** _"Pointer hone ka matlab MAALIK hona nahi hota."_

```
main()
  ├── OWNS ──> XmlDataProvider  (adaptee)   → main delete karta hai
  ├── OWNS ──> XmlDataProviderAdapter       → main delete karta hai
  │                 └── borrows ──> XmlDataProvider  ❌ delete NAHI karta
  └── OWNS ──> Client                       → main delete karta hai
```

| Kaun | Kiska maalik | Delete kaun karta |
| --- | --- | --- |
| `main()` | `XmlDataProvider` | `main()` |
| `main()` | Adapter | `main()` |
| `main()` | `Client` | `main()` ✅ (ye **pehle chhoot gaya tha**) |
| Adapter | ❌ kuch nahi | Adaptee ko sirf **borrow** karta hai |

### Adapter me destructor kyun nahi hai?

Kyunki wo `xmlProvider` ka **maalik nahi** hai! Agar wo `delete xmlProvider`
karta, to `main()` ka `delete xmlProv` **double-free** kar deta. 💥

> 💡 Aur saaf tareeka: `unique_ptr` ya stack objects use karo — is chhote demo me
> `new` ki zaroorat hi nahi thi.

---

## 8. Kab use karein / kab nahi

### ✅ Use karo jab

| Scenario | Example |
| --- | --- |
| **Third-party library** ka interface match nahi karta | Library XML deti, app JSON chahta |
| **Legacy code** ko naye system se jodna ho | 10 saal purana module |
| **Data format** convert karna ho | XML→JSON, Celsius→Fahrenheit |
| Class ka code **badal nahi sakte** | Vendor library, sealed class |
| Ek hi legacy class ke **kai formats** chahiye | XML→JSON aaj, XML→CSV kal |

### ❌ Mat use karo jab

- Interface **badal sakte ho** → seedha badal do, adapter ki zaroorat nahi
- Sirf **ek jagah** use ho raha → shayad ek simple function kaafi hai
- Naya code likh rahe ho → **shuru se hi sahi interface** banao!

---

## 9. Fayde aur nuksan

### ✅ Fayde

| Fayda | Detail |
| --- | --- |
| **Open/Closed** | Legacy class me **ek line bhi nahi** badalti — naya bug aane ka khatra zero |
| **Single Responsibility** | Poori translation ki sar-dardi **sirf adapter** me |
| **Reusability** | Ek hi legacy class ke liye **kai adapters** — XML→JSON, XML→CSV |
| **Client aazaad** | Client ko peeche ka kuch pata hi nahi |

### ❌ Nuksan

| Nuksan | Detail |
| --- | --- |
| **Extra class** | Chhote projects me overkill lag sakta hai |
| **Ek aur layer** | Bahut minor (negligible) performance cost |
| **Bahut adapters** | 10 legacy classes = 10 adapters — sambhalna padega |

---

## 10. Adapter vs Decorator vs Proxy — interview favourite

**Teeno wrap karte hain. Teeno me IS-A + HAS-A hai.** To farak?
**Sirf NIYAT (intent) ka:**

| Pattern | Interface | **Niyat** |
| --- | --- | --- |
| **Adapter** | **BADALTA** hai (XML→JSON) | Fit karo — "plug convert karo" |
| **Decorator** | **SAME** rehta hai | Feature **jodo** — "Mario + gun" |
| **Proxy** | **SAME** rehta hai | **Control** karo — "pehle permission check" |

**Yaad rakhne ka tareeka:**

- **Adapter** → _"ye plug fit nahi ho raha, converter lagao"_ 🔌
- **Decorator** → _"aur bhi kuch jodo"_ 🧅
- **Proxy** → _"seedha jaane nahi dunga, pehle check karunga"_ 🛡️

> 📌 Code lagbhag **ek jaisa** dikhta hai! Interview me bolo: _"Structure same
> hai — farak **intent** ka hai. Interface badalna Adapter, feature jodna
> Decorator, access control Proxy."_
>
> Detail: [L13 Decorator](../../L13%20Decorator_Design_Pattern/) ·
> [L21 Proxy](../../L21%20Proxy_Design_Pattern/)

---

## 11. Interview me kya bolna

**Definition (ek line):**

> "Adapter ek structural pattern hai jo **do incompatible interfaces** ke beech
> translator ka kaam karta hai — taaki purana code **bina badle** naye system ke
> saath chal sake."

**Kyun use karein (teen point):**

1. **Legacy/third-party code badal nahi sakte** — par use karna hai
2. **Open/Closed** — legacy me ek line bhi nahi badalti
3. **Client aazaad** — usko peeche ka format pata hi nahi chalta

**Pattern ki pehchaan (ye bol dena impress karta hai 🎯):**

> "Adapter me **IS-A aur HAS-A dono** hote hain — IS-A se wo Target ban jaata
> hai (client use apna samajhta hai), aur HAS-A se wo Adaptee ko wrap karta hai.
> Structure Decorator jaisa hi hai — farak sirf **intent** ka: Decorator
> interface same rakh ke feature jodta hai, Adapter interface hi badal deta hai."

**Follow-up jo aa sakte hain:**

- _"Adapter vs Decorator?"_ → section 10
- _"Object adapter vs Class adapter?"_ → Object = composition (ye code), Class =
  multiple inheritance (C++ me possible, Java me nahi)
- _"Adaptee me validation kyun nahi daali?"_ → **Kyunki wo legacy hai, use chhu
  nahi sakte!** Isi liye validation adapter me hai (section 6b) 🎯

---

## Summary — ek nazar me

| Cheez | Jawab |
| --- | --- |
| **Type** | Structural |
| **Ek line** | Do incompatible interfaces ke beech translator |
| **Analogy** | Plug converter 🔌 |
| **Key trick** | **IS-A + HAS-A** dono |
| **Sabse zaroori** | Adaptee (legacy) ko **chhua nahi jaata** |
| **Principles** | Open/Closed, Single Responsibility |
| **Is code me** | XML (legacy) → JSON (client chahta hai) |

---

_Summary: Adapter ek **"Plug-Converter"** ki tarah hai jo purane device ko naye
socket me chalne ki ijazat deta hai._ 🔌
