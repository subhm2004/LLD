/*
=====================================================================
DRY (DO NOT REPEAT YOURSELF) VIOLATION
=====================================================================
THEORY:
DRY principle kehta hai: "Har piece of knowledge ka system me sirf
ek hi, unambiguous, authoritative representation hona chahiye."

Jab hum same logic ko different jagah copy-paste karte hain, to
DRY violate hota hai.

PROBLEM:
- Ek jagah bug fix karo, dusri jagah bhool jate ho (inconsistency)
- Code duplicate hone se maintainability kam ho jati hai
- Same logic change karni ho to har jagah dhundh ke change karna
  padta hai

SOLUTION APPROACH:
- Common logic ko ek shared/helper function me nikaal do
- Ya ek common base class/utility class bana do jisse dono
  functions inherit/use karein
=====================================================================
*/

#include <iostream>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: same "log" logic dono function me
// copy-paste ki hui hai
// ---------------------------------------------------------------
class TaxCalculator
{
public:
    double calculateStateTax(double amount)
    {
        double tax = amount * 0.05;
        // Logic copy-paste kiya hua hai
        cout << "Tax generated successfully" << endl;
        return tax;
    }

    double calculateCentralTax(double amount)
    {
        double tax = amount * 0.10;
        // Logic copy-paste kiya hua hai (same print statement repeat)
        cout << "Tax generated successfully" << endl;
        return tax;
    }
};

// ---------------------------------------------------------------
// FIXED VERSION: common logic ko ek helper function me nikala
// ---------------------------------------------------------------
class TaxCalculatorFixed
{
private:
    // Ye ek common helper function hai jo dono jagah use hoga
    void logTaxGenerated()
    {
        cout << "Tax generated successfully" << endl;
    }

    // Ye ek common calculation function hai jo rate ke basis pe
    // tax nikalta hai - taaki formula bhi repeat na ho
    double calculateTax(double amount, double rate)
    {
        double tax = amount * rate;
        logTaxGenerated();
        return tax;
    }

public:
    double calculateStateTax(double amount)
    {
        return calculateTax(amount, 0.05);
    }

    double calculateCentralTax(double amount)
    {
        return calculateTax(amount, 0.10);
    }
};

int main()
{
    cout << "--- DRY Violation (Anti-Pattern) Demo ---" << endl;
    TaxCalculator calc;
    cout << "State Tax: " << calc.calculateStateTax(1000) << endl;
    cout << "Central Tax: " << calc.calculateCentralTax(1000) << endl;

    cout << "\n--- Fixed Version (DRY Compliant) Demo ---" << endl;
    TaxCalculatorFixed calcFixed;
    cout << "State Tax: " << calcFixed.calculateStateTax(1000) << endl;
    cout << "Central Tax: " << calcFixed.calculateCentralTax(1000) << endl;

    return 0;
}