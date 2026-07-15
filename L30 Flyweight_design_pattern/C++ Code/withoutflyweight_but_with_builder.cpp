 // ============================================================================
//  withoutflyweight_but_with_builder.cpp  —  Flyweight ke BINA par Builder ke SAATH
// ----------------------------------------------------------------------------
//  Yahan hum Builder Design Pattern use kar rahe hain Asteroids ko step-by-step
//  construct karne ke liye (taaki 10-parameter wale massive constructor se bacha ja sake).
//  Lekin Flyweight Pattern use NAHI kiya hai, isliye har Asteroid object apni saari
//  fields khud duplicate karke rakhta hai -> Memory consumption abhi bhi high rahega.
//
//  BUILDER KA FAIDA:
//    Code readability improve hoti hai, telescoping constructors se bacha ja sakta hai,
//    aur client code me object creation bohot clean lagta hai.
//  FLYWEIGHT NA HONE KA NUKSAN:
//    Har asteroid unique object hai aur memory share nahi ho rahi.
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
    // Intrinsic properties (duplicated for each object because no Flyweight is used)
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

    // Private Constructor so client must use Builder to instantiate Asteroid
    // Taaki koi bina Builder ke direct `new Asteroid(...)` na kar sake aur
    // hume telescoping constructor (10 arguments) ka direct samna na karna pade.
    Asteroid(int l, int w, int wt, const string& col, const string& tex,
             const string& mat, int posX, int posY, int velX, int velY) {
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

public:
    // AsteroidBuilder class ko friend declare kiya hai taaki wo hamare private constructor ko access kar sake
    friend class AsteroidBuilder;

    // render() ko koi parameter nahi chahiye, kyunki position/velocity object ke ANDAR hi padi hai.
    // (Flyweight me yahan position bahar se pass karni padti hai)
    void render() {
        cout << "Rendering " << color <<", " << texture << ", " << material
            <<" asteroid at (" << posX << "," << posY
            << ") Size: " << length << "x" << width
            << " Velocity: (" << velocityX << ", "
            << velocityY << ")" << endl;
    }

    // Memory usage per object calculation (approximate size)
    // C++ strings heap variables storage reference ke saath sizeof(string) == 24 or 32 bytes hoti hain.
    // Flyweight na hone ki wajah se har asteroid ke liye redundant string copies RAM me store hoti hain.
    static size_t getMemoryUsage() {
        return sizeof(int) * 7 +                // length, width, weight, x, y, velocityX, velocityY
               sizeof(string) * 3 +             // color, texture, material string objects
               32 * 3;                          // Approximate string data (assuming average 10 chars each)
    }
};

// ----------------------------------------------------------------------------
//  AsteroidBuilder — Builder Object. Ye Asteroid ki sabhi properties ko 
//  step-by-step accumulate karega aur Fluent Interface (Method Chaining) 
//  ke zariye ek clean readable code likhne me madad karega.
// ----------------------------------------------------------------------------
class AsteroidBuilder {
private:
    // Staging variables jo initial default values hold karenge build() call hone tak
    int length = 0;
    int width = 0;
    int weight = 0;
    string color;
    string texture;
    string material;
    int posX = 0;
    int posY = 0;
    int velocityX = 0;
    int velocityY = 0;

public:
    // Fluent methods returning reference to current Builder (*this) for chaining
    
    AsteroidBuilder& setDimensions(int l, int w) {
        this->length = l;
        this->width = w;
        return *this; // Method chaining support ke liye current object ka reference return kiya
    }

    AsteroidBuilder& setWeight(int wt) {
        this->weight = wt;
        return *this;
    }

    AsteroidBuilder& setColor(const string& col) {
        this->color = col;
        return *this;
    }

    AsteroidBuilder& setTexture(const string& tex) {
        this->texture = tex;
        return *this;
    }

    AsteroidBuilder& setMaterial(const string& mat) {
        this->material = mat;
        return *this;
    }

    AsteroidBuilder& setPosition(int x, int y) {
        this->posX = x;
        this->posY = y;
        return *this;
    }

    AsteroidBuilder& setVelocity(int vx, int vy) {
        this->velocityX = vx;
        this->velocityY = vy;
        return *this;
    }

    // Final build command jo private constructor ko call karke actual Asteroid banayegi.
    // AsteroidBuilder friend class hai, isliye iske paas private constructor ka access hai.
    Asteroid* build() const {
        return new Asteroid(length, width, weight, color, texture, material, posX, posY, velocityX, velocityY);
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

            // Builder Pattern ka jaadu:
            // 1. Telescoping parameters se chhutkara (No complex 10 parameters constructors).
            // 2. Client code bohot readable hai, clear pata chalta hai kaunsi value kis property ke liye hai.
            // 3. Lekin dhyan rahe: Flyweight NA hone ke karan har object ke paas apni intrinsic details copy hain.
            Asteroid* ast = AsteroidBuilder()
                .setDimensions(sizes[type], sizes[type])
                .setWeight(sizes[type] * 10)
                .setColor(colors[type])
                .setTexture(textures[type])
                .setMaterial(materials[type])
                .setPosition(100 + i * 50, 200 + i * 30)
                .setVelocity(1, 2)
                .build();

            asteroids.push_back(ast);
        }

        cout << "Created " << asteroids.size() << " asteroid objects" << endl;
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
    const int ASTEROID_COUNT = 1e6; // 1,000,000 asteroids (1 million asteroids)

    cout << "\n TESTING WITHOUT FLYWEIGHT (BUT WITH BUILDER PATTERN)" << endl;
    SpaceGame* game = new SpaceGame();

    game->spawnAsteroids(ASTEROID_COUNT);

    // Show first 5 asteroids to see the pattern
    game->renderAll();

    // Calculate and display memory usage
    size_t totalMemory = game->calculateMemoryUsage();

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
