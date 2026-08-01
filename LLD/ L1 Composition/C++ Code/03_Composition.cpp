/**
 * ============================================================================
 *  03_Composition.cpp  —  COMPOSITION (object relationship #3, strong Has-A) ⭐
 * ----------------------------------------------------------------------------
 *  Composition = "PART bina WHOLE ke exist HI nahi kar sakta. Whole banta hai
 *                 to part bante hain, whole marta hai to part BHI mar jaate hain.
 *                 Sabse STRONG ownership."
 *
 *  Asli duniya ka example: House aur Rooms 🏠
 *    Rooms house ke ANDAR hi bante hain — house ke bina room ka koi wajood
 *    nahi. House giraa do (demolish) to saare rooms bhi khatam. Room ko nikaal
 *    ke kisi aur house me nahi laga sakte (Engine ki tarah reusable nahi).
 *    Yahi composition ko aggregation se alag karta hai — EXCLUSIVE ownership.
 *
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │  AGGREGATION vs COMPOSITION (dono "whole-part" — bada confusion):       │
 *  │                                                                          │
 *  │              | Aggregation (Car-Engine) | Composition (House-Room)      │
 *  │  ------------+--------------------------+---------------------------    │
 *  │  Part banta  | BAHAR (inject hota)      | ANDAR (whole khud banata)     │
 *  │  Ownership   | shared/external          | EXCLUSIVE (whole ka)          │
 *  │  Whole mare  | part ZINDA reh sakta     | part BHI mar jaata            │
 *  │  Reuse       | part dusre me use ho     | part dobara use nahi ho       │
 *  │  UML         | hollow diamond ◇         | FILLED diamond ◆              │
 *  │  Code        | Engine* (delete NAHI)    | member obj / unique_ptr       │
 *  └──────────────────────────────────────────────────────────────────────────┘
 *
 *  📌 COMPOSITION ki pehchaan (is file me — 2 styles):
 *  1. MEMBER SUB-OBJECTS: `Room livingRoom;` — House object ke andar hi baithe.
 *     House bante hi bante hain, House marte hi marte hain. Automatic lifetime.
 *  2. unique_ptr: `unique_ptr<Room>` — House EXCLUSIVELY own karti hai (unique_ptr
 *     copy nahi hota, ek hi malik). House marte hi unique_ptr auto-delete.
 *  + Room ka constructor PRIVATE hai (sirf House friend bana sakti) — matlab
 *    Room akele kahin nahi ban sakta. Ye composition ko CODE me ENFORCE karta!
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
    }  // <-- House scope KHATAM -> House destroy -> saare Rooms AUTOMATIC destroy
       //     (member rooms + unique_ptr rooms, sab House ke saath gaye)

    // >>> COMPOSITION KA PROOF <<<
    // Yahan koi Room object bacha hi nahi — sab House ke saath mar gaye.
    // Aggregation (Engine) me part bach jaata tha; yahan EXCLUSIVE ownership
    // ki wajah se part bhi khatam. "Part can't outlive the whole."
    cout << "House scope ended — no Room objects left\n";
    return 0;
}

/**
 * EXPECTED OUTPUT (dhyan do rooms House ke SAATH bante aur SAATH marte hain):
 *   [Room] created inside House: Living
 *   [Room] created inside House: Bedroom
 *   [Room] created inside House: Kitchen
 *   [House] created at 221B Baker Street
 *   [House] rooms at 221B Baker Street:
 *     Room: Living / Bedroom / Kitchen
 *   [House] destroyed at 221B Baker Street (all rooms go with it)
 *   [Room] destroyed: Kitchen / Bedroom / Living   <- sab House ke saath gaye!
 *   House scope ended — no Room objects left
 *
 *   ^ House destroy hote hi saare rooms destroy = composition. Aggregation
 *   (Engine) me part whole ke baad zinda tha; yahan nahi.
 */
