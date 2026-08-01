// ============================================================================
//  WithoutFlyWeight.cpp  —  FLYWEIGHT ke BINA (problem samajhne ke liye)
// ----------------------------------------------------------------------------
//  Ye file jaan-bujh kar MEHNGA (memory-blowing) tareeka dikhati hai, taaki
//  WithFlyWeight.cpp ka fix samajh aaye. Pehle problem feel karo!
//
//  Asli duniya ka example: space game me 10,00,000 (1 million) asteroids:
//    - Asteroid ke sirf 3 TYPE hain: Red/Rocky/Iron, Blue/Metallic/Stone,
//      Gray/Icy/Ice — har type ka color+texture+material+size BILKUL same
//    - Phir bhi HAR asteroid apni personal copy rakh raha hai — wahi 3
//      strings RAM me 10 lakh baar duplicate! 999,997 copies pure waste.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  PROBLEM KA MATH — memory ka blast ek nazar me:                         │
//  │                                                                          │
//  │    Ek Asteroid object = ~196 bytes:                                     │
//  │       28 bytes -> 7 ints (size/weight/position/velocity)                │
//  │       72 bytes -> 3 string OBJECTS (sizeof(string)=24 each)             │
//  │       96 bytes -> strings ka heap data (approx)                         │
//  │                                                                          │
//  │    Total = 196 bytes × 10,00,000 objects = ~186.92 MB 💥                │
//  │                                                                          │
//  │    Jabki unique data kitna tha? Sirf 3 types × ~180 bytes = 540 bytes!  │
//  │    Matlab ~186 MB me se ~164 MB sirf DUPLICATE copies hain. 😵          │
//  │                                                                          │
//  │  FIX (WithFlyWeight.cpp): shared data EK baar rakho, pointer se share   │
//  │  karo -> 24 bytes/asteroid -> ~22.89 MB (~8x kam!)                      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ============================================================================
//   ⭐ INTRINSIC vs EXTRINSIC — ye table samajh liya to Flyweight aa gaya!
//   (Sawaal khud se poochho: "Do alag asteroids ke liye ye value SAME ho
//    sakti hai kya, bina kuch toote?" Haan -> intrinsic. Nahi -> extrinsic.)
//  ----------------------------------------------------------------------------
//   Field                | Type      | Kyun                | Flyweight me jayega?
//   ---------------------+-----------+---------------------+---------------------
//   color                | INTRINSIC | type pe depend hai,  | SHARED (ek baar
//   texture              | (shared   |  individual asteroid |  store, pointer
//   material             |  ho sakta |  pe nahi — "Red/     |  se share) ✅
//   length/width/weight  |  hai)     |  Rocky/Iron" sabka   |
//                        |           |  same hai            |
//   ---------------------+-----------+---------------------+---------------------
//   posX, posY           | EXTRINSIC | HAR asteroid ki      | NAHI — ye har
//   velocityX, velocityY | (har ek   |  position genuinely  |  object apna
//                        |  ka apna) |  alag hai            |  rakhega (context)
//
//   📌 IS FILE KI GALTI: dono type ka data EK HI class me ghusa hua hai —
//   isliye intrinsic data bhi har object ke saath duplicate ho raha hai.
//   Flyweight inko ALAG karta hai — wahi pura pattern hai.
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
class Asteroid
{
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
             string mat, int posX, int posY, int velX, int velY)
    {
        this->length = l;
        this->width = w;
        this->weight = wt;
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
    void render()
    {
        cout << "Rendering " << color << ", " << texture << ", " << material
             << " asteroid at (" << posX << "," << posY
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
    static size_t getMemoryUsage()
    {
        return sizeof(int) * 7 +    // length, width, weight, x, y, velocityX, velocityY
               sizeof(string) * 3 + // color, texture, material string objects
               32 * 3;              // Approximate string data (assuming average 10 chars each)
    }
};

// ----------------------------------------------------------------------------
//  SpaceGame — client. Bas asteroids ka vector bhar deta hai.
// ----------------------------------------------------------------------------
class SpaceGame
{
private:
    vector<Asteroid *> asteroids; // 10 lakh alag-alag heap objects

public:
    void spawnAsteroids(int count)
    {
        cout << "\n=== Spawning " << count << " asteroids ===" << endl;

        // Sirf 3 hi variety hai — yahi baat is poore demo ka point hai.
        vector<string> colors = {"Red", "Blue", "Gray"};
        vector<string> textures = {"Rocky", "Metallic", "Icy"};
        vector<string> materials = {"Iron", "Stone", "Ice"};
        int sizes[] = {25, 35, 45};

        for (int i = 0; i < count; i++)
        {
            int type = i % 3; // 0,1,2,0,1,2... -> round-robin se teeno type banenge

            // Har iteration me ek POORA naya object — intrinsic data samet.
            // i=0 aur i=3 dono "Red/Rocky/Iron" hain, par phir bhi do alag
            // objects ban rahe hain jinme wahi 3 strings dubara store ho rahi hain.
            asteroids.push_back(new Asteroid(
                sizes[type], sizes[type], sizes[type] * 10,    // <- intrinsic (repeat hota hai)
                colors[type], textures[type], materials[type], // <- intrinsic (repeat hota hai)
                100 + i * 50,                                  // Simple x: 100, 150, 200, 250...   <- extrinsic (unique)
                200 + i * 30,                                  // Simple y: 200, 230, 260, 290...   <- extrinsic (unique)
                1,                                             // All move right with velocity 1     <- extrinsic
                2                                              // All move down with velocity 2      <- extrinsic
                ));
        }

        cout << "Created " << asteroids.size() << " asteroid objects" << endl;
        // ^ 10,00,000 objects. WithFlyWeight me ye line "contexts" bolegi aur
        //   saath me "Total flyweight objects: 3" bhi print hoga.
    }

    void renderAll()
    {
        cout << "\n--- Rendering first 5 asteroids ---" << endl;
        // Sirf 5 print kar rahe hain — 10 lakh lines terminal me kaun dekhega.
        for (int i = 0; i < min(5, (int)asteroids.size()); i++)
        {
            asteroids[i]->render();
        }
    }

    // Total = per-object size × object count. Bilkul linear growth.
    // Koi sharing nahi, isliye count double karo to memory bhi double.
    size_t calculateMemoryUsage()
    {
        return asteroids.size() * Asteroid::getMemoryUsage();
    }

    int getAsteroidCount()
    {
        return asteroids.size();
    }
};

int main()
{
    const int ASTEROID_COUNT = 1e6; // 1,000,000 asteroids (1 million asteroids)

    cout << "\n TESTING WITHOUT FLYWEIGHT PATTERN" << endl;
    SpaceGame *game = new SpaceGame();

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
