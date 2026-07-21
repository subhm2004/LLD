# Design Patterns Used — JSON Parser LLD

Ye system ek JSON string ko in-memory TREE me badalta hai. Design do baaton pe khada hai:
ek structural pattern (**Composite** — JSON ke nested nature ko model karne ke liye) aur ek
parsing technique (**Recursive Descent** — grammar ko seedha code me utaarne ke liye). Saath
me model aur parser ka saaf separation (SRP) hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Composite ⭐    │  JsonValue + 6 subclasses       │  nested JSON tree        │
│  Recursive       │  JsonParser (parseValue/Object/  │  grammar -> code         │
│  Descent (tech)  │  Array/String/Number)           │  (mutual recursion)      │
│  SRP separation  │  models/ vs parser/             │  data vs logic decoupled │
│  Facade (light)  │  JsonParserApp                  │  demo lifecycle wrap     │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Composite Design Pattern ⭐ — `JsonValue` tree

### 📌 Concept
Composite pattern "part-whole" hierarchy ke liye hai: individual objects (leaf) aur unke
groups (composite) ko ek COMMON interface se treat karo, taaki client dono ke saath ek jaisa
code likhe. JSON iske liye perfect hai — kyunki JSON khud recursive/nested hai.

### PATTERN KE ROLES
- **Component (base)** → [`JsonValue`](./models/JsonValue.h) — common interface (`getType`, `print`)
- **Leaf nodes** (akeli value, andar aur kuch nahi):
  - [`JsonString`](./models/JsonString.h), [`JsonNumber`](./models/JsonNumber.h), [`JsonBoolean`](./models/JsonBoolean.h), [`JsonNull`](./models/JsonNull.h)
- **Composite nodes** (apne andar aur JsonValues rakhte hain):
  - [`JsonObject`](./models/JsonObject.h) (`map<string, JsonValue*>`), [`JsonArray`](./models/JsonArray.h) (`vector<JsonValue*>`)

```
JsonValue (component)
  ├── JsonObject / JsonArray   ← COMPOSITE (andar aur JsonValue* rakhte hain)
  └── String/Number/Boolean/Null ← LEAF (akeli value)
```

### ⭐ KYUN? (Why Composite)
> Client (aur `print()`) ko "ye leaf hai ya container" farak karne ki zaroorat HI NAHI. Ek
> array apne har child ka `print()` bulata hai — child leaf ho ya nested object, code ek jaisa.
> Isi tarah nested JSON (`{"a":[1,{"b":2}]}`) ek uniform tree ban jaati hai. Naya type add karna
> ho (jaise date)? Bas ek nayi `JsonValue` subclass — baaki code untouched.

### ⭐ Recursive cleanup — Composite + virtual destructor ka combined jaadu
> Har composite ka destructor apne children ko `delete` karta hai. Wo child agar khud container
> hai to uska destructor uske children ko... yaani **`delete root` se POORA tree recursively saaf**
> ho jaata hai. Ye tabhi kaam karta hai jab `JsonValue` ka destructor **virtual** ho (warna base
> pointer se delete pe derived ka destructor chalta hi nahi).

---

## 2. Recursive Descent Parsing (technique) — `JsonParser`

### 📌 Concept
Grammar ke har rule ka ek function banao, aur wo functions ek doosre ko **recursively** bulayein.
JSON grammar recursive hai (value → object → value → ...), to parser bhi recursive.

### Implementation ([`JsonParser`](./parser/JsonParser.h))
```
parseValue()  → pehla char dekho, dispatch karo:
   '{' → parseObject()   '[' → parseArray()   '"' → parseString()
   digit/- → parseNumber()   t/f/n → true/false/null

parseObject() → apne values ke liye parseValue() bulata hai  ─┐
parseArray()  → apne elements ke liye parseValue() bulata hai ─┼─ MUTUAL RECURSION
parseValue()  → phir parseObject/parseArray bula sakta hai   ─┘  (nesting handle)
```

### ⭐ Cursor (`pos_`) model
Parser input ko ek baar left-to-right padhta hai. `peek()` (dekho), `get()` (lo + aage badho),
`expect(c)` (ye char hona chahiye), `skipWhitespace()`. Simple aur samajhne layak.

### ⭐ KYUN? (Why recursive descent)
> Grammar → code ka **seedha, natural mapping**. Har rule = ek function, padhne me grammar jaisa
> lagta hai. Interview-scale JSON ke liye perfect (chhota, correct, extend karna aasaan). Bade/
> performance-critical parsers table-driven (LR/LALR) hote, par un ka code samajhna mushkil.

---

## 3. SRP — model (data) aur parser (logic) alag

`models/` sirf JSON ko **represent** karta hai (data + print). `parser/` string ko tree me
**badalta** hai (logic). Dono decoupled:
> Kal ko naya parser (jaise streaming/SAX-style) likhna ho, ya model se serializer/pretty-printer
> banana ho — ek doosre ko chhue bina ho sakta hai. (Requirements NFR2: "data model aur parser
> logic decoupled".)

---

## Object Relationships (UML samajhne ke liye)

```
JsonObject / JsonArray      ─▷  JsonValue        (Inheritance: composite is-a value)
String/Number/Boolean/Null  ─▷  JsonValue        (Inheritance: leaf is-a value)
JsonObject / JsonArray      ◆──  JsonValue        (Composition: OWNS children, dtor deletes)
JsonParser                  ··▶  all Json* types  (Dependency: banata + return karta)
JsonParserApp               ··▶  JsonParser       (Dependency: uses for demo)
```
- `─▷` Inheritance · `◆──` Composition (owns) · `··▶` Dependency

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| parse valid JSON | **Recursive descent** parser |
| 6 value types | **Composite** (JsonValue + 6 subclasses) |
| nested objects/arrays | Composite + mutual recursion |
| in-memory object model | JsonValue tree |
| descriptive errors on invalid syntax | `throw runtime_error(...)` in parser |
| basic escapes (\" \\ \/ \n \t) | `parseString()` escape handling |
| model/parser decoupled (NFR2) | `models/` vs `parser/` separation |

> **Cleanup + fix notes:**
> 1. 🧹 Har file me `#include <bits/stdc++.h>` tha (non-portable, GCC-only) — **hata diya**, targeted includes pe.
> 2. 🐛 **Memory leak fix (error paths):** `parseObject`/`parseArray` container ko `new` se banate
>    the, aur agar beech me exception aa jaati (invalid JSON), to already-bana `obj`/`arr`/`key`
>    **leak** ho jaata (delete kabhi nahi hota). Valid JSON pe theek, par INVALID pe har error = leak.
>    **Fix:** container ko `unique_ptr` me rakha (RAII) + success par `.release()`. Ab kahin bhi
>    exception aaye, unique_ptr auto-cleanup kar deta. **Verified:** 5 invalid inputs par ASan
>    **completely clean** (no leaks). Composite model classes waise hi raw-ptr rakhti hain — sirf
>    parser ke andar (jahan exceptions aati) RAII lagaya.

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Visitor pattern (print/serialize ke liye)?** Abhi `print()` har node pe ek virtual method hai
  (Composite ka uniform operation). Agar bahut saari operations chahiye (print, validate, serialize,
  transform...) bina har node ko chhue, tab **Visitor** add karte (operations ko nodes se alag).
  Ek hi operation ke liye Visitor over-engineering — abhi virtual method saaf hai.
- **`std::variant<...>` (single value type, no inheritance)?** Modern C++ me JSON value ko
  `variant` se bhi model kar sakte (no virtual, no heap). Achha alternative hai! Par Composite
  (inheritance) LLD/OOP interview me pattern dikhane ke liye clearer hai, aur `print()` polymorphism
  natural aata. Trade-off dono taraf.
- **`unique_ptr<JsonValue>` poore model me (raw ptr ki jagah)?** Aur behtar hota — manual delete hi
  na padta, saare leaks structurally impossible. Yahan model interview-simple (raw + dtor) rakha, par
  **parser ke andar unique_ptr** se error-path leaks fix kar diye. Production me poora tree `unique_ptr`.
- **Streaming / SAX parser?** Ye DOM-style hai (poora tree memory me banata). Bade files ke liye
  SAX (event-based, tree nahi banata) behtar — par requirements me streaming out of scope hai.
```
