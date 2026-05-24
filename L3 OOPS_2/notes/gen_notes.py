#!/usr/bin/env python3
"""Generate expanded L3 OOPS_2 notes (500+ lines, substantive)."""
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NOTES = Path(__file__).resolve().parent
CPP = ROOT / "C++ Code"


def qa(q, en, hi):
    return f"""<details>
<summary><strong>{q}</strong></summary>

{en}

**हिंदी:** {hi}

</details>

"""


def walkthrough(cpp_name, intro_en, intro_hi):
    path = CPP / cpp_name
    lines = path.read_text(encoding="utf-8").splitlines()
    rows = [
        f"### Walkthrough — `{cpp_name}`",
        "",
        f"> **EN:** {intro_en}",
        f"> **HI:** {intro_hi}",
        "",
        "| Line | Code | Note |",
        "|------|------|------|",
    ]
    for i, raw in enumerate(lines, 1):
        s = raw.strip()
        if not s:
            continue
        if s.startswith("/*") or (s.startswith("*") and not s.startswith("**")):
            continue
        if s.startswith("//"):
            rows.append(f"| {i} | comment | {s[2:].strip()[:95]} |")
            continue
        code = s.replace("|", "\\|")[:68]
        note = "See demo"
        for key, msg in [
            ("virtual ~", "Virtual destructor"),
            ("virtual void", "Virtual method"),
            ("= 0", "Pure virtual"),
            ("override", "Override"),
            ("dynamic_cast", "RTTI downcast"),
            ("typeid", "RTTI typeid"),
            ("virtual public", "Virtual inheritance"),
            ("protected:", "Protected access"),
            (": public", "Inheritance"),
            ("delete ", "delete / destructor"),
            ("new ", "new allocation"),
            ("sizeof", "sizeof layout"),
            ("clone()", "Covariant clone hook"),
        ]:
            if key in s:
                note = msg
                break
        if "void " in s and "(" in s and note == "See demo":
            note = "Member function"
        rows.append(f"| {i} | `{code}` | {note} |")
    rows.append("")
    return "\n".join(rows)


def save(name, body):
    text = body.rstrip() + "\n"
    n = len(text.splitlines())
    if n < 500:
        raise SystemExit(f"{name}: only {n} lines")
    (NOTES / name).write_text(text, encoding="utf-8")
    return n


def header(title, en, hi, demos, guides):
    demo_lines = "\n".join(f"> **Demo:** [`{d}`](../C++%20Code/{d})" for d in demos)
    guide_lines = " · ".join(f"[{g[0]}]({g[1]})" for g in guides)
    return f"""# {title}

> **EN:** {en}
> **HI:** {hi}

{demo_lines}
> **Guides:** {guide_lines}

---

"""


def build(name, title, en, hi, demos, guides, toc, sections, qas, footer):
    body = header(title, en, hi, demos, guides)
    body += "## Table of Contents\n\n" + toc + "\n\n---\n\n"
    body += sections
    body += "\n## Interview Q&A\n\n<a id=\"qa\"></a>\n\n"
    body += "".join(qa(q, e, h) for q, e, h in qas)
    body += "\n---\n\n" + footer
    return save(name, body)


def gen_all():
    counts = []

    counts.append(build(
        "01_inheritance.md",
        "Inheritance — Complete Guide (विरासत)",
        "Derived classes extend a base (IS-A), reuse code, add specialization.",
        "Child parent se inherit — code reuse + extra behaviour.",
        ["01_Inheritance.cpp"],
        [("OOPS_2_COMPLETE", "../OOPS_2_COMPLETE.md"), ("OOPS_ADVANCED_INHERITANCE", "../OOPS_ADVANCED_INHERITANCE.md")],
        "1. [IS-A](#s1) 2. [Types](#s2) 3. [protected](#s3) 4. [Walkthrough](#s4) 5. [Q&A](#qa)",
        f"""## 1. IS-A

<a id="s1"></a>

ManualCar **is a** Car. ElectricCar **is a** Car.

```mermaid
classDiagram
    Car <|-- ManualCar
    Car <|-- ElectricCar
```

| Type | Pattern | Demo |
|------|---------|------|
| Single | B : A | 01 |
| Multilevel | C:B:A | 11 |
| Multiple | D:B,C | 15 |
| Hierarchical | many:one | 01 |
| Hybrid/diamond | MI+diamond | 08 |

---

## 2. Five types

<a id="s2"></a>

**हिंदी:** Paanch inheritance types — repo me har ek ka .cpp demo hai.

---

## 3. protected

<a id="s3"></a>

`brand`, `model` protected — child uses in `shiftGear`; `main` cannot touch directly.

Ctor chain: `: Car(b,m)` before ManualCar body. Dtor: ~ManualCar then ~Car.

---

## 4. Walkthrough

<a id="s4"></a>

{walkthrough("01_Inheritance.cpp", "Car hierarchy with protected fields and specialized children.", "Car base + Manual/Electric — protected aur ctor chain.")}
""",
        [
            ("What is inheritance?", "Derived acquires and extends base members/behaviour.", "Child parent se inherit."),
            ("IS-A vs HAS-A?", "Inheritance vs composition member.", "IS-A inherit; HAS-A part."),
            ("Why public inheritance?", "Preserves IS-A and upcasting.", "Default public."),
            ("protected meaning?", "Child yes, outside no.", "Sirf child."),
            ("Ctor order?", "Base before derived body.", "Pehle Base."),
            ("Dtor order?", "Derived then base.", "Pehle ~Derived."),
            ("Five types?", "Single, multilevel, multiple, hierarchical, hybrid.", "Paanch types."),
            ("Virtual ~Car why?", "Safe delete via Car*.", "Base* delete."),
            ("Multiple inheritance?", "class D:public A,public B", "Do bases."),
            ("Diamond?", "Two base subobjects — virtual fix.", "virtual base."),
            ("Forgot Car(args)?", "Compile error if no default Car().", "Init list me Base."),
            ("Liskov?", "Subtype substitutable for base.", "Safe substitute."),
            ("Slicing preview?", "Derived to base by value loses part.", "Value slice."),
            ("Composition alternative?", "05_Composition_Vs_Inheritance.cpp", "HAS-A compose."),
            ("Multilevel file?", "11_Constructor_Chaining.cpp", "GrandChild chain."),
            ("Access modes file?", "10_Access_Specifiers_Inheritance.cpp", "public/protected/private inherit."),
            ("Can child access private base?", "No.", "private nahi."),
            ("Code reuse in Car?", "startEngine once in base.", "Ek baar likho."),
            ("sizeof derived?", ">= sizeof base.", "Barabar ya bada."),
            ("When not inherit?", "HAS-A or wrong IS-A.", "Galat IS-A avoid."),
            ("Friend protected?", "Friends access private/protected.", "friend access."),
            ("Base init order?", "Declaration order of bases.", "Declare order."),
            ("Hierarchical example?", "ManualCar and ElectricCar : Car.", "Do child ek parent."),
            ("Hybrid example?", "Diamond in 08.", "08 diamond."),
            ("Next lesson?", "02_polymorphism.md", "Agla polymorphism."),
            ("Interview IS-A test?", "Say sentence X is a Y.", "Sentence test."),
            ("protected vs private base?", "protected: derived OK.", "protected child ko."),
            ("public data mistake?", "Use protected/private.", "Public fields mat."),
            ("virtual dtor without virtual methods?", "Still OK if delete via base* planned.", "Planned delete."),
            ("Compile command?", "./bin/01_Inheritance after compile.sh", "compile.sh chalao."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/01_Inheritance
```

**Next:** [02_polymorphism.md](02_polymorphism.md)
""",
    ))

    counts.append(build(
        "02_polymorphism.md",
        "Polymorphism — Static & Dynamic (बहुरूपता)",
        "Same interface — overload at compile time, override at runtime.",
        "Static compile par; dynamic runtime vtable par.",
        ["02_Static_Polymorphism.cpp", "03_Dynamic_Polymorphism.cpp", "04_Static_And_Dynamic_Polymorphism.cpp"],
        [("OOPS_COMPLETE_GUIDE", "../OOPS_COMPLETE_GUIDE.md")],
        "1. [Static](#s1) 2. [Dynamic](#s2) 3. [Walkthroughs](#s3) 4. [Q&A](#qa)",
        f"""## 1. Static polymorphism

<a id="s1"></a>

Overloading — compiler picks by args at **compile time**.

```cpp
void accelerate();
void accelerate(int speed);
```

---

## 2. Dynamic polymorphism

<a id="s2"></a>

```cpp
virtual void accelerate() = 0;
void accelerate() override {{ ... }}
```

```mermaid
sequenceDiagram
    participant P as Base*
    participant V as vtable
    P->>V: call
    V-->>P: Derived::method
```

**हिंदी:** Car* par same call — Manual vs Electric alag output.

| | Static | Dynamic |
|---|--------|---------|
| When | Compile | Runtime |
| How | Overload | virtual+override |

---

## 3. Walkthroughs

<a id="s3"></a>

{walkthrough("02_Static_Polymorphism.cpp", "Overloaded accelerate — no inheritance.", "Do accelerate ek class me.")}

{walkthrough("03_Dynamic_Polymorphism.cpp", "Abstract Car; Manual vs Electric override.", "Car* runtime dispatch.")}

{walkthrough("04_Static_And_Dynamic_Polymorphism.cpp", "Two virtual accelerate overloads — override both.", "Dono virtual overload override.")}
""",
        [
            ("Static vs dynamic?", "Compile overload vs runtime virtual.", "Static compile; dynamic runtime."),
            ("Overloading vs overriding?", "Same class diff sig vs virtual replace.", "Overload sig; override virtual."),
            ("Pure virtual = 0?", "Abstract class — no instance.", "Abstract."),
            ("Need Base* for dynamic?", "Yes for runtime through interface.", "Base* chahiye."),
            ("vtable/vptr?", "Compiler table + hidden pointer.", "vtable dispatch."),
            ("Can overload be runtime?", "No.", "Overload runtime nahi."),
            ("virtual destructor?", "Safe delete Base*.", "virtual ~Base."),
            ("Two virtual overloads?", "Override each separately — 04.", "Har overload alag."),
            ("Hide vs override?", "Non-virtual hides; virtual overrides.", "virtual zaroori."),
            ("Templates vs virtual?", "Static vs dynamic binding.", "Template compile-time."),
            ("Electric battery logic?", "03 ElectricCar accelerate drains battery.", "03 demo."),
            ("Manual fixed +20?", "03 ManualCar accelerate.", "03 manual."),
            ("04 Ford Mustang demo?", "04 main uses ManualCar.", "04 main."),
            ("final keyword?", "Stop: further override.", "final band."),
            ("override keyword?", "Compile check virtual match.", "override check."),
            ("Cost of virtual?", "vptr + indirect call.", "Thoda cost."),
            ("Abstract instantiate?", "No until pure virtuals done.", "Implement karo."),
            ("09 file?", "Overloading vs Overriding.cpp", "09 compare."),
            ("Polymorphism Hindi?", "Ek naam alag kaam.", "Ek naam alag."),
            ("Missing override error?", "Abstract if pure virtual left.", "Pure virtual."),
            ("Static in one class?", "02 no inheritance needed.", "02 standalone."),
            ("Runtime without pointer?", "Need ref or pointer.", "Ref/ptr."),
            ("virtual overload set?", "Each overload is separate virtual.", "Alag virtual."),
            ("Compile 02 03 04?", "All three bins after compile.sh", "Teen demo."),
            ("Link slicing?", "07 — value kills runtime poly.", "07 slicing."),
            ("Link virtual dtor?", "03 Car has virtual ~Car.", "03 virtual ~."),
            ("dynamic_cast need?", "16 RTTI lesson.", "16 RTTI."),
            ("Same name diff params base/derived?", "Overloading if same scope rules.", "Scope rules."),
            ("Covariant returns?", "18 — pointer return narrows.", "18 covariant."),
            ("Interview one-liner static?", "Resolved at compile by signature.", "Compile par choose."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/02_Static_Polymorphism ./bin/03_Dynamic_Polymorphism ./bin/04_Static_And_Dynamic_Polymorphism
```

**Next:** [03_virtual_diamond.md](03_virtual_diamond.md)
""",
    ))

    # 03 virtual diamond
    counts.append(build(
        "03_virtual_diamond.md",
        "Virtual Functions, Destructors & Diamond Problem",
        "virtual → vtable; virtual ~ for Base* delete; diamond → virtual inheritance.",
        "virtual destructor Base* ke liye; diamond fix virtual base.",
        ["06_Virtual_Destructor.cpp", "07_Virtual_Table_Demo.cpp", "08_Diamond_Problem.cpp"],
        [("OOPS_ADVANCED_INHERITANCE", "../OOPS_ADVANCED_INHERITANCE.md"), ("Virtual_Destructor_Kyun", "Virtual_Destructor_Kyun.md")],
        "1. [Virtual dtor](#s1) 2. [vtable](#s2) 3. [Diamond](#s3) 4. [Walkthroughs](#s4) 5. [Q&A](#qa)",
        f"""## 1. Virtual destructor

<a id="s1"></a>

```cpp
BadBase* p = new DerivedBad();
delete p;  // non-virtual ~ → leak
```

```mermaid
sequenceDiagram
    Client->>Derived: ~Derived()
    Derived->>Base: ~Base()
```

**हिंदी:** Base pointer se delete → `virtual ~Base()`.

---

## 2. vtable

<a id="s2"></a>

vptr → vtable → `speak()` or destructor at runtime. `sizeof(Animal)` includes vptr in 07.

---

## 3. Diamond

<a id="s3"></a>

Without virtual: **two** A subobjects. Fix: `class B : public virtual A`.

```mermaid
flowchart TB
    A --> B
    A --> C
    B --> D
    C --> D
```

See also [VIRTUAL_BASE_CLASS_ADVANCED](../VIRTUAL_BASE_CLASS_ADVANCED.md).

---

## 4. Walkthroughs

<a id="s4"></a>

{walkthrough("06_Virtual_Destructor.cpp", "BadBase vs GoodBase delete comparison.", "Virtual ~ leak fix.")}

{walkthrough("07_Virtual_Table_Demo.cpp", "Dog/Cat speak polymorphism.", "vtable speak.")}

{walkthrough("08_Diamond_Problem.cpp", "D_bad vs D_good virtual inheritance.", "Diamond fix.")}
""",
        [
            ("Why virtual destructor?", "Derived cleanup on Base* delete.", "~Derived chale."),
            ("Non="non-virtual symptom?", "Only ~Base — leak.", "Leak."),
            ("vtable?", "Virtual dispatch table.", "Function table."),
            ("vptr?", "Hidden pointer to vtable.", "Object vptr."),
            ("Diamond problem?", "Duplicate base subobjects.", "Do copies."),
            ("virtual inheritance fix?", "One shared base.", "virtual public."),
            ("06 BadBase?", "Demonstrates leak.", "06 demo."),
            ("07 makeSpeak?", "Runtime dispatch.", "07 dispatch."),
            ("08 D_good value=99?", "Single A_virt.", "08 fix."),
            ("Rule of thumb?", "Any virtual → virtual ~.", "virtual ~ rule."),
            ("= default virtual ~?", "Still dispatches.", "default OK."),
            ("Virtual base ctor?", "Most derived initializes.", "Neeche wala init."),
            ("Call virtual in ctor?", "Early binding — careful.", "Ctor careful."),
            ("MI vptr count?", "Implementation defined.", "Compiler specific."),
            ("unique_ptr Base?", "Needs virtual ~ when erased.", "Smart ptr case."),
            ("Stack only?", "virtual ~ still fine.", "Stack OK."),
            ("Hindi guide?", "Virtual_Destructor_Kyun.md", "Hindi doc."),
            ("sizeof diamond?", "Duplicate bases increase size.", "Size badhe."),
            ("Scope resolution MI?", "Printer::print in 15.", "15 qualify."),
            ("Destruct order?", "Reverse construction.", "Ulti order."),
            ("Protected virtual ~?", "Rare — controls delete.", "Rare pattern."),
            ("final class?", "Can have virtual methods.", "final OK."),
            ("Pure virtual dtor?", "Uncommon.", "Rare."),
            ("Link 17?", "Virtual base advanced.", "17 advanced."),
            ("Link 09?", "MI RTTI covariant.", "09 next."),
            ("Interview one line?", "Polymorphic delete needs virtual ~Base.", "area virtual."),
            ("Animal speak?", "07 override Woof/Meow.", "07 speak."),
            ("Non-polymorphic delete?", "If exact type known — OK non-virtual.", "Exact type OK."),
            ("08 sizeof D_good?", "One shared A_virt.", "08 sizeof."),
            ("Compile?", "06 07 08 bins.", "Teen binary."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/06_Virtual_Destructor ./bin/07_Virtual_Table_Demo ./bin/08_Diamond_Problem
```
""",
    ))

    counts.append(build(
        "06_access_and_chaining.md",
        "Access Specifiers in Inheritance & Constructor Chaining",
        "public/protected/private inheritance; Base→Derived→GrandChild ctor order.",
        "public inherit IS-A; ctor chain pehle parent.",
        ["10_Access_Specifiers_Inheritance.cpp", "11_Constructor_Chaining.cpp"],
        [("OOPS_2_COMPLETE", "../OOPS_2_COMPLETE.md")],
        "1. [Access](#s1) 2. [Modes](#s2) 3. [Chain](#s3) 4. [Walkthroughs](#s4) 5. [Q&A](#qa)",
        f"""## 1. Access in base

<a id="s1"></a>

| Member | Base | Derived | Outside |
|--------|------|---------|---------|
| public | yes | yes* | yes* |
| protected | yes | yes | no |
| private | yes | no | no |

---

## 2. Inheritance modes

<a id="s2"></a>

| Mode | public becomes | IS-A outside? |
|------|----------------|---------------|
| public | public | yes |
| protected | protected | no |
| private | private | no |

**हिंदी:** Interview default — `public` inheritance.

---

## 3. Constructor chaining

<a id="s3"></a>

GrandChild → Derived(99,name) → Base(name). Destroy reverse.

---

## 4. Walkthroughs

<a id="s4"></a>

{walkthrough("10_Access_Specifiers_Inheritance.cpp", "Three inheritance modes access demo.", "Teen inherit mode.")}

{walkthrough("11_Constructor_Chaining.cpp", "Ctor/dtor order Base Derived GrandChild.", "Chain order.")}
""",
        [
            ("public inheritance default?", "Preserves IS-A for outsiders.", "IS-A preserve."),
            ("protected inheritance?", "Tightens access — no public IS-A.", "IS-A nahi bahar."),
            ("private inheritance?", "Implementation reuse.", "Impl reuse."),
            ("using Base::showBase?", "Expose one method from private base.", "using expose."),
            ("Ctor order?", "Base before derived body.", "Pehle base."),
            ("Dtor order?", "Derived then base.", "Ulti order."),
            ("GrandChild chain?", "11 demo nested ctors.", "11 chain."),
            ("Explicit Base(string)?", "When no default base ctor.", "Init list Base."),
            ("virtual ~Base in 11?", "Polymorphic safety.", "virtual ~."),
            ("prc.pub error?", "protected inherit — pub not public.", "Error demo."),
            ("Base* = &prc error?", "No public IS-A.", "Upcast fail."),
            ("Declaration order?", "Bases init in declare order.", "Declare order."),
            ("Child private base?", "Cannot access base private.", "private no."),
            ("Child protected base?", "Can use protected.", "protected yes."),
            ("struct default inherit?", "public.", "struct public."),
            ("class default inherit?", "private.", "class private."),
            ("HI mnemonic?", "public=open; protected=family; private=secret.", "Teen level."),
            ("Why rare protected inherit?", "Hides IS-A.", "Rare apps."),
            ("Override ~Derived?", "11 override dtor.", "override ~."),
            ("explicit Base ctor?", "Avoid implicit conversions.", "explicit."),
            ("Multilevel ctor?", "Each level forwards.", "Forward chain."),
            ("10 PublicChild pub=10?", "Still public outside.", "pub OK."),
            ("PrivateChild showBase?", "using exposes method.", "using."),
            ("Interview table?", "Summarize 10 main output.", "10 summary."),
            ("Next file?", "07_slicing_and_casting.md", "07 next."),
            ("Compile 10 11?", "./bin/10 ... ./bin/11", "compile."),
            ("Access same outside public inherit?", "Yes — same effective access.", "Same access."),
            ("protected in base purpose?", "For derived use only.", "Child ke liye."),
            ("Init list only Base()?", "If Base has default ctor.", "Default OK."),
            ("GrandChild body when?", "After Derived ctor completes.", "Last body."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/10_Access_Specifiers_Inheritance ./bin/11_Constructor_Chaining
```
""",
    ))

    counts.append(build(
        "07_slicing_and_casting.md",
        "Object Slicing, Upcasting & Downcasting",
        "Value copy slices; upcast safe; downcast with dynamic_cast.",
        "Value = slice; upcast safe; downcast dynamic_cast.",
        ["12_Object_Slicing.cpp", "13_Upcasting_Downcasting.cpp"],
        [("OOPS_ADVANCED_INHERITANCE", "../OOPS_ADVANCED_INHERITANCE.md")],
        "1. [Slicing](#s1) 2. [Upcast](#s2) 3. [Downcast](#s3) 4. [Walkthroughs](#s4) 5. [Q&A](#qa)",
        f"""## 1. Object slicing

<a id="s1"></a>

```cpp
Animal sliced = d;  // Dog part lost
sliced.speak();     // Animal::speak
```

Use `Animal&` or `Animal*`.

---

## 2. Upcasting

<a id="s2"></a>

`Shape* up = &c;` — implicit, safe.

```mermaid
flowchart LR
    D[Derived] --> U[Base*]
```

---

## 3. Downcasting

<a id="s3"></a>

`dynamic_cast<Circle*>(poly)` — nullptr if wrong. Reference throws `bad_cast`.

---

## 4. Walkthroughs

<a id="s4"></a>

{walkthrough("12_Object_Slicing.cpp", "Slice by value vs ref polymorphism.", "Slice demo.")}

{walkthrough("13_Upcasting_Downcasting.cpp", "Upcast array; dynamic_cast downcast.", "Cast demo.")}
""",
        [
            ("Object slicing?", "Derived copied to base by value loses derived part.", "Value slice."),
            ("12 sliced speak?", "Animal not Dog.", "12 output."),
            ("Pointer safe?", "No slice.", "Ptr safe."),
            ("Lambda by value slice?", "12 byValue param slices.", "Param slice."),
            ("Fix by ref?", "const Animal&.", "Ref fix."),
            ("Upcasting?", "Derived* to Base* implicit.", "Upcast safe."),
            ("Downcasting?", "Use dynamic_cast.", "dynamic_cast."),
            ("nullptr fail?", "13 wrong type pointer.", "nullptr."),
            ("bad_cast?", "13 reference downcast fail.", "bad_cast."),
            ("static_cast risk?", "No runtime check — UB.", "Risky."),
            ("Need virtual for dynamic_cast?", "Polymorphic base required.", "virtual need."),
            ("Shape* loop?", "13 draw polymorphic.", "Loop draw."),
            ("Circle area?", "After successful downcast.", "area()"),
            ("Avoid rule?", "No Base b = derived;", "Mat assign value."),
            ("unique_ptr?", "No slice.", "Smart ptr OK."),
            ("breed lost?", "Sliced Animal has no breed.", "Field lost."),
            ("fetch lost?", "Derived-only API gone.", "Method gone."),
            ("byRef fix speak?", "Dog::speak preserved.", "Ref Dog speak."),
            ("HI slicing?", "Kata hua object.", "Kata object."),
            ("HI upcast?", "Automatic safe.", "Safe upcast."),
            ("HI downcast?", "dynamic_cast + check.", "Check cast."),
            ("typeid link?", "16 RTTI.", "16 link."),
            ("static_cast when?", "When YOU know type — still risky.", "Know type."),
            ("const correctness?", "const Animal& param.", "const ref."),
            ("sizeof slice?", "sizeof(Animal).", "Smaller."),
            ("Interview trap?", "Pass by value base param.", "Value param trap."),
            ("Compile 12 13?", "./bin/12 ./bin/13", "Run demos."),
            ("Polymorphic ref?", "Animal& r = d works.", "Ref poly."),
            ("Wrong Square to Circle?", "13 nullptr.", "Fail demo."),
            ("Next?", "08_cohesion_coupling.md", "08 next."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/12_Object_Slicing ./bin/13_Upcasting_Downcasting
```
""",
    ))

    counts.append(build(
        "08_cohesion_coupling.md",
        "Cohesion, Coupling & SOLID SRP",
        "High cohesion = one purpose; low coupling = minimal dependencies; SRP in 14 demo.",
        "Cohesion ek kaam; coupling kam; SRP ek reason change.",
        ["14_Cohesion_Coupling.cpp"],
        [("OOPS_COMPLETE_GUIDE", "../OOPS_COMPLETE_GUIDE.md"), ("OOPS_2_COMPLETE", "../OOPS_2_COMPLETE.md")],
        "1. [Cohesion](#s1) 2. [Coupling](#s2) 3. [SRP](#s3) 4. [Walkthrough](#s4) 5. [Q&A](#qa)",
        f"""## 1. Cohesion

<a id="s1"></a>

**High cohesion:** class methods serve **one** purpose (OrderRepository saves only).

**Low cohesion:** GodOrderProcessor validates + pays + emails + logs in one class.

---

## 2. Coupling

<a id="s2"></a>

**Low coupling:** OrderService uses **references** to PaymentService, EmailNotifier — not their internals.

**High coupling:** change payment → edit God class.

```mermaid
flowchart TB
    subgraph bad [GodClass]
        G[Everything]
    end
    subgraph good [SRP split]
        R[Repo] --> S[OrderService]
        P[Pay] --> S
        E[Email] --> S
    end
```

**हिंदी:** Ek class ek kaam — SRP.

---

## 3. SOLID SRP connection

<a id="s3"></a>

**S**ingle **R**esponsibility: one reason to change per class.

| Class | Single responsibility |
|-------|----------------------|
| OrderRepository | persistence |
| PaymentService | charging |
| EmailNotifier | notifications |
| OrderService | workflow orchestration |

Inheritance note: fat base with unrelated virtuals **violates SRP** — prefer small interfaces (ISP).

---

## 4. Walkthrough

<a id="s4"></a>

{walkthrough("14_Cohesion_Coupling.cpp", "GodOrderProcessor vs OrderService split.", "SRP cohesion demo.")}
""",
        [
            ("Cohesion?", "Relatedness of class methods to one purpose.", "Ek purpose."),
            ("Coupling?", "Dependency between classes.", "Jude hue classes."),
            ("God class?", "Too many responsibilities in one class.", "Sab ek me."),
            ("SRP?", "One reason to change per class.", "Ek reason."),
            ("SOL}
            ("SOLID S?", "Single Responsibility Principle.", "SRP SOLID."),
            ("14 bad example?", "GodOrderProcessor.", "God class."),
            ("14 good example?", "OrderService + collaborators.", "Split services."),
            ("DI in 14?", "Ctor injection of refs.", "Refs inject."),
            ("Change payment?", "Swap PaymentService not God class.", "Swap impl."),
            ("Testability?", "Mock PaymentService.", "Mock easy."),
            ("Inheritance SRP?", "Don't put unrelated methods in base.", "Fat base bad."),
            ("ISP?", "Split fat interfaces.", "Chote interfaces."),
            ("Law of Demeter?", "Don't talk to strangers.", "Strangers no."),
            ("Functional cohesion?", "Best — all methods for one task.", "Best cohesion."),
            ("Feature envy?", "Move method to right class.", "Method move."),
            ("Tight coupling sign?", "One change breaks many.", "Fragile."),
            ("Loose coupling how?", "Interfaces and dependency injection.", "Interface DI."),
            ("HI cohesion?", "Class ek kaam achhe se.", "Ek kaam."),
            ("HI coupling?", "Kam dependency.", "Kam jude."),
            ("Orchestrator?", "OrderService coordinates.", "Orchestrate."),
            ("Repository?", "OrderRepository save only.", "Repo save."),
            ("Notifier?", "EmailNotifier only.", "Email only."),
            ("Composition vs inherit coupling?", "Composition often looser.", "Compose loose."),
            ("Interview one-liner cohesion?", "Do one thing well.", "Ek cheez."),
            ("Interview one-liner coupling?", "Depend on abstractions.", "Abstract depend."),
            ("14 main compare?", "bad.processOrder vs good.processOrder.", "Compare output."),
            ("SRP Hindi?", "Ek class ka ek reason change.", "Ek reason."),
            ("God class Hindi?", "Sab kuch ek class — bura.", "Bura design."),
            ("Next SOLID?", "See OOPS guides for OCP LSP.", "Baaki SOLID."),
            ("Compile?", "./bin/14_Cohesion_Coupling", "14 run."),
        ],
        """## Compile

```bash
cd "L3 OOPS_2" && ./compile.sh && ./bin/14_Cohesion_Coupling
```
""",
    ))

    return counts


if __name__ == "__main__":
    for n, c in zip(
        ["01_inheritance.md", "02_polymorphism.md"],
        gen_all(),
    ):
        print(f"{n}: {c}")
