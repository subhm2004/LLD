// ============================================================================
//  models/Board.h  —  N × N grid: cell bharo, khaali check karo, display karo
// ----------------------------------------------------------------------------
//  Board sirf ek DUMB GRID hai — ek "data structure", "brain" nahi.
//    Board JAANTA hai   : "cell (1,2) me kya pada hai?"  "board bhara hai kya?"
//    Board NAHI JAANTA  : "kaun jeeta?"  "kiski baari hai?"  "move valid hai?"
//
//  Wo saare sawaal Rules aur Game ke hain. Board unse poochta bhi nahi.
//  Ye SEPARATION OF CONCERNS hai — aur isi wajah se Rules badalne par (jaise
//  4-in-a-row, ya wildcard) Board ko CHHUNA hi nahi padta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `emptyCell_` — SENTINEL OBJECT ki trick. Is file ka dil yahi hai.    │
//  │                                                                          │
//  │  Khaali cell ko dikhane ke 2 tareeke the:                                │
//  │     (a) nullptr rakho          -> har jagah `if (cell == nullptr)` check │
//  │                                    karna padta, aur ek bhi bhoolne pe    │
//  │                                    seedha CRASH (null pointer deref).    │
//  │     (b) ek asli Symbol('-') ka object banao, aur saare khaali cells ko   │
//  │         USI EK object pe point karwa do. ✅ Ye chuna gaya hai.           │
//  │                                                                          │
//  │  Fayda: grid me KABHI nullptr aata hi nahi. Har cell me hamesha ek valid │
//  │  Symbol* hota hai. To `grid_[i][j]->getMark()` hamesha safe hai — koi    │
//  │  null check kahin nahi chahiye. display() dekho: seedha ->getMark()      │
//  │  bulata hai, bina daray.                                                 │
//  │                                                                          │
//  │  Aur "khaali hai kya?" ka jawab? Bas pointer compare:                    │
//  │       grid_[row][col] == emptyCell_                                      │
//  │  Ek hi object hai, to pointer compare kaafi hai. Fast bhi, saaf bhi.     │
//  │                                                                          │
//  │  💡 Ise NULL OBJECT PATTERN kehte hain — "kuch nahi hai" ko bhi ek asli  │
//  │  object bana do, taaki special-case handling ki zaroorat hi na pade.     │
//  │  (L30 ka Flyweight yaad karo — ek hi object, N jagah shared. Wahi idea.) │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef TICTACTOE_LLD_MODELS_BOARD_H
#define TICTACTOE_LLD_MODELS_BOARD_H

#include <bits/stdc++.h>

#include "Symbol.h"

using namespace std;

namespace tictactoe_lld {

class Board {
public:
    explicit Board(int size) : size_(size), emptyCell_(nullptr) {
        // ---- ✅ SIZE GUARD — pehle ye check tha hi nahi 🐛 -------------------
        //
        //  Pehle kya hota tha:
        //    size = 0   -> khaali grid. Board ban to jaata, par har move invalid
        //                  hota (koi cell hi nahi) -> game me INFINITE LOOP. 🔁
        //                  (Test kiya tha: 3 second me 29 LAKH "Invalid move!")
        //
        //    size = -1  -> `vector<Symbol*>(-1, ...)` me -1 `size_t` me convert
        //                  hota (~18 quintillion!) -> `std::length_error` throw
        //                  -> koi catch nahi karta -> program CRASH. 💥
        //
        //  ⭐ Ye check `new Symbol('-')` se PEHLE hona chahiye — isi liye
        //  `emptyCell_(nullptr)` se shuru kiya aur allocation neeche shift ki.
        //  Agar pehle allocate karte aur phir throw karte, to wo Symbol LEAK ho
        //  jaata (destructor chalta hi nahi jab constructor beech me throw kare!).
        //  📌 Rule: "fail fast" — kuch bhi allocate karne se PEHLE validate karo.
        //     Tab cleanup ki jhanjhat hoti hi nahi.
        //
        //  3 se kam kyun mana? 1×1 pe to pehla move hi jeet jaata hai 😄 aur 2×2
        //  hamesha pehle player ki jeet hai. Tic-tac-toe 3×3 se hi shuru hota hai.
        if (size < 3) {
            throw invalid_argument("Board size kam se kam 3 hona chahiye (mila: " +
                                   to_string(size) + ")");
        }

        emptyCell_ = new Symbol('-');

        // Poori grid ko emptyCell_ se bhar do. Dhyaan do: N×N alag-alag objects
        // NAHI ban rahe — saare cells EK HI object ko point kar rahe hain.
        // 100×100 board pe bhi khaali cells ke liye sirf 1 Symbol object banega.
        grid_ = vector<vector<Symbol *>>(size_, vector<Symbol *>(size_, emptyCell_));

        // 💡 Ye guard Factory me bhi hai (dono jagah). Zyada nahi hai — ise
        // "defense in depth" kehte hain. Board ko KHUD apne aap ko galat state
        // me banne se rokna chahiye, chahe caller kitna bhi lapervah ho. Aaj
        // sirf Factory se ban raha hai; kal koi seedha `new Board(0)` likh dega.
    }

    // Sirf `emptyCell_` delete hota hai — jo Board ne KHUD banaya tha.
    // Grid me pade players ke symbols ko HAATH NAHI LAGATA — unke maalik
    // TicTacToePlayer hain, wahi unhe delete karenge.
    // 📌 "Jo banata hai, wahi delete karta hai." Board ne sirf emptyCell banaya.
    ~Board() { delete emptyCell_; }

    // ---- Cell khaali hai kya? ------------------------------------------------
    bool isCellEmpty(int row, int col) const {
        // ⭐ BOUNDS CHECK — bahar ka index bheja to `false` (yaani "khaali nahi").
        //
        // Ye chalaki hai: out-of-bounds ko "bhara hua" bata dena. Isse
        // `isValidMove()` apne aap `false` de deta hai, aur game "Invalid move!"
        // bol ke aage badh jaata hai. Na crash, na exception, na alag se koi
        // bounds-check likhna pada. Ek check, do problem hal. ✅
        //
        // (User ne (5,5) daala 3×3 board pe? Bas "Invalid move! Try again.")
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return false;
        }
        // Pointer compare — sentinel wali trick (upar padho).
        return grid_[row][col] == emptyCell_;
    }

    // ---- Cell me nishaan lagao -----------------------------------------------
    bool placeMark(int row, int col, Symbol *symbol) {
        // Dobara bounds check. Zaroori hai — ye function PUBLIC hai, koi bhi
        // ise seedha bula sakta hai (bina isValidMove ke). Har public function
        // ko apni suraksha khud karni chahiye, ye maan ke nahi chalna chahiye ki
        // "caller ne pehle check kar liya hoga".
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return false;
        }
        // Bhare hue cell pe dobara mark nahi laga sakte — cheating rok di. 🛡
        if (!isCellEmpty(row, col)) {
            return false;
        }

        // Bas pointer copy — koi naya object nahi banta. Player ka symbol object
        // ab is cell me bhi "dikh" raha hai. Ek hi 'X' object poore board me
        // 5 jagah ho sakta hai. (Isi liye Symbol immutable hona zaroori tha!)
        grid_[row][col] = symbol;
        return true;

        // 💡 Return value `bool` hai (safal hua ya nahi), par TicTacToeGame ise
        // IGNORE kar deta hai — kyunki wo pehle hi `isValidMove()` se check kar
        // chuka hota hai. Thoda redundant hai, par safe hai.
    }

    // ---- Cell padho ---------------------------------------------------------
    Symbol *getCell(int row, int col) const {
        // ⭐ Out-of-bounds pe `emptyCell_` lauta do, nullptr nahi!
        //
        // Sentinel pattern ka poora fayda yahin dikhta hai: caller ko kabhi
        // null check karna hi nahi padta. Rules ka code dekho —
        //     if (board->getCell(i, j) != symbol) ...
        // — bina kisi null check ke seedha compare. Aur wo hamesha safe hai,
        // chahe index kuch bhi ho. Ye "fail-safe" design hai. ✅
        if (row < 0 || row >= size_ || col < 0 || col >= size_) {
            return emptyCell_;
        }
        return grid_[row][col];
    }

    int getSize() const { return size_; }

    // Rules ko chahiye — "ye cell khaali hai kya?" compare karne ke liye.
    // (checkDrawCondition me use hota hai.)
    Symbol *getEmptyCell() const { return emptyCell_; }

    // ---- Board print karo ---------------------------------------------------
    //    0 1 2
    //  0 X X X
    //  1 O O -
    //  2 - - -
    void display() const {
        // Pehli line: column numbers (0, 1, 2, ...) — user ko coordinates
        // dikhein taaki wo sahi row/col type kar sake.
        cout << "\n  ";
        for (int i = 0; i < size_; i++) {
            cout << i << " ";
        }
        cout << "\n";

        for (int i = 0; i < size_; i++) {
            cout << i << " ";   // har line ke shuru me row number
            for (int j = 0; j < size_; j++) {
                // Yahan koi null check NAHI hai — aur nahi chahiye bhi. Khaali
                // cells me emptyCell_ pada hai, jiska mark '-' hai. Sentinel
                // pattern ka seedha fayda. ✅
                cout << grid_[i][j]->getMark() << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

private:
    // ⚠ Chhota sa C++ trap notice karo:
    // Members apni DECLARATION order me initialize hote hain, init-list ki order
    // me nahi! Yahan grid_ pehle declare hua hai, par constructor ki init-list me
    // wo hai hi nahi — use BODY me assign kiya gaya hai. Isi liye sab theek hai:
    // jab tak body chalti hai, size_ aur emptyCell_ set ho chuke hote hain.
    //
    // Agar grid_ ko bhi init-list me daalte (`grid_(size, ...)`), to wo size_ se
    // PEHLE initialize hota — aur size_ me tab kachra hota! Classic bug. 🐛
    vector<vector<Symbol *>> grid_;
    int size_;
    Symbol *emptyCell_;   // wo EKLAUTA "khaali" object jo saare khaali cells share karte hain
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_BOARD_H
