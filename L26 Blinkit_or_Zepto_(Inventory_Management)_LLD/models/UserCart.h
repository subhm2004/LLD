// ============================================================================
//  models/UserCart.h — Cart + User models
// ----------------------------------------------------------------------------
//  Cart = (Product, quantity) pairs ki list. addItem(sku, qty) me
//  ProductFactory se product banta (SKU se). Order place karte waqt yahi
//  cart nearby stores ke stock ke against validate + split hota hai.
//
//  User = naam + location (x, y coordinates) + apna Cart. Location zaroori
//  hai — isi se decide hota kaunse dark stores 5km ke andar hain (nearby
//  stores dhundhne ke liye distance calculate hota).
//
//  OWNERSHIP: User apne Cart ka malik (dtor me delete); Cart apne products
//  ka malik (dtor me delete). Clean ownership hierarchy.
// ============================================================================
#ifndef BLINKIT_LLD_MODELS_USERCART_H
#define BLINKIT_LLD_MODELS_USERCART_H

#include <bits/stdc++.h>

#include "Product.h"

using namespace std;

namespace blinkit_lld {

class Cart {
public:
    ~Cart() {
        for (auto &entry : items_) {
            delete entry.first;
        }
    }

    void addItem(int sku, int qty) {
        Product *product = ProductFactory::createProduct(sku);
        items_.push_back({product, qty});
    }

    const vector<pair<Product *, int>> &getItems() const { return items_; }

private:
    vector<pair<Product *, int>> items_;
};

class User {
public:
    User(string name, double x, double y) : name_(std::move(name)), x_(x), y_(y), cart_(new Cart()) {}
    ~User() { delete cart_; }

    const string &getName() const { return name_; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    Cart *getCart() const { return cart_; }

private:
    string name_;
    double x_;
    double y_;
    Cart *cart_;
};

} // namespace blinkit_lld

#endif // BLINKIT_LLD_MODELS_USERCART_H
