/**
 * ============================================================================
 *  03_Composition.cpp  —  COMPOSITION (strong Has-A, exclusive ownership)
 * ----------------------------------------------------------------------------
 *  Composition = "part" bina "whole" ke exist hi nahi karta. Whole banta hai
 *  to part bante hain, whole marta hai to part bhi mar jaate hain. Sabse
 *  strong ownership. Yahan House apne Rooms ko khud banati/destroy karti hai.
 *  Do styles dikhaye: (1) member sub-objects, (2) unique_ptr ctor me owned.
 *  UML: filled diamond ◆ House ki taraf.
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

class Room {
    string name;
    // Room ka constructor private hai — sirf House (friend) ise bana sakti hai.
    // Matlab Room akele kahin bhi nahi banaya ja sakta (composition enforce).
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
    Room livingRoom; // member sub-object -> House ke saath banega/marega
    Room bedroom;    // member sub-object
    // unique_ptr se bhi composition: House exclusively own karti hai inhe.
    vector<unique_ptr<Room>> extraRooms;

public:
    // Member rooms initializer list me bante hain (House bante hi).
    House(string addr)
        : address(addr),
          livingRoom("Living"),
          bedroom("Bedroom") {
        // House friend hai isliye yahan naya Room bana sakti hai.
        extraRooms.push_back(unique_ptr<Room>(new Room("Kitchen")));
        cout << "[House] created at " << address << "\n";
    }

    // House destroy hote hi saare rooms (member + unique_ptr) automatic destroy.
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
    }  // <-- House yahan destroy -> saare Rooms bhi automatic destroy (dekho output)

    cout << "House scope ended — no Room objects left\n";
    return 0;
}
