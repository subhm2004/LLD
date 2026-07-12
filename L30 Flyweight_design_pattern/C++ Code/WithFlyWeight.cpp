// ============================================================================
//  WithFlyWeight.cpp  —  Flyweight Design Pattern (Structural) — FIXED version
// ----------------------------------------------------------------------------
//  Flyweight = bahut saare similar objects ki SHARED heavy data (intrinsic
//  state: color, texture, size) ko ek hi baar store karo, aur per-object unique
//  data (extrinsic state: position, velocity) alag rakho. Yahan 1,000,000
//  asteroids sirf 3 shared flyweights use karte hain -> memory bahut kam.
//  Factory cache karta hai taaki same-type flyweight dobara na bane.
//  (Compare with WithoutFlyWeight.cpp jahan har object apni copy rakhta hai.)
//
//  Pattern ke roles (GoF naming) is file me:
//    1. Flyweight        -> AsteroidFlyweight  (sirf INTRINSIC/shared state)
//    2. FlyweightFactory -> AsteroidFactory    (cache + reuse; naya tabhi banega
//                                               jab wo type pehle se na ho)
//    3. Context / Client -> AsteroidContext    (EXTRINSIC state + flyweight ka pointer)
//
//  Pura idea ek line me:
//    "Jo data sabka same hai use EK baar store karo aur POINTER share kar do;
//     jo data har object ka alag hai wahi per-object rakho."
//
//  Kaise pata karein kaunsa field intrinsic hai?
//    Khud se poochho: "Do alag asteroids ke liye ye value same ho sakti hai kya,
//    bina kuch toote?" Haan -> intrinsic (share karo). Nahi -> extrinsic (alag rakho).
//      color/texture/material/size -> haan, type pe depend karta hai -> SHARED
//      position/velocity           -> nahi, har asteroid ka apna         -> CONTEXT
// ============================================================================
#include <iostream>
#include <vector>
#include <unordered_map>  // factory ka cache — key se flyweight dhoondhne ke liye
#include <string>
#include <random>
#include <memory>
#include <chrono>
using namespace std;

// ============================================================================
//  1. FLYWEIGHT — sirf INTRINSIC (shared) state store karta hai
// ----------------------------------------------------------------------------
//  Is class ke poore program me sirf 3 objects banenge (3 asteroid types).
//  Inhi 3 pe 10 lakh contexts point karenge.
//
//  Golden rule: is class me koi bhi aisi field MAT daalo jo har asteroid ki
//  alag ho (jaise posX). Kyunki object SHARED hai — agar posX yaha daala, to
//  ek asteroid ki position change karne se baaki 3 lakh asteroids ki position
//  bhi badal jayegi. Isi liye flyweight ko immutable rakhte hain.
// ============================================================================
class AsteroidFlyweight {
private:
    // Intrinsic properties (shared among asteroids of same type)
    // Ye 6 fields ~180 bytes lete hain, par ab sirf 3 baar — 10 lakh baar nahi.
    // Yaani shared data ka TOTAL cost = 3 × 180 = 540 bytes. Bas itna hi.
    int length;
    int width;
    int weight;
    string color;
    string texture;
    string material;

public:
    AsteroidFlyweight(int l, int w, int wt, string col, string tex,
        string mat) {
            this->length = l;
            this->width = w;
            this->weight = wt;   
            this->color = col;
            this->texture = tex;
            this->material = mat;
    }

    // >>> YAHI PATTERN KA SABSE IMPORTANT HISSA HAI <<<
    // WithoutFlyWeight me signature tha: render()  — koi param nahi.
    // Yaha hai: render(posX, posY, velocityX, velocityY) — 4 params.
    //
    // Kyun? Kyunki flyweight ke paas position hai hi nahi (wo shared hai, position
    // rakh hi nahi sakta). To position use CALL TIME pe bahar se "inject" ki jaati hai.
    // Isi ko kehte hain: "extrinsic state ko method parameter ke through pass karna".
    void render(int posX, int posY, int velocityX, int velocityY) {
        cout << "Rendering " << color <<", " << texture << ", " << material
            <<" asteroid at (" << posX << "," << posY
            << ") Size: " << length << "x" << width
            << " Velocity: (" << velocityX << ", "
            << velocityY << ")" << endl;
    }

    // Ek flyweight ka size = 180 bytes (12 + 72 + 96).
    // Utna hi heavy hai jitna WithoutFlyWeight ka Asteroid tha —
    // farq sirf itna ki ye cost 3 BAAR lagti hai, per-object nahi.
    static size_t getMemoryUsage() {
        return sizeof(int) * 3 +            // length, width, weight
                sizeof(string) * 3 +        // color, texture, material string objects
                32 * 3;                     // Approximate string data
    }
};

// ============================================================================
//  2. FLYWEIGHT FACTORY — cache/pool. Duplicate flyweight banne hi nahi deta.
// ----------------------------------------------------------------------------
//  Client kabhi bhi `new AsteroidFlyweight(...)` DIRECT nahi karega. Hamesha
//  factory se maangega. Factory dekhega: "ye type pehle se bana hua hai?"
//    - Haan -> wahi purana pointer wapas de do (REUSE)
//    - Nahi -> ek baar bana ke cache me daalo, phir do
//  Isi wajah se 10 lakh requests aane ke baad bhi map me sirf 3 entries hongi.
// ============================================================================
class AsteroidFactory {
private:
    // static = poore program me ek hi cache (class ka, kisi object ka nahi).
    // key   = asteroid type ka unique "fingerprint"
    // value = us type ka ek-hi-ek shared flyweight
    static unordered_map<string, AsteroidFlyweight*> flyweights;

public:
    static AsteroidFlyweight* getAsteroid(int length, int width, int weight,
                                        string color, string texture, string material) {

        // Key = saare intrinsic fields ko jod ke ek unique string.
        // e.g. "25_25_250_Red_Rocky_Iron"
        // Do asteroids ka key same = wo same type ke hain = same flyweight milega.
        string key = to_string(length) + "_" + to_string(width) + "_" + to_string(weight) +
                    "_" + color + "_" + texture + "_" + material;

        // Cache MISS -> ab banega (ye poore run me sirf 3 baar hoga: i=0,1,2 pe)
        if (flyweights.find(key) == flyweights.end()) {
            flyweights[key] = new AsteroidFlyweight(length, width, weight, color, texture, material);
        }

        // Cache HIT -> purana pointer hi wapas (baaki 999,997 calls yahi se return hongi)
        return flyweights[key];
    }

    // Proof ke liye — main me print hota hai. 10 lakh spawn ke baad bhi output "3" hi aayega.
    static int getFlyweightCount() {
        return flyweights.size();
    }

    // Shared data ka TOTAL cost = 3 × ek flyweight ka size. Bas itna hi.
    static size_t getTotalFlyweightMemory() {
        return flyweights.size() * AsteroidFlyweight::getMemoryUsage();
    }

    static void cleanup() {
        // NOTE: ye sirf map ke ENTRIES hata raha hai, `new` kiye hue
        // AsteroidFlyweight objects delete nahi kar raha -> leak.
        // Sahi tareeka: pehle loop chala ke `delete pair.second;` karo, phir clear().
        // (Ya `unordered_map<string, unique_ptr<AsteroidFlyweight>>` use karo.)
        flyweights.clear();
    }
};

// Static member definition
// C++ me static member ko class ke BAAHAR ek baar define karna zaroori hai,
// warna linker error aata hai ("undefined reference"). Class ke andar wali line
// sirf declaration thi.
unordered_map<string, AsteroidFlyweight*> AsteroidFactory::flyweights;


// ============================================================================
//  3. CONTEXT — Stores EXTRINSIC state only
// ----------------------------------------------------------------------------
//  Ye hai wo object jo 10 lakh baar banega. Isliye ise HALKA hona chahiye.
//  Andar strings nahi hain, sirf ek pointer + 4 int -> 24 bytes.
//  (WithoutFlyWeight ka Asteroid 196 bytes ka tha. Yahi ~8x saving hai.)
// ============================================================================
class AsteroidContext {
private:
    AsteroidFlyweight* flyweight;          // 8 bytes — shared type ka POINTER
                                           // (data copy nahi, sirf address. 3 lakh
                                           //  contexts isi ek address pe point karenge)
    int posX, posY;                        // 8 bytes (position)   <- iska apna
    int velocityX, velocityY;              // 8 bytes (velocity)   <- iska apna

public:

AsteroidContext(AsteroidFlyweight* fw, int posX, int posY, int velX, int velY) {
        this->flyweight = fw;   // flyweight yaha BANTA nahi, sirf ATTACH hota hai
        this->posX = posX;
        this->posY = posY;
        this->velocityX = velX;
        this->velocityY = velY;
    }

    // Context apna extrinsic data flyweight ko UDHAAR deta hai render ke liye.
    // Flyweight: "shakal main bata dunga, jagah tu bata."
    void render() {
        flyweight->render(posX, posY, velocityX, velocityY);
    }

    static size_t getMemoryUsage() {
        return sizeof(AsteroidFlyweight*) +  // 8 bytes (64-bit machine pe pointer)
                sizeof(int) * 4;             // 16 bytes (posX, posY, velX, velY)
                                             // = 24 bytes per asteroid. Bas.
    }
};

// ============================================================================
//  CLIENT — SpaceGame. Bahar se dekho to WithoutFlyWeight jaisa hi lagta hai.
// ============================================================================
class SpaceGameWithFlyweight {
private:
    vector<AsteroidContext*> asteroids; // 10 lakh HALKE contexts (fat objects nahi)

public:
    void spawnAsteroids(int count) {
        cout << "\n=== Spawning " << count << " asteroids ===" << endl;

        vector<string> colors = {"Red", "Blue", "Gray"};
        vector<string> textures = {"Rocky", "Metallic", "Icy"};
        vector<string> materials = {"Iron", "Stone", "Ice"};
        int sizes[] = {25, 35, 45};

        for (int i = 0; i < count; i++) {
            int type = i % 3;

            // STEP 1: Type ka shared flyweight factory se maango.
            // i=0,1,2 pe naya banega. i=3 se aage HAMESHA cache se purana milega.
            // Yaani 10 lakh loop iterations, par sirf 3 AsteroidFlyweight objects.
            AsteroidFlyweight* flyweight = AsteroidFactory::getAsteroid(
                sizes[type], sizes[type], sizes[type] * 10,
                colors[type], textures[type], materials[type]
            );

            // STEP 2: Sirf unique data ka halka context banao, usme flyweight ka
            // pointer chipka do. Heavy strings yaha dobara copy NAHI ho rahi —
            // yahi WithoutFlyWeight se asli farq hai.
            asteroids.push_back(new AsteroidContext(
                flyweight,                      // <- shared (pointer, 8 bytes)
                100 + i * 50,                   // Simple x: 100, 150, 200, 250...
                200 + i * 30,                   // Simple y: 200, 230, 260, 290...
                1,                              // All move right with velocity 1
                2                               // All move down with velocity 2
            ));
        }

        cout << "Created " << asteroids.size() << " asteroid contexts" << endl;
        cout << "Total flyweight objects: " << AsteroidFactory::getFlyweightCount() << endl;
        // ^^ 10,00,000 contexts banane ke baad bhi ye "3" print karega.
        //    Yahi ek line pure pattern ka proof hai.
    }

    void renderAll() {
        cout << "\n--- Rendering first 5 asteroids ---" << endl;
        // Output WithoutFlyWeight se BILKUL same aayega — user ko farq nahi dikhega.
        // Pattern behavior nahi badalta, sirf memory bachata hai. Yahi to khoobi hai.
        for (int i = 0; i < min(5, (int)asteroids.size()); i++) {
            asteroids[i]->render();
        }
    }

    // Ab total memory = do hisso ka jod:
    //   contextMemory   = 10,00,000 × 24 = 24,000,000 bytes (linear — count ke saath badhta hai)
    //   flyweightMemory = 3 × 180        =        540 bytes (CONSTANT — chahe 1 crore asteroid ho)
    //   total           = 24,000,540 bytes (~22.89 MB)
    // WithoutFlyWeight me heavy hissa bhi 10,00,000 baar count hota tha (196 × 10 lakh
    // = ~186.92 MB). Us 540-byte ke constant ne ~164 MB bacha diye.
    size_t calculateMemoryUsage() {
        size_t contextMemory = asteroids.size() * AsteroidContext::getMemoryUsage();
        size_t flyweightMemory = AsteroidFactory::getTotalFlyweightMemory();
        return contextMemory + flyweightMemory;
    }

    int getAsteroidCount() {
        return asteroids.size();
    }
};

int main() {
    const int ASTEROID_COUNT = 1000000;

    cout << "\nTESTING WITH FLYWEIGHT PATTERN" << endl;
    SpaceGameWithFlyweight* game = new SpaceGameWithFlyweight();

    game->spawnAsteroids(ASTEROID_COUNT);

    // Show first 5 asteroids to see the pattern
    game->renderAll();

    // Calculate and display memory usage
    size_t totalMemory = game->calculateMemoryUsage();

    // Isko WithoutFlyWeight.cpp ke output ke saath side-by-side rakh ke dekho:
    //   Without : 196 bytes/asteroid -> 186.92 MB
    //   With    :  24 bytes/asteroid ->  22.89 MB   (~8x kam)
    cout << "\n=== MEMORY USAGE ===" << endl;
    cout << "Total asteroids: " << ASTEROID_COUNT << endl;
    cout << "Memory per asteroid: " << AsteroidContext::getMemoryUsage() << " bytes" << endl;
    cout << "Total memory used: " << totalMemory << " bytes" << endl;
    cout << "Memory in MB: " << totalMemory / (1024.0 * 1024.0) << " MB" << endl;

    // NOTE: AsteroidFactory::cleanup() call hi nahi hua, aur `game` + 10 lakh
    // contexts bhi delete nahi hue -> leak. Demo ke liye theek hai.
    //
    // KAB FLYWEIGHT USE KARNA HAI (interview answer):
    //   - Objects ki count BAHUT zyada ho (lakhs/millions), AUR
    //   - unme se zyadatar data DUPLICATE ho (unique types kam hon), AUR
    //   - wo shared data IMMUTABLE ho (koi ek object usko badal na sake).
    // Agar har object ka data genuinely unique hai, to Flyweight ka koi fayda nahi —
    // ulta factory lookup ka overhead aur pointer indirection hi milega.

    return 0;
}
