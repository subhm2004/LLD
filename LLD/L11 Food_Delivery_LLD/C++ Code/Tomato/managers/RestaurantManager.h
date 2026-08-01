// ============================================================================
//  RestaurantManager.h — SINGLETON (DCL): saare restaurants ka central registry
// ----------------------------------------------------------------------------
//  Poore app me restaurant catalog EK hona chahiye — do catalogs hote to
//  ek me restaurant register, dusre me search = "not found" chaos! Isliye
//  SINGLETON: getInstance() ekmatra instance deta hai (add + searchByLocation).
//
//  Ye DOUBLE-CHECKED LOCKING (DCL) style hai — L10 me detail padha:
//    Check #1 (bina lock, fast) -> Check #2 (lock ke andar, race rokta hai)
//  File me neeche ek commented block bhi hai jo MEYERS style dikhata hai
//  (static local) — wahi modern/simple choice hai. Dono valid, DCL classic.
//
//  searchByLocation ka detail: dono locations LOWERCASE karke compare karta
//  hai (transform + ::tolower) — "Delhi" == "delhi" == "DELHI" sab match!
//  Mutex se add/search operations thread-safe hain.
// ============================================================================
#ifndef RESTAURANT_MANAGER_H //
#define RESTAURANT_MANAGER_H

#include <vector>
#include <string>
#include <algorithm>
#include "../models/Restaurant.h"
#include <mutex>
using namespace std;

class RestaurantManager
{
private:
    vector<Restaurant *> restaurants;
    static RestaurantManager *instance;
    static mutex mtx;

    RestaurantManager() {}

public:
    static RestaurantManager *getInstance()
    {
        if (instance == nullptr)
        { // First check (no lock)
            lock_guard<mutex> lock(mtx);
            if (instance == nullptr)
            { // Second check (with lock)
                instance = new RestaurantManager();
            }
        }
        return instance;
    }

    /*
    YE PREFER KRTE HAI LEKIN MNE TO UPAR DCL DOUBLE LOCKING KI HAI THREADS KI
    static RestaurantManager* getInstance() {
    static RestaurantManager instance;
    return &instance;
    }

    */

    void addRestaurant(Restaurant *r)
    {
        lock_guard<mutex> lock(mtx);
        restaurants.push_back(r);
    }

    vector<Restaurant *> searchByLocation(string loc)
    {
        lock_guard<mutex> lock(mtx);

        vector<Restaurant *> result;
        // ye line poori string ko lowercase me convert kar rahi hai.
        /*

        iska matlab hai :-

        loc.begin(), loc.end() → poori string loc ke characters
        loc.begin() → result wahi same string me store hoga (in-place)
        ::tolower → har character ko lowercase me convert karo

        */
        transform(loc.begin(), loc.end(), loc.begin(), ::tolower);

        for (auto r : restaurants)
        {
            string rl = r->getLocation();
            transform(rl.begin(), rl.end(), rl.begin(), ::tolower);
            if (rl == loc)
            {
                result.push_back(r);
            }
        }
        return result;
    }
};

// Static initialization
RestaurantManager *RestaurantManager::instance = nullptr;
mutex RestaurantManager::mtx;

#endif // RESTAURANT_MANAGER_H
