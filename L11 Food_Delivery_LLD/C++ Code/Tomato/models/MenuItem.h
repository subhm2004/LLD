// ============================================================================
//  MenuItem.h — Menu ka ek item: code (P1/P2...), naam, price
// ----------------------------------------------------------------------------
//  Sabse basic data holder. `code` se item identify hota hai (addToCart
//  isi code se menu me item dhundta hai). Cart aur Order inhi items ki
//  COPIES rakhte hain (pointers nahi — value type, simple ownership).
// ============================================================================
#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
using namespace std;

class MenuItem {
private:
    string code;
    string name;
    int price;

public:
    MenuItem(const string& code, const string& name, int price) {
        this->code = code;
        this->name = name;
        this->price = price;
    }

    //Getters and setters
    string getCode() const {
        return code;
    }

    void setCode(const string &c) {
        code = c;
    }

    string getName() const {
        return name;
    }

    void setName(const string &n) {
        name = n;
    }

    int getPrice() const {
        return price;
    }

    void setPrice(int p) {
        price = p;
    }
};

#endif // MENUITEM_H
