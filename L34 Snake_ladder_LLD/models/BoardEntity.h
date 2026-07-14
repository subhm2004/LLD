// ============================================================================
//  models/BoardEntity.h  —  Saanp aur Seedhi ka COMMON model
// ----------------------------------------------------------------------------
//  Saanp aur Seedhi me farak kya hai? Sochne pe... bahut kam!
//
//     Saanp  : position A pe pahuncho -> B pe pahuncha diye jaao, jahan B < A 🐍
//     Seedhi : position A pe pahuncho -> B pe pahuncha diye jaao, jahan B > A 🪜
//
//  DONO ka kaam bilkul EK HI hai: "start pe aao, end pe chale jaao."
//  Bas DISHA ka farak hai — saanp neeche le jaata hai, seedhi upar.
//
//  Isi liye dono ka ek hi base class hai: `BoardEntity` (start + end).
//  Aur isi wajah se `Board` ko ye jaanne ki zaroorat hi NAHI ki kaunsa entity
//  saanp hai aur kaunsa seedhi. Board bas itna karta hai:
//
//      BoardEntity *e = board->getEntity(position);
//      if (e != nullptr) newPosition = e->getEnd();     // bas! 🎯
//
//  Ek hi line, dono cases handle. Koi `if (isSnake) ... else ...` nahi.
//  Rules engine (`calculateNewPosition`) me dekho — exactly yahi hua hai.
//  Yahi POLYMORPHISM ka asli fayda hai: alag cheezon ka common vyavhaar
//  pehchano, aur unhe ek hi tarah se treat karo.
//
//  ⭐ To phir `name()` kyun chahiye ("SNAKE" / "LADDER")?
//     Sirf DISPLAY ke liye — user ko batane ke liye "Oh no! Snake at 62!" ya
//     "Great! Ladder at 28!". LOGIC me kahin bhi iski zaroorat nahi padti.
//
//     ⚠ Par `name()` ka string return karna thoda kaccha hai. Game engine me
//     `if (entity->name() == "SNAKE")` likha hai — yaani STRING COMPARE se
//     faisla ho raha hai. Ek typo ("SNAKE " ya "Snake") aur logic chup-chaap
//     galat ho jaayega, compiler kuch nahi bolega. 😬
//
//     Behtar: ek `virtual bool isSnake() const` ya ek enum-based type rakho.
//     Ya aur bhi saaf: entity khud apna message banaye —
//         virtual string message(int pos, int dest) = 0;
//     Tab engine ko `if` ki zaroorat hi na pade. Yahi asli polymorphism hoti.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_MODELS_BOARDENTITY_H
#define SNAKE_LADDER_LLD_MODELS_BOARDENTITY_H

#include <bits/stdc++.h>
using namespace std;

namespace snake_ladder_lld {

// ---- BASE: dono ka common dhaancha ----------------------------------------
class BoardEntity {
protected:
    // `protected` -> derived classes (Snake/Ladder) seedha use kar sakti hain,
    // par bahar wala koi nahi. (Display functions me inhe seedha use kiya hai.)
    int startPosition_;   // kahan pe entity ka MUNH hai (jahan pahunchne pe trigger)
    int endPosition_;     // kahan pahuncha degi

public:
    BoardEntity(int start, int end) : startPosition_(start), endPosition_(end) {}

    int getStart() const { return startPosition_; }
    int getEnd() const { return endPosition_; }

    // Pure virtual — har entity ko batana padega ki wo kya hai (display ke liye)
    virtual string name() = 0;
    virtual void display() = 0;

    // Virtual destructor — Board entities ko `BoardEntity*` me rakhta hai, par
    // asal objects Snake/Ladder hain. Board ka destructor `delete entity` karta
    // hai — BASE pointer se. Iske bina derived ka destructor chalta hi nahi.
    // 📌 Ek bhi virtual function hai? To destructor bhi virtual. Har baar.
    virtual ~BoardEntity() {}
};

// ---- SAANP: neeche le jaata hai (end < start) 🐍 ---------------------------
class Snake : public BoardEntity {
public:
    Snake(int start, int end) : BoardEntity(start, end) {}
    string name() override { return "SNAKE"; }
    void display() override { cout << "Snake: " << startPosition_ << " -> " << endPosition_ << endl; }

    // ⚠ Notice: yahan koi check NAHI hai ki `end < start` ho!
    // `new Snake(5, 90)` bilkul chal jaata hai — ek saanp jo UPAR le jaaye. 😵
    // (Test kiya tha — pehle ye aasani se ban jaata tha.)
    //
    // ✅ Ab ye validation `Board::addBoardEntity()` me hai — wahan har entity
    // check hoti hai add hone se pehle. Wo ek hi "darwaza" hai board me ghusne
    // ka, isi liye guard wahan lagana sahi hai. Yahan constructor me bhi laga
    // sakte the, par tab har jagah try/catch karna padta. Ek choke-point pe
    // validation = saaf design.
};

// ---- SEEDHI: upar le jaati hai (end > start) 🪜 ----------------------------
class Ladder : public BoardEntity {
public:
    Ladder(int start, int end) : BoardEntity(start, end) {}
    string name() override { return "LADDER"; }
    void display() override { cout << "Ladder: " << startPosition_ << " -> " << endPosition_ << endl; }
};

}

#endif
