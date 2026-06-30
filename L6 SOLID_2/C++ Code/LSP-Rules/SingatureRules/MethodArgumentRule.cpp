// ============================================================================
//  MethodArgumentRule.cpp  —  LSP Signature Rule: Method arguments (contravariance)
// ----------------------------------------------------------------------------
//  LSP rule: override ke parameter types SAME ya base type ke SUPERTYPE ho
//  sakte hain (CONTRAVARIANT) — yani override ko utne ya usse ZYADA general
//  inputs accept karne chahiye, kam nahi. Agar override base se zyada SPECIFIC
//  argument maange to woh substitute hone par kuch valid calls reject kar dega
//  -> LSP break. (C++ me directly contravariant params nahi hote — concept
//  samajhne ke liye yeh demo.)
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Method Argument Rule :
// Subtype method arguments can be identical or wider than the supertype
// C++ imposes this by keeping singature identical

class Parent
{
public:
    virtual void print(string msg)
    {
        cout << "Parent: " << msg << endl;
    }
};

class Child : public Parent
{
public:
    void print(string msg) override
    {
        cout << "Child: " << msg << endl;
    }
};

// Client that pass string as msg as client expects.
class Client
{
private:
    Parent *p;

public:
    Client(Parent *p)
    {
        this->p = p;
    }
    void printMsg()
    {
        p->print("Hello");
    }
};

int main()
{

    Parent *parent = new Parent();
    Parent *child = new Child();

    // Client* client = new Client(parent);
    Client *client = new Client(child);

    client->printMsg();

    return 0;
}
