// ============================================================================
//  main.cpp  —  TOMATO food-delivery app ka demo driver (L11 LLD project)
// ----------------------------------------------------------------------------
//  "Tomato" = Zomato/Swiggy ka mini version! End-to-end happy flow:
//
//   User banao -> searchRestaurants("Delhi") -> restaurant select ->
//   addToCart(P1, P2) -> checkoutNow (Delivery + UPI strategy) ->
//   payForOrder -> payment success -> notification + cart clear
//
//  DHYAN DO: client (ye file) sirf TOMATOAPP (Facade) se baat karta hai —
//  RestaurantManager, OrderManager, factories, notification... sab andar
//  chhupa hai. Ek bhi manager/factory ka naam yahan nahi dikhta (UPI
//  strategy inject karna client ka decision hai isliye wo dikhti hai).
//
//  IS PROJECT ME 4 PATTERNS: Facade (TomatoApp) + Singleton (managers) +
//  Strategy (payment) + Factory Method (now/scheduled orders).
//  Detail: ../design_patterns_used.md padho!
// ============================================================================
#include <iostream>
#include "TomatoApp.h"
using namespace std;

int main()
{
    // Create TomatoApp Object
    TomatoApp *tomato = new TomatoApp();

    // Simulate a user coming in (Happy Flow)
    User *user = new User(101, "Shubham", "Delhi");
    cout << "User: " << user->getName() << " is active." << endl;

    // User searches for restaurants by location
    vector<Restaurant *> restaurantList = tomato->searchRestaurants("Delhi");

    if (restaurantList.empty())
    {
        cout << "No restaurants found!" << endl;
        return 0;
    }
    cout << "Found Restaurants:" << endl;
    for (auto restaurant : restaurantList)
    {
        cout << " - " << restaurant->getName() << endl;
    }

    // User selects a restaurant
    tomato->selectRestaurant(user, restaurantList[0]);

    cout << "Selected restaurant: " << restaurantList[0]->getName() << endl;

    // User adds items to the cart
    tomato->addToCart(user, "P1");
    tomato->addToCart(user, "P2");

    tomato->printUserCart(user);

    // User checkout the cart
    Order *order = tomato->checkoutNow(user, "Delivery", new UpiPaymentStrategy("1234567890"));

    // User pay for the cart. If payment is success, notification is sent.
    tomato->payForOrder(user, order);

    // Cleanup Code.
    delete tomato;
    delete user;

    return 0;
}
