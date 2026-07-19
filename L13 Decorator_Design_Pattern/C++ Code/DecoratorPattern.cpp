// ============================================================================
//  DecoratorPattern.cpp  —  DECORATOR DESIGN PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Decorator = "object ke UPAR ek parat (layer) chadha do, jo usme naya feature
//               jod de — bina uski class ko chhue, aur RUNTIME pe."
//
//  Yahan Mario ka example hai: basic Mario pe HeightUp, Gun, Star — ek ke upar
//  ek "wrap" hote jaate hain, bilkul PYAAZ ki parton ki tarah. 🧅
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DECORATOR KYUN? — inheritance se "CLASS EXPLOSION" ho jaata hai      │
//  │                                                                          │
//  │  Socho tum inheritance se ye banate. 3 power-ups ke liye kitni classes?  │
//  │      MarioWithHeight                                                     │
//  │      MarioWithGun                                                        │
//  │      MarioWithStar                                                       │
//  │      MarioWithHeightAndGun                                               │
//  │      MarioWithHeightAndStar                                              │
//  │      MarioWithGunAndStar                                                 │
//  │      MarioWithHeightAndGunAndStar                                        │
//  │  = 7 classes! (har power-up "hai ya nahi" -> 2×2×2 − 1)                  │
//  │                                                                          │
//  │  Formula: n power-ups ->  2^n − 1  classes 😱                            │
//  │      3 power-ups ->  7 classes                                           │
//  │      5 power-ups ->  31 classes                                          │
//  │     10 power-ups ->  1023 classes!!                                      │
//  │                                                                          │
//  │  Decorator se? Sirf  n + 1  classes:                                     │
//  │      3 power-ups -> 4 classes (Mario + 3 decorators)                     │
//  │     10 power-ups -> 11 classes                                           │
//  │                                                                          │
//  │  📌 Yaad rakhne ka formula:  2^n  ->  n+1                                │
//  │     GUNA ko JOD me badal deta hai. (L34 ka Bridge bhi aisa hi karta tha  │
//  │      — M×N ko M+N. Idea alag, fayda ek jaisa.)                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ DECORATOR KA ASLI JAADU — "IS-A" aur "HAS-A" DONO ek saath:
//
//      class Character_Decorator : public Character   // <- IS-A  (inherit)
//      {
//      protected:
//          Character *character;                      // <- HAS-A (wrap)
//      };
//
//     Ye dono cheezein saath me kyun? Dono ka apna kaam hai:
//       IS-A  -> decorator KHUD ek Character hai, isi liye use wahan bhi rakh
//                sakte ho jahan Character chahiye (aur doosre decorator uske
//                upar chadh sakte hain)
//       HAS-A -> decorator ke ANDAR ek Character hai, jisko wo wrap karke uska
//                kaam aage badhata hai
//
//     Sirf IS-A hota -> to wrap nahi kar paate (wahi inheritance ka jaal)
//     Sirf HAS-A hota -> to decorator khud Character nahi hota, aur uske upar
//                        doosra decorator nahi chadh paata
//     📌 DONO milke hi "parat pe parat" (stacking) possible banate hain.
//
//  PATTERN KE ROLES (GoF naming) is file me:
//     1. Component (interface)  -> Character            : common contract
//     2. ConcreteComponent      -> Mario                : asli basic object
//     3. Decorator (abstract)   -> Character_Decorator  : IS-A + HAS-A wala base
//     4. ConcreteDecorator      -> HeightUp, GunPowerUp, StarPowerUp
//
//  🧹 MEMORY: `Character_Decorator` ka destructor apne andar wale character ko
//     delete karta hai — aur wo RECURSIVE hai, to ek `delete mario` se poori
//     chain (Mario tak) apne aap saaf ho jaati hai. Detail wahin likhi hai.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
//  1. COMPONENT (interface) — Character
// ----------------------------------------------------------------------------
//  Ye "contract" hai jo Mario aur SAARE decorators, dono follow karte hain.
//
//  ⭐ Yahi is pattern ki jad hai: kyunki Mario aur decorator DONO `Character`
//  hain, isi liye:
//     - decorator Mario ko wrap kar sakta hai
//     - decorator DOOSRE decorator ko bhi wrap kar sakta hai (stacking!)
//     - client ko farak hi nahi padta ki uske paas kya hai — sabse `Character*`
//       ki tarah baat karta hai
// ============================================================================
class Character
{
public:
    // `= 0` -> pure virtual. Mario ho ya koi decorator — sabko ye likhna PADEGA.
    //
    // `const` -> ye function object ka data badal nahi sakta, sirf padh sakta
    // hai. Getter pe hamesha `const` lagao — compiler khud check karega.
    virtual string getAbilities() const = 0;

    // Virtual destructor — ZAROORI hai. Client `Character*` (base pointer) me
    // rakhta hai, par asal object `StarPowerUp` hai. `delete mario` BASE pointer
    // se hota hai — iske bina derived ka destructor chalta hi nahi.
    //
    // 📌 RULE: class me ek bhi `virtual` function hai? To destructor bhi virtual.
    virtual ~Character() {} // Virtual destructor
};

// ============================================================================
//  2. CONCRETE COMPONENT — Mario (basic, bina kisi power-up ke)
// ----------------------------------------------------------------------------
//  Sabse andar wali "parat" — pyaaz ka core. 🧅
//
//  ⭐ Notice: Mario ko power-ups ke baare me KUCH NAHI pata! Wo bas "Mario"
//  bolta hai. Naye power-ups aate rahenge, Mario ka code kabhi nahi badlega.
//  Yahi OPEN/CLOSED PRINCIPLE hai — extension ke liye khula, modification ke
//  liye band. Aur SINGLE RESPONSIBILITY bhi: Mario ka kaam sirf Mario hona hai.
// ============================================================================
class Mario : public Character
{
public:
    string getAbilities() const override
    {
        return "Mario"; // recursion ka BASE CASE — yahin chain rukti hai
    }
};

// ============================================================================
//  3. ABSTRACT DECORATOR — Character_Decorator (pattern ka DIL)
// ----------------------------------------------------------------------------
//  Ye class hi poore pattern ka dhaancha hai. Ismein "IS-A + HAS-A" ka jodha
//  hai (upar file-header me poora explanation).
//
//  ⭐ `Character *character` — ye INTERFACE ka pointer hai, `Mario*` nahi!
//  Isi ek baat se stacking possible hoti hai:
//       HeightUp(mario)              -> Mario ko wrap kiya
//       GunPowerUp(heightUpObject)   -> DECORATOR ko wrap kiya!
//  Agar ye `Mario*` hota, to sirf Mario wrap hota — decorator pe decorator
//  kabhi na chadhta, aur poora pattern bekaar ho jaata.
//
//  💡 Ye class ABSTRACT hai — kyunki isme `getAbilities()` ka body nahi hai.
//  Wo `Character` se pure-virtual hi rahega. Matlab iska object banaya hi nahi
//  ja sakta — jo sahi hai, kyunki "generic decorator" ka koi matlab nahi.
//  Sirf concrete decorators (HeightUp waghairah) hi ban sakte hain.
// ============================================================================
// Abstract Decorator: Character_Decorator "is-a realtionship" Charatcer and "has-a" Character.
class Character_Decorator : public Character
{
protected:
    // `protected` -> derived decorators (HeightUp waghairah) isko seedha use kar
    // sakte hain, par bahar wala koi nahi.
    Character *character; // Wrapped component (has-a relationship)

public:
    Character_Decorator(Character *c)
    {
        this->character = c;
    }

    // ⭐⭐ YE EK LINE POORE PATTERN KA SABSE ZAROORI HISSA HAI
    //
    //  Decorator apne andar wale character ka MAALIK hai — to usko delete karna
    //  bhi uski hi zimmedari hai.
    //
    //  🔥 SABSE KHOOBSURAT BAAT — ye khud RECURSIVE hai! Ek line poori chain
    //  saaf kar deti hai:
    //
    //      delete mario  (StarPowerUp)
    //        -> ~Character_Decorator -> delete GunPowerUp
    //             -> ~Character_Decorator -> delete HeightUp
    //                  -> ~Character_Decorator -> delete Mario   <- chain khatam
    //
    //  Har parat apne ANDAR wali parat ko maarti hai — pyaaz ulta chhilte hue. 🧅
    //  Client ko sirf `delete mario` likhna hai, baaki sab apne aap.
    //
    //  ⚠ Ye pehle MISSING tha, aur wahi is file ka sabse bada bug tha:
    //     `delete mario` sirf sabse BAHAR wale decorator ko maarta tha, aur
    //     andar ki poori chain LEAK ho jaati thi. 7 objects bante the, sirf 1
    //     delete hota tha -> 6 LEAK. 💀
    //     Uska saboot output me hi tha: DO StarPowerUp bante the, par
    //     "Destroying StarPowerUp Decorator" sirf EK baar print hota tha.
    //     Ab dono print hote hain. ✅
    //
    //  📌 RULE: agar class raw pointer ki MAALIK hai, to uska destructor use
    //     delete kare. (Ya `unique_ptr` use karo — tab ye jhanjhat hi khatam.)
    //
    //  `override` -> compiler check karega ki base me sach me virtual destructor
    //  hai. (Hai — `Character` me.) Ye typo se bachata hai.
    ~Character_Decorator() override
    {
        delete character;
    }
};

// ============================================================================
//  4. CONCRETE DECORATORS — asli power-ups
// ----------------------------------------------------------------------------
//  Teeno ka dhaancha BILKUL ek jaisa hai — aur yahi baat hai:
//
//      return character->getAbilities()  +  " apna naya feature";
//             └── (1) andar wale se poochho ──┘   └── (2) apna jodo ─┘
//
//  Ye "recursion" jaisa chalta hai. Sabse bahar wale se shuru hoke andar tak
//  jaata hai, phir wapas bahar aate hue har parat apna hissa jodti hai:
//
//      StarPowerUp.getAbilities()
//        -> GunPowerUp.getAbilities()
//             -> HeightUp.getAbilities()
//                  -> Mario.getAbilities() = "Mario"        <- BASE CASE
//             <- "Mario with HeightUp"
//        <- "Mario with HeightUp with Gun"
//      <- "Mario with HeightUp with Gun with Star Power"
// ============================================================================

// Concrete Decorator: Height-Increasing Power-Up.
class HeightUp : public Character_Decorator
{
public:
    // `: Character_Decorator(c)` -> base ka constructor bulao, jo `character`
    // set kar deta hai. Har decorator me yahi ek line hoti hai.
    HeightUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with HeightUp";
        //     └── andar wale se poochho ─┘   └── apna jodo ─┘
    }
};

// Concrete Decorator: Gun Shooting Power-Up.
class GunPowerUp : public Character_Decorator
{
public:
    GunPowerUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with Gun";
    }
};

// Concrete Decorator: Star Power-Up (temporary ability).
class StarPowerUp : public Character_Decorator
{
public:
    StarPowerUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with Star Power (Limited Time)";
    }

    // ⭐ Ye destructor is file ka sabse kaam ka "detective" hai!
    //
    // Ye print karta hai jab StarPowerUp destroy hota hai. Ab main() dekho —
    // DO StarPowerUp bante hain. To ye message DO baar aana chahiye, na?
    //
    // Par output me sirf EK baar aata hai. 🔍
    //
    // Yahi upar wale memory leak ka SEEDHA SABOOT hai — doosra StarPowerUp
    // (aur uske andar ki poori chain) kabhi destroy hota hi nahi.
    // 📌 Sabak: ek chhota sa print destructor me daal do — memory bugs turant
    //    pakde jaate hain, bina kisi tool ke.
    ~StarPowerUp()
    {
        cout << "Destroying StarPowerUp Decorator" << endl;
    }
};

// ============================================================================
//  5. CLIENT — demo
// ============================================================================
int main()
{
    // ---- Parat 1: basic Mario (pyaaz ka core) 🧅 --------------------------
    Character *mario = new Mario();
    cout << "Basic Character: " << mario->getAbilities() << endl;
    // -> "Mario"

    // ---- Parat 2: HeightUp chadha do --------------------------------------
    //
    // ⭐ Dhyaan se dekho — `mario` ko REASSIGN kar rahe hain:
    //       mario = new HeightUp(mario);
    //               └── naya object    └── purana mario ANDAR chala gaya
    //
    // Ab `mario` HeightUp ko point kar raha hai, aur purana Mario uske andar
    // hai. Ye chalti hai kyunki DONO `Character*` hain (IS-A ka fayda).
    mario = new HeightUp(mario);
    cout << "After HeightUp: " << mario->getAbilities() << endl;
    // -> "Mario with HeightUp"

    // ---- Parat 3: Gun bhi chadha do ---------------------------------------
    // Ab GunPowerUp ek DECORATOR ko wrap kar raha hai (Mario ko nahi!). Yahi
    // stacking hai — aur ye isi liye possible hai kyunki `character` ka type
    // `Character*` hai, `Mario*` nahi.
    mario = new GunPowerUp(mario);
    cout << "After GunPowerUp: " << mario->getAbilities() << endl;
    // -> "Mario with HeightUp with Gun"

    // ---- Parat 4: Star bhi ------------------------------------------------
    mario = new StarPowerUp(mario);
    cout << "After StarPowerUp: " << mario->getAbilities() << endl;
    // -> "Mario with HeightUp with Gun with Star Power (Limited Time)"
    //
    // ⭐ Notice: har baar Mario ki class BILKUL nahi badli. Sirf uske upar
    // parat chadhti gayi. Aur ye sab RUNTIME pe hua — compile time pe nahi.
    // Inheritance me ye namumkin hai (class compile time pe fix ho jaati hai).

    cout << endl;

    // ---- "Just for learning" — sab ek hi line me -------------------------
    //
    // Ye wahi cheez hai, bas ek line me — andar se bahar padho:
    //     HeightUp(mario)  -> uske upar GunPowerUp -> uske upar StarPowerUp
    //
    // ⚠ Par dhyaan do: `mario` me pehle se HI 4 parat thi! To ab chain 7 objects
    // ki ho gayi:
    //     StarPowerUp -> GunPowerUp -> HeightUp -> StarPowerUp -> GunPowerUp
    //                 -> HeightUp -> Mario
    // Isi liye output me "with HeightUp with Gun with Star Power" DO BAAR aata
    // hai — kyunki chain me sach me do baar hai! Ye galti nahi, aisa hi likha hai.
    mario = new StarPowerUp(new GunPowerUp(new HeightUp(mario)));
    cout << "Just for learning :" << mario->getAbilities() << endl;

    // ---- ✅ CLEANUP — ek `delete`, aur POORI chain saaf --------------------
    //
    //  Client ko bas itna likhna hai. Baaki sab apne aap:
    //     delete mario (StarPowerUp)
    //       -> uska destructor andar wale GunPowerUp ko delete karta hai
    //            -> wo HeightUp ko
    //                 -> wo StarPowerUp ko ... Mario tak (7 ke 7 objects)
    //
    //  Ye `Character_Decorator::~Character_Decorator()` ki wajah se hota hai
    //  (upar dekho) — ek line, aur poori chain recursive tareeke se saaf. 🧅
    //
    //  🔍 OUTPUT ME KHUD DEKHO: ab "Destroying StarPowerUp Decorator" DO BAAR
    //  print hota hai — kyunki chain me sach me do StarPowerUp hain aur ab DONO
    //  destroy hote hain. Pehle sirf EK baar aata tha (wahi leak ka saboot tha).
    //
    //  💡 Aur bhi saaf tareeka: `unique_ptr<Character>` use karo — tab destructor
    //     likhne ki zaroorat hi nahi padti, aur bhoolne ka sawaal hi khatam.
    //     Yahan raw pointer isi liye rakha hai taaki ownership manually dikhe.
    delete mario;

    return 0;
}
