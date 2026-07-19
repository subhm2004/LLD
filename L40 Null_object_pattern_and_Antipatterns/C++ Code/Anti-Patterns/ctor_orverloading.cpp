/*
=====================================================================
CONSTRUCTOR OVERLOADING / TELESCOPING ANTI-PATTERN
=====================================================================
THEORY:
Jab ek class me bahut saare constructors bana diye jate hain - har
ek me alag number of parameters - to ise "Telescoping Constructor
Anti-Pattern" kehte hain. Jaise-jaise fields badhti hain,
constructors ki list bhi badhti jaati hai.

PROBLEM:
- Confusing hota hai ki kaunsa constructor kab use karna hai
- Naye fields add karne pe naya constructor banana padta hai
- Optional parameters handle karna mushkil ho jata hai
- Code readability kharab ho jati hai

SOLUTION APPROACH:
- Builder Pattern use karo (especially jab bahut saari optional fields hon)
- Default parameters use karo (jahan language support kare)
=====================================================================
*/

#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: telescoping constructors
// ---------------------------------------------------------------
class User
{
private:
    string name;
    int age;
    string email;
    string city;

public:
    User(string n) : name(n), age(0), email(""), city("") {}
    User(string n, int a) : name(n), age(a), email(""), city("") {}
    User(string n, int a, string e) : name(n), age(a), email(e), city("") {}
    User(string n, int a, string e, string c) : name(n), age(a), email(e), city(c) {}
    // Problem: agar aage aur fields add karni hon (phone, address etc)
    // to constructors ki list aur lambi hoti jayegi

    void printDetails()
    {
        cout << "Name: " << name << ", Age: " << age
             << ", Email: " << email << ", City: " << city << endl;
    }
};

// ---------------------------------------------------------------
// FIXED VERSION: Builder Pattern use karke readable object creation
// ---------------------------------------------------------------
class UserFixed
{
private:
    string name;
    int age;
    string email;
    string city;

    // Constructor ko private rakha, sirf Builder ke through hi
    // object banega
    UserFixed(string n, int a, string e, string c)
        : name(n), age(a), email(e), city(c) {}

public:
    void printDetails()
    {
        cout << "Name: " << name << ", Age: " << age
             << ", Email: " << email << ", City: " << city << endl;
    }

    // Builder class jo step by step fields set karti hai
    class Builder
    {
        string name = "";
        int age = 0;
        string email = "";
        string city = "";

    public:
        Builder &setName(string n)
        {
            name = n;
            return *this;
        }
        Builder &setAge(int a)
        {
            age = a;
            return *this;
        }
        Builder &setEmail(string e)
        {
            email = e;
            return *this;
        }
        Builder &setCity(string c)
        {
            city = c;
            return *this;
        }

        UserFixed build()
        {
            return UserFixed(name, age, email, city);
        }
    };
};

int main()
{
    cout << "--- Constructor Overloading (Anti-Pattern) Demo ---" << endl;
    User u1("Shubham");
    User u2("Shubham", 22);
    User u3("Shubham", 22, "shubham@example.com");
    u3.printDetails();

    cout << "\n--- Fixed Version (Builder Pattern) Demo ---" << endl;
    UserFixed userFixed = UserFixed::Builder()
                              .setName("Shubham")
                              .setAge(22)
                              .setEmail("shubham@example.com")
                              .setCity("Rohtak")
                              .build();
    userFixed.printDetails();

    return 0;
}