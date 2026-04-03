#include <bits/stdc++.h>
#include <memory> // std::unique_ptr aur make_unique ke liye
using namespace std;

class A
{
public:
    void method1()
    {
        cout << "Method1 of Class A called" << endl;
    }
};

class B
{
private:
    unique_ptr<A> a; // smart pointer jo A ka ownership handle karta hai

public:
    // Constructor me 'a' ko initialize kar rahe hain make_unique ke through
    // ---------------------------------------------------------
    // Pehle agar hum a = new A(); likhte to:

    // 1️⃣ Memory manually allocate hoti, aur hume delete bhi khud karna padta.
    //    Agar delete bhool gaye to memory leak ho jata.
    // 2️⃣ Agar constructor me ya kahin exception aata, to 'new' se allocate memory
    //    kabhi delete nahi hoti → memory leak.
    //
    // make_unique<A>() ka fayda:

    // ✅ Automatic memory management: unique_ptr automatically delete kar deta hai jab B destroy hota hai.
    // ✅ Exception safe: agar exception aaye bhi, memory safe rehti hai.
    // ✅ Modern C++ best practice: ownership clearly dikhata hai, safer hai aur cleaner code hota.

    B()
    {
        a = make_unique<A>();
    }

    void method2()
    {
        cout << "Method2 of Class B called" << endl;
    }

    // .get() c++ internal function hai jo unique_ptr se raw pointer return karta hai.

    // ye safe hai kyoki expose kar rahe hain A ka pointer, lekin ownership B ke paas hi rahegi aur memory management B ke through hi hoga.
    A *getA()
    {
        return a.get(); // unique_ptr se raw pointer return kar rahe hain
    }

    // B ke andar se A ka method call karne ke liye ek helper method
    void call_A_method()
    {
        a->method1();
    }
};

int main()
{
    // B ka instance create kar rahe hain make_unique se
    // ------------------------------------------------
    // new B() avoid kiya kyunki manual delete ka risk hota.
    // unique_ptr automatically cleanup kar dega jab scope se bahar jaayega.
    unique_ptr<B> b = make_unique<B>();

    b->method2();         // B ka method call
    b->getA()->method1(); // B ke andar se A ka method call
    b->call_A_method();   // B ke andar se A ka method call karne ka ek aur tarika

    // Delete ki zarurat nahi: unique_ptr automatically cleanup karega
    return 0;
}

/*
Summary in Hinglish:

1️⃣ Humne 'new' use nahi kiya kyunki manual memory allocation risky hoti hai.
   Agar 'delete' bhool gaye to memory leak ho jaata.

2️⃣ make_unique<T>() heap pe object create karta hai aur unique_ptr uska owner ban jaata hai.
   Jab object scope se bahar jaata hai, memory automatically delete ho jaati.

3️⃣ unique_ptr clearly ownership dikhata hai: B owns A. Jab B destroy hota hai, A bhi destroy ho jaata.

4️⃣ Modern C++ me best practice: raw pointers avoid karo for owning memory.
   Smart pointers (unique_ptr, shared_ptr) + make_unique use karo.

In short:
- 'new' = manual aur risky
- 'make_unique' = automatic, safe, modern C++ way
*/

/*
 Classic (Old) Style                     | Modern C++ Style (modern me delete apne aap ho jata h mantually krna nhi padta)                                                                |
---------------------------------------- | ------------------------------------------------------------------------------------- |
1.                  A*                   |  unique_ptr<A>

2.               A* a = new A();        | unique_ptr<A> a = make_unique<A>()
                                                        OR
                                        |  auto a = make_unique<A>()
                                                        OR
                                        | unique_ptr<A> a = unique_ptr<A>(new A());

*/
