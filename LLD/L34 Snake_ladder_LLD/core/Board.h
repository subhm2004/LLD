// ============================================================================
//  core/Board.h  —  N×N board: cells + snakes/ladders ki positions
// ----------------------------------------------------------------------------
//  ⭐ Board ka size CELLS me hota hai, dimension me nahi:
//       Board(10)  ->  10 × 10  ->  size_ = 100 cells (positions 1 se 100)
//     Player 0 pe shuru hota hai (board se BAHAR), aur 100 pe pahunch ke jeetta hai.
//
//  ⭐ DO alag containers, ek hi data ke liye — kyun?
//       entities_      (vector) -> saari entities, DISPLAY ke liye (list ghumani hai)
//       entityByStart_ (map)    -> position se entity, LOOKUP ke liye
//
//     Game har move pe poochta hai: "position 62 pe kuch hai kya?" Agar sirf
//     vector hoti, to har baar poori list scan karni padti — O(n). Map se ye
//     O(log n) me ho jaata hai.
//
//     ⚠ Trade-off: ek hi entity DO jagah point hoti hai. Delete karte waqt
//     dhyaan rakhna — sirf EK baar delete karo (destructor `entities_` se karta
//     hai, `entityByStart_` se nahi). Warna DOUBLE FREE = crash. 💀
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `addBoardEntity()` = board ka EKMATR DARWAZA                         │
//  │                                                                          │
//  │  Board me koi bhi saanp/seedhi isi ek function se ghusti hai. Isi liye   │
//  │  SAARI validation yahin lagayi hai — ek jagah. Koi bhi strategy (aaj ki  │
//  │  ya kal ki) galat entity nahi daal sakti, chahe wo kitni bhi lapervah ho.│
//  │                                                                          │
//  │  Ise CHOKE POINT design kehte hain: jab har raasta ek hi darwaze se       │
//  │  guzarta ho, to pehredaar ek hi jagah bithao. Har strategy me alag-alag  │
//  │  check likhne se ye kahin zyada behtar hai — aur ek jagah bhoolne ka     │
//  │  khatra bhi khatam.                                                      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef SNAKE_LADDER_LLD_CORE_BOARD_H
#define SNAKE_LADDER_LLD_CORE_BOARD_H

#include <bits/stdc++.h>

#include "../models/BoardEntity.h"

using namespace std;

namespace snake_ladder_lld {

// Forward declaration — Board ko `BoardSetupStrategy*` chahiye (setupBoard me),
// par strategy ko bhi Board chahiye. Murgi-anda wali problem. Pointer banane ke
// liye itna kaafi hai; poori definition BoardSetupBridge.h me jud jaati hai.
class BoardSetupStrategy;

class Board {
private:
    int size_;                            // kul cells (dimension × dimension)
    vector<BoardEntity *> entities_;      // saari entities — display ke liye (aur OWNER yahi hai)
    map<int, BoardEntity *> entityByStart_;  // start position -> entity (fast lookup)

public:
    // ---- ✅ MINIMUM SIZE GUARD — pehle ye check tha hi nahi 🐛 --------------
    //
    //  Pehle `Board(3)` bhi ban jaata tha -> 9 cells. Aur phir setup strategies
    //  me `rand() % (boardSize - 10)` chalta -> `rand() % -1` -> NEGATIVE MODULO!
    //
    //  Natija (test karke dekha tha): 9-cell board pe **"Snake: 10 -> 8"** —
    //  yaani saanp position 10 pe, jabki board hi 1..9 tak hai! Board ke BAHAR
    //  ka saanp — jise koi kabhi chhu hi nahi sakta. 😵
    //
    //  Aur seedhiyon me isse bhi bura: chhote board pe ladder start HAMESHA 1
    //  aata tha, to doosri seedhi kabhi lag hi nahi paati -> INFINITE LOOP. 🔁
    //  (Ye bhi test karke confirm kiya tha — game hang ho jaata tha.)
    //
    //  Ab dimension >= 4 zaroori hai -> size_ >= 16 -> `boardSize - 10 >= 6`,
    //  yaani modulo hamesha POSITIVE. Jad se problem khatam.
    explicit Board(int dimension) {
        if (dimension < 4) {
            throw invalid_argument("Board dimension kam se kam 4 hona chahiye (mila: " +
                                   to_string(dimension) + ")");
        }
        size_ = dimension * dimension;
    }

    // Board apni saari entities ka MAALIK hai -> wahi unhe delete karta hai.
    // ⚠ Sirf `entities_` se delete karo! `entityByStart_` me WAHI pointers hain
    // (koi nayi copy nahi) — dono se delete karte to DOUBLE FREE ho jaata. 💀
    ~Board() { for (auto *entity : entities_) delete entity; }

    int getBoardSize() const { return size_; }

    // Is position pe pehle se koi entity hai kya?
    // (Ek cell pe do saanp nahi lag sakte — kaunsa chalega, confusion ho jaata.)
    bool canAddEntity(int position) { return entityByStart_.find(position) == entityByStart_.end(); }

    // ========================================================================
    //  ✅ addBoardEntity — BOARD KA PEHREDAAR (yahan 2 bug fix hue)
    // ========================================================================
    //  Pehle ye aisa tha:
    //      void addBoardEntity(BoardEntity *entity) {
    //          if (!canAddEntity(entity->getStart())) return;   // 👈 LEAK!
    //          entities_.push_back(entity);
    //          entityByStart_[entity->getStart()] = entity;
    //      }
    //
    //  🐛 BUG 1 — MEMORY LEAK: caller `new Snake(...)` karke bhejta hai. Agar
    //     position pehle se bhari thi, to ye chup-chaap `return` kar deta —
    //     aur wo `new` kiya hua object HAMESHA ke liye leak ho jaata. Kisi ne
    //     use kabhi delete nahi kiya. ✅ Ab reject pe `delete entity` hota hai.
    //
    //  🐛 BUG 2 — KOI VALIDATION NAHI THI: kuch bhi board pe chadh sakta tha —
    //     - `Snake(5, 90)`   -> saanp jo UPAR le jaaye 😵 (test kiya, chal jaata tha)
    //     - `Snake(100, 5)`  -> jeetne wale ghar (100) pe saanp! 💀 Ab koi kabhi
    //                           jeet hi nahi sakta — 100 pe pahuncho, saanp
    //                           neeche patak dega. Game me INFINITE LOOP.
    //                           (Test kiya tha: 4 second me 16 LAKH lines output.)
    //     - Board ke bahar ki positions
    //
    //  ✅ Ab return type `bool` hai — caller ko pata chalta hai ki entity lagi ya
    //     nahi. (Pehle `void` tha, to caller ko kuch pata hi nahi chalta tha!)
    //     Strategies isi return value se decide karti hain ki dobara try karein.
    bool addBoardEntity(BoardEntity *entity) {
        if (entity == nullptr) return false;

        int start = entity->getStart();
        int end = entity->getEnd();
        bool isSnake = (entity->name() == "SNAKE");

        // Saari sharten ek saath — ek bhi tooti to entity reject:
        bool valid =
            // (1) start board ke ANDAR ho, aur JEETNE WALE ghar pe NA ho.
            //     `start < size_` — yahi wo check hai jo "saanp 100 pe" wale
            //     unwinnable-game bug ko rokta hai. Aakhri ghar hamesha khaali.
            (start >= 1 && start < size_) &&

            // (2) end bhi board ke andar ho, aur jeetne wale ghar pe na ho.
            //     Seedhi seedha 100 tak na le jaaye — warna instant jeet, aur
            //     "exact roll chahiye" wala niyam bekaar ho jaata.
            (end >= 1 && end < size_) &&

            // (3) start aur end alag hon (khud pe hi le jaane wali entity bekaar hai)
            (start != end) &&

            // (4) ⭐ DISHA sahi ho — yahi wo check hai jo "ulta saanp" rokta hai:
            //        saanp  -> NEECHE (end < start)
            //        seedhi -> UPAR   (end > start)
            (isSnake ? (end < start) : (end > start)) &&

            // (5) ye position pehle se bhari na ho
            canAddEntity(start);

        if (!valid) {
            delete entity;   // ⭐ LEAK FIX — reject kiya to memory bhi saaf karo
            return false;    //    (caller ne `new` kiya tha, aur ab wo orphan hai)
        }

        entities_.push_back(entity);
        entityByStart_[start] = entity;
        return true;
    }

    // Is position pe kya hai? Kuch nahi -> nullptr.
    BoardEntity *getEntity(int position) {
        auto it = entityByStart_.find(position);
        return it == entityByStart_.end() ? nullptr : it->second;
    }

    // ========================================================================
    //  ✅ isGoalReachable() — "kya ye board JEETA bhi ja sakta hai?"
    // ========================================================================
    //  🐛 YE EK GEHRA BUG THA jo per-entity validation se PAKDA NAHI JAATA.
    //
    //  `addBoardEntity` har entity ko ALAG-ALAG check karta hai (disha sahi hai?
    //  board ke andar hai? aakhri ghar pe to nahi?). Har entity apne aap me
    //  bilkul valid ho sakti hai — aur phir bhi POORA BOARD toota ho sakta hai!
    //
    //  Asli misaal (test karke pakda):
    //     4×4 board = 16 cells. User ne 8 saanp maange.
    //     Saanp lage: 10, 11, 12, 13, 14, 15 — har ek bilkul valid! ✅
    //     (sab neeche le jaate hain, sab board ke andar, koi 16 pe nahi)
    //
    //     Par ab socho: 16 pe pahunchne ke liye kahan se chalna padega?
    //        Pasa 1..6 hai, to 16 pe pahunchne ke liye 10..15 me se kisi ghar
    //        pe khada hona padega. Par wo SAARE ghar saanp hain — wahan ruk
    //        hi nahi sakte! Aur 9 se neeche se? 9 + 6 = 15, 16 nahi. 💀
    //
    //     Natija: 16 tak pahunchne ka KOI raasta nahi. Game HAMESHA chalta
    //     rahega, koi kabhi nahi jeetega. INFINITE LOOP. 🔁
    //
    //  📌 SABAK — ye is poore lecture ka sabse bada sabak hai:
    //     **"Har hissa alag-alag sahi hai" ≠ "poora system sahi hai."**
    //     Kuch shartein sirf POORE system pe check ho sakti hain, tukdon pe nahi.
    //     Aisi shart ko INVARIANT kehte hain, aur use alag se verify karna padta
    //     hai. Yahan invariant hai: "jeet ka ghar hamesha pahunchne layak ho."
    //
    //  ⭐ HAL — ise ek GRAPH PROBLEM ki tarah dekho:
    //     Har ghar = ek node. "Yahan se wahan ja sakte ho" = ek edge.
    //     Sawaal: kya 0 se `size_` tak koi raasta hai? -> seedha BFS! 🎯
    //
    //     (LLD interview me graph algorithm nikaal dena — ye alag hi impression
    //      banata hai. Ye dikhata hai ki tum "correctness" ko seriously lete ho,
    //      sirf classes bana ke khush nahi ho jaate.)
    bool isGoalReachable(int maxDiceValue = 6) {
        // visited[p] = kya ghar `p` pe pahuncha ja sakta hai?
        vector<bool> visited(size_ + 1, false);

        queue<int> q;
        q.push(0);            // player 0 se shuru hota hai (board ke bahar)
        visited[0] = true;

        while (!q.empty()) {
            int pos = q.front();
            q.pop();

            if (pos == size_) return true;   // 🎉 jeet ka ghar mil gaya!

            // Yahan se har mumkin pasa chalo (1 se 6)
            for (int d = 1; d <= maxDiceValue; d++) {
                int next = pos + d;

                // "Exact roll" wala niyam — board se bahar nahi ja sakte.
                // (Rules::isValidMove wahi karta hai. Yahan bhi wahi logic.)
                if (next > size_) continue;

                // Us ghar pe saanp/seedhi hui? To asli landing wahan hogi.
                BoardEntity *e = getEntity(next);
                int landing = (e == nullptr) ? next : e->getEnd();

                if (!visited[landing]) {
                    visited[landing] = true;
                    q.push(landing);
                }
            }
        }

        return false;   // saare raaste ghoom liye, jeet ka ghar mila hi nahi 💀
    }

    // ---- BRIDGE ka connection point ----------------------------------------
    // Iski body yahan NAHI hai — BoardSetupBridge.h me hai. Kyun? Kyunki yahan
    // `BoardSetupStrategy` sirf forward-declared hai (uske members use nahi kar
    // sakte). Poori definition BoardSetupBridge.h me milti hai, aur wahin ye
    // function likha gaya hai. Ye circular dependency todne ka standard tareeka
    // hai. (Us file me detail me likha hai.)
    void setupBoard(BoardSetupStrategy *strategy);

    void display() {
        cout << "\n=== Board Configuration ===" << endl;

        int snakeCount = 0, ladderCount = 0;
        for (auto *entity : entities_) {
            if (entity->name() == "SNAKE") snakeCount++; else ladderCount++;
        }

        cout << "Board Size: " << size_ << " cells" << endl;
        cout << "\nSnakes: " << snakeCount << endl;
        for (auto *entity : entities_) if (entity->name() == "SNAKE") entity->display();
        cout << "\nLadders: " << ladderCount << endl;
        for (auto *entity : entities_) if (entity->name() == "LADDER") entity->display();
        cout << "=========================" << endl;
    }
};

}

#endif
