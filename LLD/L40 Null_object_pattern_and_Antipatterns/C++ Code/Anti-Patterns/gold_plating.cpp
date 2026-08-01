/*
=====================================================================
GOLD PLATING / OVER ENGINEERING ANTI-PATTERN
=====================================================================
THEORY:
Gold Plating tab hota hai jab hum ek simple problem ke liye
zaroorat se zyada complex solution bana dete hain - jaise ek
chhoti si problem ke liye design patterns, interfaces aur
abstractions ka pura architecture khada kar dena, jabki ek simple
function se hi kaam chal jata.

PROBLEM:
- Code samajhna mushkil ho jata hai (unnecessary complexity)
- Development time waste hota hai
- Maintenance overhead badh jata hai
- YAGNI Principle (You Aren't Gonna Need It) violate hota hai

SOLUTION APPROACH:
- Simple problems ke liye simple solutions rakho
- Pattern tabhi apply karo jab genuinely uski zaroorat ho
  (jaise future me multiple implementations chahiye ho)
- "Keep It Simple, Stupid" (KISS) principle follow karo
=====================================================================
*/

#include <iostream>
#include <memory>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: ek simple print ke liye Strategy Pattern
// ka poora setup laga diya, jabki ek hi tarike se print karna tha
// ---------------------------------------------------------------
class IPrinterStrategy
{
public:
    virtual void print() = 0;
    virtual ~IPrinterStrategy() = default;
};

class ConsolePrinterStrategy : public IPrinterStrategy
{
public:
    void print() override
    {
        cout << "Printing via Console Strategy..." << endl;
    }
};

class PrinterContext
{
    unique_ptr<IPrinterStrategy> strategy;

public:
    PrinterContext(unique_ptr<IPrinterStrategy> strat) : strategy(move(strat)) {}
    void execute()
    {
        strategy->print();
    }
    // Problem: itna saara boilerplate (interface + strategy class +
    // context class) sirf ek console print statement ke liye
};

// ---------------------------------------------------------------
// FIXED VERSION: ek simple function hi kaafi hai jab tak
// multiple printing strategies ki genuinely zaroorat na ho
// ---------------------------------------------------------------
class SimplePrinter
{
public:
    void print()
    {
        cout << "Printing directly, no need for extra abstraction..." << endl;
    }
};

int main()
{
    cout << "--- Gold Plating (Anti-Pattern) Demo ---" << endl;
    PrinterContext printerContext(make_unique<ConsolePrinterStrategy>());
    printerContext.execute();

    cout << "\n--- Fixed Version (Simple Direct Approach) Demo ---" << endl;
    SimplePrinter simplePrinter;
    simplePrinter.print();

    return 0;
}