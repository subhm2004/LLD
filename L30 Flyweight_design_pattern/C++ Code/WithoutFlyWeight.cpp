// ============================================================================
//  WithoutFlyWeight.cpp  —  Flyweight ke BINA (problem dikhane ke liye)
// ----------------------------------------------------------------------------
//  Yahan har Asteroid object apni saari fields (color, texture, material, size
//  + position + velocity) khud rakhta hai. 1,000,000 asteroids matlab heavy
//  shared data 1,000,000 baar duplicate -> bahut zyada memory. Sirf 3 types
//  hain phir bhi har object full copy. Iska fix WithFlyWeight.cpp (shared
//  intrinsic state). Dono ka memory output compare karke farq dekho.
//
//  ASLI PROBLEM ek line me:
//    Game me asteroid ke sirf 3 TYPE hain (Red/Rocky/Iron, Blue/Metallic/Stone,
//    Gray/Icy/Ice). Har type ka color+texture+material+size BILKUL same hai.
//    Phir bhi hum wo same data 1,000,000 baar RAM me duplicate kar rahe hain.
//    Yaani ~10 lakh me se sirf 3 unique copies chahiye thi — baaki sab waste.
//
//  Data ke do type (ye samajh liya to Flyweight aa gaya):
//    INTRINSIC (shared ho sakta hai) -> color, texture, material, length, width,
//                                       weight. Ye asteroid ke TYPE pe depend
//                                       karta hai, individual asteroid pe nahi.
//    EXTRINSIC (har object ka apna)  -> posX, posY, velocityX, velocityY.
//                                       Ye har asteroid ka alag hai, share nahi
//                                       ho sakta.
//    Is file me dono ek hi class me ghuse hue hain — yahi galti hai.
// ============================================================================
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <random>
#include <memory>
#include <chrono>
using namespace std;

// ----------------------------------------------------------------------------
//  Asteroid — "fat" object. Intrinsic + Extrinsic dono yahi rakhta hai.
//  Har `new Asteroid(...)` pe 3 string objects bante hain aur unka heap data
//  bhi copy hota hai. 10 lakh baar. Yahi memory blow-up ki jad hai.
// ----------------------------------------------------------------------------
class Asteroid {
private:
    // Intrinsic properties (same for many asteroids) - DUPLICATED FOR EACH OBJECT
    // Sirf 3 unique combinations hain in 6 fields ke, phir bhi har object
    // apni personal copy rakh raha hai. 999,997 copies bekaar hain.
    int length;
    int width;
    int weight;
    string color;    // "Red" / "Blue" / "Gray"     -> sirf 3 unique values
    string texture;  // "Rocky" / "Metallic" / "Icy" -> sirf 3 unique values
    string material; // "Iron" / "Stone" / "Ice"     -> sirf 3 unique values

    // Extrinsic properties (unique for each asteroid)
    // YE waali fields duplicate nahi hain — har asteroid ki position/velocity
    // genuinely alag hai. Inko share karna possible hi nahi.
    int posX, posY;
    int velocityX, velocityY;

public:
    // Constructor 10 parameters leta hai: 6 intrinsic + 4 extrinsic.
    // Note: strings by VALUE aa rahi hain (const& nahi) — matlab har call pe
    // ek extra copy banti hai. 10 lakh calls = 30 lakh string copies.
    Asteroid(int l, int w, int wt, string col, string tex,
        string mat, int posX, int posY, int velX, int velY) {
            this->length = l;
            this->width = w;
            this->weight = w;   // BUG: yaha `wt` hona chahiye tha, `w` nahi.
                                // Abhi weight me width ki value ja rahi hai
                                // (25/35/45), jabki caller ne 250/350/450 bheja tha.
                                // Render output me weight print nahi hota isliye
                                // pakda nahi gaya. WithFlyWeight.cpp me bhi same typo hai.
            this->color = col;
            this->texture = tex;
            this->material = mat;
            this->posX = posX;
            this->posY = posY;
            this->velocityX = velX;
            this->velocityY = velY;
    }

    // Dhyaan do: render() ko koi parameter nahi chahiye, kyunki position/velocity
    // object ke ANDAR hi padi hai. WithFlyWeight.cpp me yahi signature badal
    // jayega — wahan position bahar se pass karni padegi. Yahi pattern ka core diff hai.
    void render() {
        cout << "Rendering " << color <<", " << texture << ", " << material
            <<" asteroid at (" << posX << "," << posY
            << ") Size: " << length << "x" << width
            << " Velocity: (" << velocityX << ", "
            << velocityY << ")" << endl;
    }

    // Calculate approximate memory usage per object
    // Rough estimate hai (exact sizeof nahi) — sirf comparison ke liye.
    // Real me `new` ka heap overhead + padding + vector ka pointer alag se lagta hai,
    // to actual RAM iss number se bhi zyada hi jayegi.
    //
    // Actual total = 196 bytes/object (macOS/libc++ pe sizeof(string) == 24):
    //     28  -> 7 ints
    //     72  -> 3 string objects (24 each)
    //     96  -> unka heap data (approx)
    // Inme se 168 bytes (72 + 96) PURA intrinsic hai — yahi Flyweight bachayega.
    // Bachega sirf 28 bytes ka extrinsic hissa.
    static size_t getMemoryUsage() {
        return sizeof(int) * 7 +                // length, width, weight, x, y, velocityX, velocityY
               sizeof(string) * 3 +             // color, texture, material string objects
               32 * 3;                          // Approximate string data (assuming average 10 chars each)
    }
};

// ----------------------------------------------------------------------------
//  SpaceGame — client. Bas asteroids ka vector bhar deta hai.
// ----------------------------------------------------------------------------
class SpaceGame {
private:
    vector<Asteroid*> asteroids; // 10 lakh alag-alag heap objects

public:
    void spawnAsteroids(int count) {
        cout << "\n=== Spawning " << count << " asteroids ===" << endl;

        // Sirf 3 hi variety hai — yahi baat is poore demo ka point hai.
        vector<string> colors = {"Red", "Blue", "Gray"};
        vector<string> textures = {"Rocky", "Metallic", "Icy"};
        vector<string> materials = {"Iron", "Stone", "Ice"};
        int sizes[] = {25, 35, 45};

        for (int i = 0; i < count; i++) {
            int type = i % 3; // 0,1,2,0,1,2... -> round-robin se teeno type banenge

            // Har iteration me ek POORA naya object — intrinsic data samet.
            // i=0 aur i=3 dono "Red/Rocky/Iron" hain, par phir bhi do alag
            // objects ban rahe hain jinme wahi 3 strings dubara store ho rahi hain.
            asteroids.push_back(new Asteroid(
                sizes[type], sizes[type], sizes[type] * 10,   // <- intrinsic (repeat hota hai)
                colors[type], textures[type], materials[type], // <- intrinsic (repeat hota hai)
                100 + i * 50,          // Simple x: 100, 150, 200, 250...   <- extrinsic (unique)
                200 + i * 30,          // Simple y: 200, 230, 260, 290...   <- extrinsic (unique)
                1,                     // All move right with velocity 1     <- extrinsic
                2                      // All move down with velocity 2      <- extrinsic
            ));
        }

        cout << "Created " << asteroids.size() << " asteroid objects" << endl;
        // ^ 10,00,000 objects. WithFlyWeight me ye line "contexts" bolegi aur
        //   saath me "Total flyweight objects: 3" bhi print hoga.
    }

    void renderAll() {
        cout << "\n--- Rendering first 5 asteroids ---" << endl;
        // Sirf 5 print kar rahe hain — 10 lakh lines terminal me kaun dekhega.
        for (int i = 0; i < min(5, (int)asteroids.size()); i++) {
            asteroids[i]->render();
        }
    }

    // Total = per-object size × object count. Bilkul linear growth.
    // Koi sharing nahi, isliye count double karo to memory bhi double.
    size_t calculateMemoryUsage() {
        return asteroids.size() * Asteroid::getMemoryUsage();
    }

    int getAsteroidCount() {
        return asteroids.size();
    }
};

int main() {
    const int ASTEROID_COUNT = 1000000;

    cout << "\n TESTING WITHOUT FLYWEIGHT PATTERN" << endl;
    SpaceGame* game = new SpaceGame();

    game->spawnAsteroids(ASTEROID_COUNT);

    // Show first 5 asteroids to see the pattern
    game->renderAll();

    // Calculate and display memory usage
    size_t totalMemory = game->calculateMemoryUsage();

    // Ye output note kar lo, phir WithFlyWeight.cpp chala ke compare karo:
    //     WITHOUT : 196 bytes/asteroid -> 186.92 MB
    //     WITH    :  24 bytes/asteroid ->  22.89 MB   (~8x kam)
    cout << "\n=== MEMORY USAGE ===" << endl;
    cout << "Total asteroids: " << ASTEROID_COUNT << endl;
    cout << "Memory per asteroid: " << Asteroid::getMemoryUsage() << " bytes" << endl;
    cout << "Total memory used: " << totalMemory << " bytes" << endl;
    cout << "Memory in MB: " << totalMemory / (1024.0 * 1024.0) << " MB" << endl;

    // NOTE: `game` aur uske andar ke 10 lakh `new Asteroid` kabhi delete nahi hue
    // -> memory leak. Demo hai isliye chal gaya (process exit pe OS reclaim kar lega),
    // par production me destructor / smart pointers chahiye.

    return 0;
}
