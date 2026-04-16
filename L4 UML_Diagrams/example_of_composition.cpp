#include <bits/stdc++.h>
#include <memory>
using namespace std;

// Individual components (parts of Chair)

class Seat
{
public:
    void info()
    {
        cout << "Seat is comfortable\n";
    }
};

class Arms
{
public:
    void info()
    {
        cout << "Arms for support\n";
    }
};

class Wheels
{
public:
    void info()
    {
        cout << "Wheels for movement\n";
    }
};

class Cover
{
public:
    void info()
    {
        cout << "Cover for protection\n";
    }
};

// Chair HAS-A relationship (Composition)
class Chair
{
private:
    unique_ptr<Seat> seat;
    unique_ptr<Arms> arms;
    unique_ptr<Wheels> wheels;
    unique_ptr<Cover> cover;

public:
    // Constructor → all parts created WITH Chair
    Chair()
    {
        seat = make_unique<Seat>();
        arms = make_unique<Arms>();
        wheels = make_unique<Wheels>();
        cover = make_unique<Cover>();
    }
    void info()
    {
        cout << "This is a chair is made by Shubham Malik :" << endl;
    }

    void showChairDetails()
    {
        seat->info();
        arms->info();
        wheels->info();
        cover->info();
    }

    Wheels *getWheels() // ese bhi kr skte hai but isse encapsulation kharab hoti hai
    {
        return wheels.get();
    }
};

int main()
{
    auto chair = make_unique<Chair>();

    chair->info();
    chair->showChairDetails();

    return 0;
}