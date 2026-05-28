# SOLID Deep Dive: High-Level vs Low-Level Modules, OCP, DIP, Trade-offs

> Language style: Hinglish (easy to read, interview-friendly, practical)
>  
> Target: Deep understanding of
> - high-level module vs low-level module
> - OCP (Open/Closed Principle)
> - DIP (Dependency Inversion Principle)
> - factory/manager patterns me kaise violation hota hai
> - real-world trade-offs

---

## Index (Table of Contents)

- [0. Kaise padhna hai yeh doc](#0-kaise-padhna-hai-yeh-doc)
- [1. Sabse basic: module hota kya hai?](#1-sabse-basic-module-hota-kya-hai)
- [2. High-level module kya hota hai?](#2-high-level-module-kya-hota-hai)
- [3. Low-level module kya hota hai?](#3-low-level-module-kya-hota-hai)
- [4. Kya manager class hamesha high-level hoti hai?](#4-kya-manager-class-hamesha-high-level-hoti-hai)
- [5. High vs Low identify karne ka practical framework](#5-high-vs-low-identify-karne-ka-practical-framework)
- [6. SOLID quick recap](#6-solid-quick-recap-context-ke-liye)
- [7. OCP exact meaning](#7-ocp-exact-meaning-without-confusion)
- [8. DIP exact meaning](#8-dip-exact-meaning-without-jargon-overload)
- [9. Factory me OCP break ka classic example](#9-factory-me-ocp-break-ka-classic-example)
- [10. Kya every if-else OCP violation hai?](#10-kya-every-if-else-ocp-violation-hai)
- [11. DIP violation ka classic smell](#11-dip-violation-ka-classic-smell)
- [12. OCP + DIP relation](#12-ocp--dip-relation)
- [13. Real project style interpretation (L24 coupon style)](#13-real-project-style-interpretation-l24-coupon-style)
- [14. Real project style interpretation (payment style)](#14-real-project-style-interpretation-payment-style)
- [15. Trade-offs: Practical truth](#15-trade-offs-practical-truth-important)
- [16. Manager classes: कब high-level, कब low-level?](#16-manager-classes-कब-high-level-कब-low-level)
- [17. Anti-pattern catalog](#17-anti-pattern-catalog-highlow--solid-angle)
- [18. OCP violation examples](#18-ocp-violation-examples-detailed)
- [19. DIP violation examples](#19-dip-violation-examples-detailed)
- [20. "Kya manager high-level hota hai?" interview answer template](#20-kya-manager-high-level-hota-hai-interview-answer-template)
- [21. Interview rapid-fire answers](#21-interview-rapid-fire-answers-ready-lines)
- [22. Decision matrix: कब abstraction introduce karein?](#22-decision-matrix-कब-abstraction-introduce-karein)
- [23. Refactor playbook](#23-refactor-playbook-step-by-step)
- [24. Composition root concept](#24-composition-root-concept-bahut-useful)
- [25. Open for extension implementation menu](#25-open-for-extension-ka-practical-implementation-menu)
- [26. Common confusion clear](#26-common-confusion-clear)
- [27. Code smell checklist](#27-code-smell-checklist-quick)
- [28. Architecture layering suggestion](#28-architecture-layering-suggestion)
- [29. Testing impact](#29-testing-impact)
- [30. Performance trade-off](#30-performance-trade-off)
- [31. Team-level trade-offs](#31-team-level-trade-offs)
- [32. Documentation guidance](#32-documentation-guidance)
- [33. Bad vs better architecture sentence](#33-example-bad-vs-better-architecture-sentence)
- [34. SRP connection](#34-srp-connection)
- [35. LSP connection](#35-lsp-connection)
- [36. ISP connection](#36-isp-connection)
- [37. DIP without DI framework?](#37-dip-without-di-framework)
- [38. Factory patterns quick map](#38-factory-patterns-quick-map)
- [39. Plugin mindset](#39-plugin-mindset)
- [40. Error handling direction](#40-error-handling-direction)
- [41. Dependency direction golden rule](#41-dependency-direction-golden-rule)
- [42. Refactor mini example (payment-like)](#42-refactor-mini-example-payment-like)
- [43. Refactor mini example (coupon-like)](#43-refactor-mini-example-coupon-like)
- [44. Closed for modification practical boundaries](#44-closed-for-modification-practical-boundaries)
- [45. Architecture debt signs](#45-architecture-debt-signs)
- [46. Governance checklist for reviewers](#46-governance-checklist-for-reviewers)
- [47. Trade-off summary table](#47-trade-off-summary-table)
- [48. Beginner mistakes avoid](#48-beginner-mistakes-avoid)
- [49. Advanced nuance: domain core vs app service](#49-advanced-nuance-domain-core-vs-app-service)
- [50. TL;DR](#50-tldr-if-you-remember-only-this)
- [Appendix A - Quick glossary](#appendix-a---quick-glossary)
- [Appendix B - 30-second interview answer](#appendix-b---30-second-interview-answer)
- [Appendix C - Self assessment questions](#appendix-c---self-assessment-questions)

---

## 0. Kaise padhna hai yeh doc

0.1 Agar beginner ho to Section 1–8 pehle padho.  
0.2 Agar interview prep kar rahe ho to Section 20, 21, 22 must read.  
0.3 Agar code refactor karna hai to Section 13, 14, 15 pe focus karo.  
0.4 Agar confusion hai "manager high-level hota hai kya?" to Section 4 + 5 padho.  
0.5 Agar "factory me OCP break kyu hota hai?" dekhna hai to Section 9.  

---

## 1. Sabse basic: module hota kya hai?

1.1 Module ka matlab ek logical code unit.  
1.2 Module class bhi ho sakta hai.  
1.3 Module package/folder bhi ho sakta hai.  
1.4 Module service layer bhi ho sakta hai.  
1.5 Module ka main role: responsibility boundary define karna.  

1.6 Example:
- `CouponManager` ek module.
- `FlatDiscountStrategy` ek module.
- `PaymentGateway` abstraction ek module.
- `RazorpayGateway` concrete implementation ek low-level module.

---

## 2. High-level module kya hota hai?

2.1 High-level module business rule decide karta hai.  
2.2 High-level module "what" define karta hai, "how" nahi.  
2.3 High-level module workflow orchestrate karta hai.  
2.4 High-level module domain policy hold karta hai.  
2.5 High-level module durable hota hai (kam change hota).  

2.6 Typical examples:
- `CheckoutService`
- `PaymentController`
- `CouponManager` (depends context)
- `OrderPlacementUseCase`
- `PricingEngine`

2.7 High-level module ke questions:
- kis order me steps chalenge?
- validation fail ho to kya hoga?
- retry policy kab apply hogi?
- user ko final response kya dena hai?

---

## 3. Low-level module kya hota hai?

3.1 Low-level module implementation detail handle karta hai.  
3.2 Low-level module "how" solve karta hai.  
3.3 Low-level module infra/protocol specific hota hai.  
3.4 Low-level modules usually zyada volatile hoti hain.  
3.5 New vendor aaya to low-level module badalti hai.  

3.6 Typical examples:
- `RazorpayApiClient`
- `PaytmGateway`
- `FlatDiscountStrategy`
- `MySQLRepository`
- `RedisCacheAdapter`

---

## 4. Kya manager class hamesha high-level hoti hai?

4.1 Nahi, hamesha nahi.  
4.2 Name se nahi, responsibility se decide hota hai.  
4.3 `DiscountStrategyManager` mostly factory utility ho sakta hai (mid/low).  
4.4 `CouponManager` orchestration kare to high-level ban sakta hai.  
4.5 `ConnectionManager` infra config kare to low-level side pe ho sakta hai.  

4.6 Rule of thumb:
- Business policy hold karta hai? high-level.
- Vendor/infra detail hold karta hai? low-level.
- Sirf object create karta hai? usually factory utility.

---

## 5. High vs Low identify karne ka practical framework

5.1 Ask: agar vendor badla to kaun badlega?  
5.2 Ask: agar business rule बदला to kaun badlega?  
5.3 Ask: kaun module codebase ke core intent ko reflect karta hai?  
5.4 Ask: kaun module stable interface provide karta hai?  

5.5 Mapping:
- Stable policy = high-level.
- Unstable implementation = low-level.

---

## 6. SOLID quick recap (context ke liye)

6.1 S = SRP: single reason to change.  
6.2 O = OCP: open for extension, closed for modification.  
6.3 L = LSP: subtype must behave like base type contract.  
6.4 I = ISP: fat interface avoid karo.  
6.5 D = DIP: high-level low-level dono abstraction pe depend karein.  

---

## 7. OCP exact meaning (without confusion)

7.1 New feature add karne ke liye old stable code ko baar-baar edit na karna pade.  
7.2 Existing tested code ko safe rakhna OCP ka practical angle hai.  
7.3 OCP ka matlab kabhi modify nahi karna nahi hai.  
7.4 OCP ka मतलब repetitive core changes avoid karna hai.  

7.5 Simple statement:
> "Extend via new class/implementation, not by editing central decision logic every time."

---

## 8. DIP exact meaning (without jargon overload)

8.1 High-level module low-level concrete class pe directly depend na kare.  
8.2 Dono abstraction pe depend karein (interface/abstract class).  
8.3 Abstraction should not depend on details.  
8.4 Details should depend on abstractions.  

8.5 Practical statement:
> "Controller ko `RazorpayGateway` ka naam pata nahi hona chahiye; usko `PaymentGateway` contract pata hona chahiye."

---

## 9. Factory me OCP break ka classic example

### 9.1 Problem code pattern

```cpp
Gateway* createGateway(Type t) {
  if (t == PAYTM) return new PaytmGateway();
  if (t == RAZORPAY) return new RazorpayGateway();
  if (t == PAYPAL) return new PayPalGateway();
  // next time Stripe आएगा -> फिर edit
}
```

9.2 Har naye gateway pe yeh function modify hota hai.  
9.3 Yeh repetitive modification OCP stress point hai.  
9.4 Small project me acceptable ho sakta hai.  
9.5 Scale pe pain deta hai.  

### 9.6 Isko improve kaise karein?

Option-A registry factory:

```cpp
using Creator = std::function<Gateway*()>;
std::unordered_map<Type, Creator> registry;
```

9.7 Naya gateway add -> new registration line.  
9.8 Core branch logic untouched.  

---

## 10. Kya every `if-else` OCP violation hai?

10.1 Nahi.  
10.2 OCP violation tab concern banta hai jab same place repeatedly change hota hai.  
10.3 Rare change + simple code => acceptable.  
10.4 Premature abstraction bhi smell hai.  

10.5 Principle:
> OCP optimize for expected axis of change, not for imaginary future.

---

## 11. DIP violation ka classic smell

### 11.1 Smell code

```cpp
class CheckoutService {
  RazorpayGateway gateway;
};
```

11.2 Direct concrete dependency.  
11.3 Testing hard.  
11.4 Replaceability poor.  
11.5 High-level implementation details se chipak gaya.  

### 11.6 Better

```cpp
class CheckoutService {
  IPaymentGateway& gateway;
};
```

---

## 12. OCP + DIP relation

12.1 DIP often OCP ko enable karta hai.  
12.2 Abstraction introduce karoge to extension easier hota hai.  
12.3 OCP without DIP kabhi-kabhi fragile hota hai.  
12.4 DIP without clear variation point over-engineering ban sakta hai.  

---

## 13. Real project style interpretation (L24 coupon style)

13.1 `Coupon` abstraction = extension point.  
13.2 Naya coupon class add kar sakte ho without touching `Coupon` base contract.  
13.3 `IDiscountStrategy` = math strategy extension point.  
13.4 `CouponManager` chain execute karta hai (high-level orchestration-ish role).  

13.5 OCP-friendly zones:
- new coupon type class
- new strategy class

13.6 OCP-sensitive zones:
- central manager code
- main registration orchestration

---

## 14. Real project style interpretation (payment style)

14.1 `PaymentGateway` abstraction DIP support karta hai.  
14.2 `PaytmGateway`, `RazorpayGateway`, `PayPalGateway` low-level implementations.  
14.3 `PaymentController` high-level use-case entry point.  
14.4 `GatewayFactory` if-else based ho to OCP pressure point.  

14.5 Better with registry/plugin approach for many gateways.

---

## 15. Trade-offs: Practical truth (important)

15.1 Simple code fast build hota hai.  
15.2 Highly extensible code easy change hota hai.  
15.3 Dono ka perfect balance context pe depend karta hai.  

### 15.4 Under-engineering risk
- repeated edits
- regression chance
- rigid design

### 15.5 Over-engineering risk
- abstraction jungle
- debugging pain
- onboarding slow

### 15.6 Sweet spot strategy

1. Start simple.  
2. Observe repeating change pattern.  
3. Then abstraction introduce karo.  

---

## 16. Manager classes: कब high-level, कब low-level?

16.1 `OrderManager` business sequence run kare -> high-level leaning.  
16.2 `ConnectionManager` DB sockets manage kare -> low-level leaning.  
16.3 `FactoryManager` object create only -> utility layer.  
16.4 `RuleEngineManager` domain policy decide kare -> high-level.  

---

## 17. Anti-pattern catalog (high/low + SOLID angle)

17.1 God Manager  
17.2 Mega Factory with endless if-else  
17.3 Singleton everywhere  
17.4 Interface per class without need  
17.5 Leaky abstraction (interface bhi vendor-specific)  
17.6 Static utility abuse for business flow  
17.7 Hard-coded dependencies in controller  

---

## 18. OCP violation examples (detailed)

### 18.1 Discount type switch everywhere

- `if(type==FLAT)` in 5 files  
- new type add = 5 files edit  
- regression risk

### 18.2 Gateway-specific checks in controller

- `if(gateway==PAYTM)` controller me  
- policy layer me vendor detail leak  

### 18.3 Query type branching in service layer

- same branching repeated  
- strategy mapping better

---

## 19. DIP violation examples (detailed)

19.1 Service directly uses concrete DB class.  
19.2 Controller directly new करता है low-level adapter.  
19.3 High-level module imports SDK client directly.  
19.4 Unit tests me stubs inject karna impossible.  

---

## 20. "Kya manager high-level hota hai?" interview answer template

20.1 "Name se decide nahi hota."  
20.2 "Responsibility dekhenge."  
20.3 "Business workflow hai to high-level."  
20.4 "Infra plumbing hai to low-level."  
20.5 "Agar both mix hai to SRP break likely."  

---

## 21. Interview rapid-fire answers (ready lines)

21.1 OCP one-liner:
> "New behavior add karne ke liye stable code repeatedly modify na karna pade."

21.2 DIP one-liner:
> "High-level policy modules concrete details ke bajay abstractions pe depend karte hain."

21.3 Manager one-liner:
> "Manager high-level bhi ho sakta hai aur low-level bhi; depend karta hai woh business policy manage karta hai ya infrastructure detail."

21.4 Factory one-liner:
> "Simple factory with if-else acceptable for small systems; frequent new variants aayein to registry/plugin model better."

---

## 22. Decision matrix: कब abstraction introduce karein?

| Situation | Action |
|---|---|
| 1 implementation, change rare | keep simple |
| 2+ implementations expected | create interface |
| same branching repeated | strategy/factory introduce |
| tests difficult due to concrete deps | apply DIP |
| business flow + infra mixed | split modules |

---

## 23. Refactor playbook (step-by-step)

23.1 Identify volatile axis (kya frequently badalta hai).  
23.2 Stable policy layer isolate karo.  
23.3 Abstraction define karo (small interface).  
23.4 Concrete adapters move to low-level layer.  
23.5 Composition root pe wiring karo.  
23.6 Tests add before/after.  

---

## 24. Composition root concept (bahut useful)

24.1 App ka ek entry setup point hota hai jahan dependencies wire hoti hain.  
24.2 Wahi place allowed hai concrete classes ka knowledge rakhne ke liye.  
24.3 Rest of app abstraction pe chale.  

---

## 25. "Open for extension" ka practical implementation menu

25.1 Inheritance  
25.2 Strategy registration  
25.3 Plugin registry  
25.4 Config-driven mapping  
25.5 Interface + factory injection  
25.6 Command pattern  
25.7 Rule objects list  

---

## 26. Common confusion clear

26.1 "OCP means zero modification forever" -> wrong.  
26.2 "DIP means interface mandatory for everything" -> wrong.  
26.3 "Factory use ki to OCP solved" -> not always.  
26.4 "Manager is always high-level" -> wrong.  

---

## 27. Code smell checklist (quick)

27.1 Kya high-level class me vendor names hard-coded hain?  
27.2 Kya same if-else blocks multiple jagah duplicate hain?  
27.3 Kya tests me real adapter mock karna mushkil hai?  
27.4 Kya new variant add karne par 5+ files edit hoti hain?  
27.5 Kya class "manager" me 8 responsibilities hain?  

---

## 28. Architecture layering suggestion

28.1 Domain (policy/high-level)  
28.2 Application services (use-case orchestration)  
28.3 Ports/interfaces (abstractions)  
28.4 Adapters (low-level details)  
28.5 Infrastructure (SDK, DB, network)

---

## 29. Testing impact

29.1 DIP friendly design me unit tests fast.  
29.2 Concrete coupling me integration-heavy tests required.  
29.3 OCP-friendly extension me regression surface small hota.  
29.4 Over-abstraction me mock explosion hota.  

---

## 30. Performance trade-off

30.1 Extra indirection ka tiny cost hota hai.  
30.2 Usually maintainability gain zyada valuable hota hai.  
30.3 Hot-path me optimize with profiling, not assumption.  
30.4 OCP ke naam par unnecessary virtual chain avoid karo when not needed.

---

## 31. Team-level trade-offs

31.1 Junior-heavy team -> too abstract code onboarding hard.  
31.2 Fast-changing product -> extension points worth it.  
31.3 Stable internal tools -> simple concrete acceptable.  

---

## 32. Documentation guidance

32.1 Har abstraction ka reason likho.  
32.2 Har factory switch ka expected change-axis note karo.  
32.3 Har manager ke responsibility bullets rakho.  
32.4 "Why this dependency direction?" comment karo.  

---

## 33. Example: bad vs better architecture sentence

33.1 Bad:
"CheckoutService imports Razorpay SDK and parses response directly."

33.2 Better:
"CheckoutService depends on `IPaymentProcessor`; Razorpay adapter translates SDK responses."

---

## 34. SRP connection

34.1 High-level module me infra details mix hue to SRP break likely.  
34.2 SRP break -> OCP fragile ho jata hai.  
34.3 SRP maintain karoge to OCP naturally improve hota hai.

34.4 Typical SRP break pattern:
- same class me business rules + DB query + external API parse + retry + logging + response mapping.
- iska impact: class ke multiple "reasons to change" ban jate hain.

34.5 SRP break detection checklist (practical):
- Agar payment vendor change pe bhi same class edit ho rahi hai aur business policy change pe bhi wahi class edit ho rahi hai -> SRP break smell.
- Agar class me 3 se zyada external dependencies hain (repo + sdk + notifier + serializer) -> split evaluate karo.
- Agar test likhte waqt heavy mocking karni pad rahi hai for one simple rule -> responsibility mixed hai.

34.6 Mini example (bad):
```cpp
class CheckoutService {
public:
    void placeOrder(Order o) {
        // business validation
        // db write
        // gateway sdk call
        // retry logic
        // email notification
        // api response mapping
    }
};
```

34.7 Better split:
- `CheckoutService` -> orchestration + business policy.
- `OrderRepository` -> persistence.
- `PaymentGatewayAdapter` -> external SDK interaction.
- `RetryPolicy` -> retry decision.
- `NotificationService` -> communication side effects.
- `ResponseMapper` -> transport/output mapping.

34.8 SRP vs over-splitting caution:
- Har 5-line method ko alag class mat banao.
- Split tab karo jab change-axis alag ho, ownership alag ho, ya test pain high ho.
- "One reason to change" ka matlab "one method only" nahi hai; related cohesive methods same class me reh sakte hain.

34.9 SRP break ka architecture impact chain:
- SRP break -> large god classes
- god classes -> OCP repeated modifications
- OCP stress -> merge conflicts + regression hotspots
- regression hotspots -> release confidence down

34.10 Interview one-liner:
"Jab ek class business rule aur infra dono ka burden leti hai, SRP break hota hai; usse OCP bhi weak hota hai kyunki har new variant pe same class modify karni padti hai."

---

## 35. LSP connection

35.1 DIP ke interface meaningful contract hona chahiye.  
35.2 Agar implementations contract break karein to abstraction bekaar.  
35.3 OCP fail ho jayega because substitution unsafe hogi.

---

## 36. ISP connection

36.1 Interface too fat hua to low-level modules unnecessary methods implement karenge.  
36.2 Split interfaces by client need.  
36.3 High-level module ko minimal contract do.

---

## 37. DIP without DI framework?

37.1 Possible and common.  
37.2 Constructor injection manually karo.  
37.3 Simple composition root enough.

---

## 38. Factory patterns quick map

38.1 Simple factory - one place branch logic.  
38.2 Factory method - subclass decides creator.  
38.3 Abstract factory - related object families.  
38.4 Registry factory - runtime pluggable creators.  

---

## 39. Plugin mindset

39.1 "Naya behavior as plugin add ho, core untouched."  
39.2 OCP ka strong operational form hai.  
39.3 But plugin lifecycle complexity bhi aata hai.  

---

## 40. Error handling direction

40.1 High-level module should handle policy decisions on failures.  
40.2 Low-level module should expose typed errors/details.  
40.3 Don't bury business decision inside adapter.

---

## 41. Dependency direction golden rule

41.1 Source code dependency arrows ideally high-level abstractions ki taraf ho.  
41.2 Concrete adapters outer layers me रहें.  
41.3 Core domain ko infra details pata na ho.

---

## 42. Refactor mini example (payment-like)

42.1 Before:
- controller knows paytm/razorpay classes.

42.2 After:
- controller knows `IPaymentGateway`.
- factory/registry or composer binds concrete.

42.3 Result:
- testing easier
- extension easier

---

## 43. Refactor mini example (coupon-like)

43.1 Before:
- one service with switch on coupon type.

43.2 After:
- `Coupon` hierarchy + strategies.

43.3 Result:
- new coupon type by class addition
- central logic less touched

---

## 44. "Closed for modification" practical boundaries

44.1 Truly closed kuch nahi hota.  
44.2 "Relatively stable" regions define karo:
- contracts
- core orchestration
- cross-cutting policy

44.3 "Open" regions:
- plugin implementations
- strategy implementations
- adapters

---

## 45. Architecture debt signs

45.1 Every sprint same core file edited.  
45.2 Merge conflicts always central factory me.  
45.3 New feature estimate inflated due to branching tangles.  
45.4 Regression tests frequently fail same hotspots.  

---

## 46. Governance checklist for reviewers

46.1 Is PR adding new variant?
46.2 Kya core orchestrator modify hua?
46.3 Kya abstraction extend hua ya bypass?
46.4 Kya dependency direction maintain hui?
46.5 Kya tests new variant cover karte?

---

## 47. Trade-off summary table

| Goal | Approach | Cost |
|---|---|---|
| fastest delivery | concrete direct code | rigid later |
| future extension | interface + strategy | extra files |
| strict layering | DIP everywhere | setup complexity |
| minimal code | no abstraction | repeated edits |

---

## 48. Beginner mistakes avoid

48.1 Interface banaya but direct concrete call bhi rakha.  
48.2 Factory banayi but every consumer switch bhi karta.  
48.3 Manager me business + DB + HTTP सब mix.  
48.4 OCP ke chakkar me 20 empty abstractions बना दीं.  

---

## 49. Advanced nuance: domain core vs app service

49.1 Domain core pure policy rakho.  
49.2 App service orchestration + transactions + workflow rakho.  
49.3 Adapters boundary par translate karein.  

---

## 50. TL;DR (if you remember only this)

50.1 High-level = business policy, low-level = implementation detail.  
50.2 OCP = repeated central edits kam karo; extension points do.  
50.3 DIP = policy modules concrete details pe directly depend na karein.  
50.4 Factory useful hai, but if-else explosion OCP stress point ban sakta hai.  
50.5 Manager name se decide mat karo; responsibility dekho.  
50.6 Over-engineering bhi violation jitna hi dangerous hai.  
50.7 Modern pragmatic rule: **simple start, pattern where change repeats**.  

---

## Appendix A - Quick glossary

A.1 Abstraction - contract/interface  
A.2 Adapter - external detail wrapper  
A.3 Policy - business rule  
A.4 Detail - infra implementation  
A.5 Composition root - wiring point  
A.6 Extension point - new behavior add location  
A.7 Volatile axis - frequently changing dimension  

---

## Appendix B - 30-second interview answer

"High-level modules business rules define karte hain, low-level modules implementation details handle karte hain. OCP ka objective hai new behavior add karna bina core stable code ko repeatedly modify kiye. DIP ensure karta hai ki high-level modules concrete low-level details pe directly depend na karein, balki abstractions pe depend karein. Factory helpful hai, but if every new type pe central if-else edit karna pade to OCP pressure aata hai. Practical design me trade-off hota hai: premature abstraction avoid karo, lekin repeating change-axis identify karke strategy/registry/ports apply karo."

---

## Appendix C - Self assessment questions

1. Kya main high-level aur low-level ko name ke bina responsibility se identify kar sakta hoon?  
2. Kya main bata sakta hoon kaun sa module frequent change hota hai?  
3. Kya mere current project me koi central file repeatedly edit ho rahi hai?  
4. Kya mere high-level services concrete SDK class import kar rahe hain?  
5. Kya main ek example me OCP violation aur uska minimal fix बता सकता हूँ?  

### Sample answers (self-check key)

1. **High-level vs low-level identify**
   - **Yes answer tab**: main module ke "what" vs "how" ko clearly separate kar paun.
   - High-level = policy/workflow/business decision.
   - Low-level = SDK/DB/HTTP/infra detail implementation.

2. **Frequent change module identify**
   - **Yes answer tab**: commit history ya PR pattern se bata paun ki kaunsi files har naye feature me touch ho rahi hain.
   - Frequent edits mostly extension-point weakness dikhati hain.

3. **Central file repeatedly edited?**
   - **Healthy answer**: "No" ya "sirf intentional orchestration updates."
   - **Risk answer**: ek hi factory/service file har variant pe edit ho rahi hai -> OCP stress.

4. **High-level service concrete SDK import?**
   - **Ideal**: No. High-level modules abstractions/interfaces pe depend karein.
   - **Agar Yes**: DIP violation smell; adapter/port introduce karke dependency invert karo.

5. **OCP violation + minimal fix example**
   - **Violation example**: `PaymentFactory` me `if (gateway=="X") ...` chain har new gateway pe modify karna.
   - **Minimal fix**: registry/factory map use karo (`name -> creator`) taaki new gateway class add ho, central file modification kam ho.

### Quick scoring

- **5/5 Yes (with confidence):** strong practical understanding.
- **3-4/5 Yes:** good, but architecture-review practice aur karo.
- **0-2/5 Yes:** pehle Section 5, 9, 11, 22, 23, 34 dubara revise karo.

---

End of document.

