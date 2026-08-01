/*
=====================================================================
GOD OBJECT ANTI-PATTERN
=====================================================================
THEORY:
Ye anti-pattern tab hota hai jab ek hi class ko itni saari
responsibilities de di jaati hain ki wo class "sab kuch janti hai,
sab kuch karti hai" - bilkul ek "God" jaisa sab kaam khud hi
karne lagta hai.

Isse Single Responsibility Principle (SOLID ka "S") violate hota
hai. Ye principle kehta hai: "Ek class ka sirf ek hi reason hona
chahiye change hone ka."

PROBLEM:
- Class bahut bada aur complex ho jata hai
- Testing mushkil ho jati hai kyunki sab kuch aapas me juda hota hai
- Ek chhota sa change karne pe pura system tootne ka risk badh jata hai
- Team collaboration mushkil hota hai, sab ek hi class me kaam karenge

SOLUTION APPROACH:
Har responsibility ko alag-alag class me todo:
PaymentProcessor, InvoiceGenerator, EmailService, DatabaseManager,
AuthenticationService - isse har class chhoti, focused aur
independently testable ban jaati hai.
=====================================================================
*/

#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: sab kuch ek hi class me daal diya
// ---------------------------------------------------------------
class GodObjectManager
{
public:
    void processPayment(double amount)
    {
        cout << "Processing payment of Rs. " << amount << endl;
    }

    void generateInvoice(string customerName)
    {
        cout << "Invoice generated for " << customerName << endl;
    }

    void sendEmail(string toAddress)
    {
        cout << "Email sent to " << toAddress << endl;
    }

    void saveToDatabase(string record)
    {
        cout << "Saved to DB: " << record << endl;
    }

    void authenticateUser(string username, string password)
    {
        // Yahan real app me hashing/DB check hota, abhi demo ke liye
        // simple check kar rahe hain
        if (username == "admin" && password == "1234")
        {
            cout << "User authenticated successfully" << endl;
        }
        else
        {
            cout << "Authentication failed" << endl;
        }
    }
    // Problem: agar payment logic change karna ho to isi bade class
    // ko touch karna padega jisme email, db, auth sab bhi hai -
    // risk of breaking unrelated features
};

// ---------------------------------------------------------------
// FIXED VERSION: responsibilities ko alag classes me split kiya
// ---------------------------------------------------------------
class PaymentProcessor
{
public:
    void processPayment(double amount)
    {
        cout << "[PaymentProcessor] Payment of Rs. " << amount << " processed" << endl;
    }
};

class InvoiceGenerator
{
public:
    void generateInvoice(string customerName)
    {
        cout << "[InvoiceGenerator] Invoice created for " << customerName << endl;
    }
};

class EmailService
{
public:
    void sendEmail(string toAddress)
    {
        cout << "[EmailService] Email sent to " << toAddress << endl;
    }
};

class DatabaseManager
{
public:
    void saveToDatabase(string record)
    {
        cout << "[DatabaseManager] Saved: " << record << endl;
    }
};

class AuthenticationService
{
public:
    bool authenticateUser(string username, string password)
    {
        return (username == "admin" && password == "1234");
    }
};

int main()
{
    cout << "--- God Object (Anti-Pattern) Demo ---" << endl;
    GodObjectManager manager;
    manager.processPayment(500.0);
    manager.generateInvoice("Rahul");
    manager.sendEmail("rahul@example.com");
    manager.saveToDatabase("Order#123");
    manager.authenticateUser("admin", "1234");

    cout << "\n--- Fixed Version (Separate Classes) Demo ---" << endl;
    PaymentProcessor payment;
    InvoiceGenerator invoice;
    EmailService email;
    DatabaseManager db;
    AuthenticationService auth;

    payment.processPayment(500.0);
    invoice.generateInvoice("Rahul");
    email.sendEmail("rahul@example.com");
    db.saveToDatabase("Order#123");

    if (auth.authenticateUser("admin", "1234"))
    {
        cout << "[AuthenticationService] Login successful" << endl;
    }
    else
    {
        cout << "[AuthenticationService] Login failed" << endl;
    }

    return 0;
}