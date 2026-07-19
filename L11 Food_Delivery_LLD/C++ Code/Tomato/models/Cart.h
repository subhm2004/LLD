// ============================================================================
//  Cart.h — User ka shopping cart (order banne se PEHLE ka state) 🛒
// ----------------------------------------------------------------------------
//  Ek restaurant + uske chune hue menu items ki list. Total khud jodta hai.
//  DESIGN RULE: item add karne se PEHLE restaurant set hona ZAROORI hai —
//  addItem() guard karta hai (bina restaurant ke item add nahi hoga), aur
//  isEmpty() bhi restaurant null hone ko empty maanta hai.
//  Ek Cart me ek hi restaurant ke items (do restaurants mix nahi hote).
//  Payment success ke baad clear() se cart khaali (main.cpp me hota hai).
// ============================================================================
#ifndef CART_H
#define CART_H

#include <iostream>
#include <vector>
#include <string>
#include "../models/MenuItem.h"
#include "../models/Restaurant.h"

using namespace std;

class Cart {
private:
    Restaurant* restaurant;
    vector<MenuItem> items;

public:
    Cart() {
        restaurant = nullptr;
    }

    void addItem(const MenuItem& item) {
        if (!restaurant) {
            cerr << "Cart: Set a restaurant before adding items." << endl;
            return;
        }
        items.push_back(item);
    }

    double getTotalCost() const {
        double sum = 0;
        for (const auto& it : items) {
            sum += it.getPrice();
        }
        return sum;
    }

    bool isEmpty() {
        return (!restaurant || items.empty());
    }

    void clear() {
        items.clear();
        restaurant = nullptr;
    }

    // Getters and Setters
    void setRestaurant(Restaurant* r) {
        restaurant = r;
    }

    Restaurant* getRestaurant() const {
        return restaurant;
    }

    const vector<MenuItem>& getItems() const {
        return items;
    }
};

#endif // CART_H
