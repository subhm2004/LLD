/**
 * COMPOSITION — strong "Has-a"; part CANNOT exist without whole
 * House owns Room objects (member subobjects)
 * Room lifetime tied to House
 * UML: filled diamond ◆ on House side
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

class Room {
    string name;
    // Room only created as part of House in this design
    friend class House;

    Room(string n) : name(n) {
        cout << "[Room] created inside House: " << name << "\n";
    }

public:
    void describe() const { cout << "  Room: " << name << "\n"; }
    ~Room() { cout << "[Room] destroyed: " << name << "\n"; }
};

class House {
    string address;
    Room livingRoom;
    Room bedroom;
    vector<unique_ptr<Room>> extraRooms;  // also composition via unique_ptr in ctor

public:
    House(string addr)
        : address(addr),
          livingRoom("Living"),
          bedroom("Bedroom") {
        extraRooms.push_back(unique_ptr<Room>(new Room("Kitchen")));  // House is friend
        cout << "[House] created at " << address << "\n";
    }

    ~House() {
        cout << "[House] destroyed at " << address << " (all rooms go with it)\n";
    }

    void listRooms() const {
        cout << "[House] rooms at " << address << ":\n";
        livingRoom.describe();
        bedroom.describe();
        for (const auto& r : extraRooms)
            r->describe();
    }
};

int main() {
    {
        House home("221B Baker Street");
        home.listRooms();
    }  // All rooms destroyed automatically with House

    cout << "House scope ended — no Room objects left\n";
    return 0;
}
