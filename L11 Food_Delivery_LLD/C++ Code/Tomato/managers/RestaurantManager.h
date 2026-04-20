// singleton banaya h restaurant manager ko Double locking ka use kiya hai
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
