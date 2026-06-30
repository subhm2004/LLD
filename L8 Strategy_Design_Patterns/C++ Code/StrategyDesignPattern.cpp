// ============================================================================
//  StrategyDesignPattern.cpp  —  Strategy Design Pattern (Behavioral)
// ----------------------------------------------------------------------------
//  Strategy = ek behavior (algorithm) ko alag class me nikaal kar, runtime par
//  plug/swap karna — "composition over inheritance". Robot ke walk/talk/fly
//  behaviors fixed inheritance se nahi aate, balki INJECT hote hain. Isse
//  ek hi Robot type alag-alag behaviors ke saath ban sakta hai, aur naya
//  behavior add karne ke liye Robot class ko chhedna nahi padta (OCP).
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

// --- Strategy interface: Walk behavior ---
class WalkableRobot
{
public:
    virtual void walk() = 0; // interface / abstract class
    virtual ~WalkableRobot() {}
};

// --- Concrete walk strategies ---
class NormalWalk : public WalkableRobot
{
public:
    void walk() override
    {
        cout << "Walking normally..." << endl;
    }
};

class NoWalk : public WalkableRobot // "no-op" strategy — chal hi nahi sakta
{
public:
    void walk() override
    {
        cout << "Cannot walk." << endl;
    }
};

// --- Strategy interface: Talk behavior ---
class TalkableRobot
{
public:
    virtual void talk() = 0;
    virtual ~TalkableRobot() {}
};

// --- Concrete talk strategies ---
class NormalTalk : public TalkableRobot
{
public:
    void talk() override
    {
        cout << "Talking normally..." << endl;
    }
};

class NoTalk : public TalkableRobot
{
public:
    void talk() override
    {
        cout << "Cannot talk." << endl;
    }
};

// --- Strategy interface: Fly behavior ---
class FlyableRobot
{
public:
    virtual void fly() = 0;
    virtual ~FlyableRobot() {}
};

class NormalFly : public FlyableRobot
{
public:
    void fly() override
    {
        cout << "Flying normally..." << endl;
    }
};

class NoFly : public FlyableRobot
{
public:
    void fly() override
    {
        cout << "Cannot fly." << endl;
    }
};

// --- Robot base class (Context) ---
// Behaviors ko HAS-A (composition) ke through rakhta hai, inherit nahi karta.
class Robot
{
protected:
    // has-a relation — yahi strategy injection hai
    WalkableRobot *walkBehavior;
    TalkableRobot *talkBehavior;
    FlyableRobot *flyBehavior;

public:
    // Behaviors constructor me inject hote hain -> runtime par decide.
    Robot(WalkableRobot *w, TalkableRobot *t, FlyableRobot *f)
    {
        this->walkBehavior = w;
        this->talkBehavior = t;
        this->flyBehavior = f;
    }
    virtual ~Robot()
    {
        delete walkBehavior;
        delete talkBehavior;
        delete flyBehavior;
        // heap memory clean ho jaayegi (Robot apne behaviors ko own karta hai)
    }

    // Robot khud kaam nahi karta — behavior object ko delegate karta hai.
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

    virtual void projection() = 0; // subclasses apna roop dikhayenge
};

// --- Concrete robot types ---
class CompanionRobot : public Robot
{
public:
    CompanionRobot(WalkableRobot *w, TalkableRobot *t, FlyableRobot *f)
        : Robot(w, t, f) {}

    void projection() override
    {
        cout << "Displaying friendly companion features..." << endl;
    }
};

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

// --- Main ---
int main()
{
    // robot1: chal/bol sakta hai par ud nahi sakta — behaviors mix-and-match.
    Robot *robot1 = new CompanionRobot(new NormalWalk(), new NormalTalk(), new NoFly());
    robot1->walk();
    robot1->talk();
    robot1->fly();
    robot1->projection();

    cout << "--------------------" << endl;

    // robot2: ud sakta hai par chal/bol nahi — same classes, alag strategies.
    Robot *robot2 = new WorkerRobot(new NoWalk(), new NoTalk(), new NormalFly());
    robot2->walk();
    robot2->talk();
    robot2->fly();
    robot2->projection();

    delete robot1;
    delete robot2;

    return 0;
}
