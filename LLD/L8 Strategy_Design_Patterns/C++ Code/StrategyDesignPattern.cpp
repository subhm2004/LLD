// ============================================================================
//  StrategyDesignPattern.cpp  —  Strategy Design Pattern (Behavioral)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  Strategy pattern kehta hai — "jo cheez badalti rehti hai (algorithm/behavior),
//  use alag class me nikaal do, aur main class me use INJECT kar do."
//
//  Yahan Robot ke 3 behaviors hain — walk, talk, fly. Agar ye teeno Robot class
//  ke andar hard-code karte ya inheritance se laate, to har combination ke liye
//  nayi subclass banani padti (FlyingWalkingRobot, NonFlyingTalkingRobot...).
//  Isse "class explosion" ho jaata hai.
//
//  SOLUTION: Har behavior ka apna interface banao (WalkableRobot, TalkableRobot,
//  FlyableRobot), uske concrete implementations banao (NormalWalk, NoWalk...),
//  aur Robot class in behaviors ko HAS-A (composition) relation se pakde —
//  IS-A (inheritance) se nahi.
//
//  FAYDA:
//  1. Ek hi Robot class alag-alag behavior combos ke saath ban sakti hai.
//  2. Naya behavior (jaise EcoWalk) add karna ho to Robot class ko touch
//     karne ki zaroorat nahi — sirf nayi strategy class banao (OCP follow).
//  3. Runtime pe bhi behavior swap kar sakte ho (setter bana ke).
//
//  Yaad rakhne ka mantra: "Composition over Inheritance"
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

// ============================================================================
//  SECTION 1: STRATEGY INTERFACES (Abstract Classes)
// ----------------------------------------------------------------------------
//  Har varying behavior ke liye ek alag interface. C++ me interface banane ka
//  tareeka = pure virtual function (= 0) wali abstract class.
//  In interfaces ka kaam sirf CONTRACT define karna hai — "jo bhi mujhe
//  implement karega, use ye method dena hi padega."
// ============================================================================

// --- Strategy Interface #1: Walk behavior ka contract ---
class WalkableRobot
{
public:
    // Pure virtual function — iska matlab ye class abstract hai, iska
    // object directly nahi ban sakta. Child classes ko walk() dena HI padega.
    virtual void walk() = 0;

    // Virtual destructor ZAROORI hai — kyunki hum base class pointer
    // (WalkableRobot*) se child object (NormalWalk) delete karenge.
    // Agar virtual nahi hota to child ka destructor call hi nahi hota
    // → memory leak / undefined behavior.
    virtual ~WalkableRobot() {}
};

// ============================================================================
//  SECTION 2: CONCRETE WALK STRATEGIES
// ----------------------------------------------------------------------------
//  Ye actual algorithms hain — interface ko implement karne wali classes.
//  Har class ka SIRF EK kaam hai (Single Responsibility Principle).
// ============================================================================

// --- Concrete Strategy: Normal chalne wala behavior ---
class NormalWalk : public WalkableRobot
{
public:
    // 'override' keyword se compiler check karta hai ki hum sach me
    // base class ka virtual function override kar rahe hain (typo bachata hai).
    void walk() override
    {
        cout << "Walking normally..." << endl;
    }
};

// --- Concrete Strategy: "No-op" strategy — robot chal hi nahi sakta ---
// Ye bhi ek valid strategy hai! "Kuch mat karo" bhi ek behavior hai.
// Isse null-check (if walkBehavior != nullptr) ki zaroorat nahi padti —
// isko "Null Object Pattern" ka flavour bhi keh sakte ho.
class NoWalk : public WalkableRobot
{
public:
    void walk() override
    {
        cout << "Cannot walk." << endl;
    }
};

// ============================================================================
//  SECTION 3: TALK BEHAVIOR — same pattern, alag dimension
// ----------------------------------------------------------------------------
//  Dhyan do: Walk aur Talk ALAG-ALAG interfaces hain, ek nahi.
//  Kyun? Kyunki ye INDEPENDENT dimensions hain — ek robot chal sakta hai
//  par bol nahi sakta. Agar dono ek hi interface me hote to ye
//  mix-and-match flexibility nahi milti.
// ============================================================================

// --- Strategy Interface #2: Talk behavior ka contract ---
class TalkableRobot
{
public:
    virtual void talk() = 0;
    virtual ~TalkableRobot() {}
};

// --- Concrete Strategy: Normal bolne wala behavior ---
class NormalTalk : public TalkableRobot
{
public:
    void talk() override
    {
        cout << "Talking normally..." << endl;
    }
};

// --- Concrete Strategy: Bol nahi sakta ---
class NoTalk : public TalkableRobot
{
public:
    void talk() override
    {
        cout << "Cannot talk." << endl;
    }
};

// ============================================================================
//  SECTION 4: FLY BEHAVIOR — teesra independent dimension
// ============================================================================

// --- Strategy Interface #3: Fly behavior ka contract ---
class FlyableRobot
{
public:
    virtual void fly() = 0;
    virtual ~FlyableRobot() {}
};

// --- Concrete Strategy: Normal udne wala behavior ---
class NormalFly : public FlyableRobot
{
public:
    void fly() override
    {
        cout << "Flying normally..." << endl;
    }
};

// --- Concrete Strategy: Ud nahi sakta ---
class NoFly : public FlyableRobot
{
public:
    void fly() override
    {
        cout << "Cannot fly." << endl;
    }
};

// ============================================================================
//  SECTION 5: CONTEXT CLASS — Robot
// ----------------------------------------------------------------------------
//  Ye Strategy pattern ka "Context" hai — wo class jo strategies ko USE
//  karti hai. Sabse important baat:
//
//  Robot ko pata hi nahi ki walk kaise hota hai! Wo bas apne paas rakhe
//  behavior object ko bolta hai — "bhai tu kar de" (DELEGATION).
//
//  HAS-A vs IS-A:
//  - Robot IS-A WalkableRobot   ❌ (inheritance — rigid, compile-time fixed)
//  - Robot HAS-A WalkableRobot* ✅ (composition — flexible, runtime pe inject)
// ============================================================================
class Robot
{
protected:
    // Ye teeno pointers hi COMPOSITION hain — Robot ke paas behaviors
    // "hai" (has-a), wo behaviors se "bana" nahi hai (is-a nahi).
    // Base class pointer rakha hai taaki KOI BHI concrete strategy
    // (NormalWalk ya NoWalk) yahan fit ho jaye — yahi polymorphism hai.
    WalkableRobot *walkBehavior;
    TalkableRobot *talkBehavior;
    FlyableRobot *flyBehavior;

public:
    // CONSTRUCTOR INJECTION — client (main) decide karta hai ki is robot
    // ko kaunse behaviors milenge. Robot khud kuch decide nahi karta.
    // Isi wajah se same Robot class se 8 alag combos ban sakte hain
    // (2 walk × 2 talk × 2 fly) — bina ek bhi nayi subclass banaye!
    Robot(WalkableRobot *w, TalkableRobot *t, FlyableRobot *f)
    {
        this->walkBehavior = w;
        this->talkBehavior = t;
        this->flyBehavior = f;
    }

    // Virtual destructor — do kaam karta hai:
    // 1. 'virtual' hone se: Robot* se CompanionRobot delete karo to
    //    child ka destructor bhi sahi se chalega.
    // 2. Andar delete: Robot apne behaviors ka MALIK (owner) hai,
    //    isliye unki heap memory clean karna Robot ki zimmedari hai.
    //    (Modern C++ me yahan unique_ptr use karte — delete chain gayab!)
    virtual ~Robot()
    {
        delete walkBehavior;
        delete talkBehavior;
        delete flyBehavior;
    }

    // ---------------- DELEGATION METHODS ----------------
    // Yahi Strategy pattern ka DIL hai. Robot khud kaam nahi karta,
    // apne injected behavior object ko forward kar deta hai.
    // robot->walk() → walkBehavior->walk() → NormalWalk::walk()
    void walk()
    {
        walkBehavior->walk();
    }
    void talk()
    {
        talkBehavior->talk();
    }
    void fly()
    {
        flyBehavior->fly();
    }

    // Ye pure virtual hai — har robot TYPE (Companion/Worker) apna
    // alag projection dikhayega. Dhyan do: projection behavior STRATEGY
    // nahi hai, ye robot ki IDENTITY hai — isliye ye inheritance se aata
    // hai, composition se nahi. (Vary hone wali cheez = strategy;
    // type-specific fixed cheez = inheritance — ye difference samajhna
    // interview me bahut kaam aata hai.)
    virtual void projection() = 0;
};

// ============================================================================
//  SECTION 6: CONCRETE CONTEXTS — Robot ke types
// ----------------------------------------------------------------------------
//  Ye classes SIRF robot ka type/identity define karti hain (projection).
//  Behaviors inse nahi aate — wo to constructor se inject hote hain.
//  Isliye ye classes itni chhoti aur clean hain.
// ============================================================================

// --- Concrete Context #1: Dost jaisa robot ---
class CompanionRobot : public Robot
{
public:
    // Behaviors ko seedha base class (Robot) ke constructor me
    // forward kar rahe hain — member initializer list se.
    CompanionRobot(WalkableRobot *w, TalkableRobot *t, FlyableRobot *f)
        : Robot(w, t, f) {}

    void projection() override
    {
        cout << "Displaying friendly companion features..." << endl;
    }
};

// --- Concrete Context #2: Kaam karne wala robot ---
class WorkerRobot : public Robot
{
public:
    WorkerRobot(WalkableRobot *w, TalkableRobot *t, FlyableRobot *f)
        : Robot(w, t, f) {}

    void projection() override
    {
        cout << "Displaying worker efficiency stats..." << endl;
    }
};

// ============================================================================
//  SECTION 7: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Client hi decide karta hai ki kaunsa robot kaunse behaviors ke saath
//  banega. Yahi "dependency injection" ka simple roop hai.
// ============================================================================
int main()
{
    // ROBOT 1: CompanionRobot — chal sakta hai, bol sakta hai,
    // par UD NAHI sakta (NoFly inject kiya).
    // Dekho kaise behaviors mix-and-match ho rahe hain — jaise
    // pizza me apni pasand ke toppings choose karna!
    Robot *robot1 = new CompanionRobot(new NormalWalk(), new NormalTalk(), new NoFly());
    robot1->walk();       // delegate → NormalWalk  → "Walking normally..."
    robot1->talk();       // delegate → NormalTalk  → "Talking normally..."
    robot1->fly();        // delegate → NoFly       → "Cannot fly."
    robot1->projection(); // virtual dispatch → CompanionRobot::projection()

    cout << "--------------------" << endl;

    // ROBOT 2: WorkerRobot — bilkul ulta combo!
    // Ud sakta hai par chal/bol nahi sakta.
    // SAME classes, ALAG strategies — ek bhi nayi subclass nahi banani padi.
    Robot *robot2 = new WorkerRobot(new NoWalk(), new NoTalk(), new NormalFly());
    robot2->walk();       // delegate → NoWalk      → "Cannot walk."
    robot2->talk();       // delegate → NoTalk      → "Cannot talk."
    robot2->fly();        // delegate → NormalFly   → "Flying normally..."
    robot2->projection(); // virtual dispatch → WorkerRobot::projection()

    // Cleanup — Robot ka virtual destructor chalega jo teeno behavior
    // objects ko bhi delete kar dega. Chain: delete robot1
    // → ~CompanionRobot() → ~Robot() → delete walk/talk/fly behaviors.
    delete robot1;
    delete robot2;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Walking normally...
//  Talking normally...
//  Cannot fly.
//  Displaying friendly companion features...
//  --------------------
//  Cannot walk.
//  Cannot talk.
//  Flying normally...
//  Displaying worker efficiency stats...
// ============================================================================
