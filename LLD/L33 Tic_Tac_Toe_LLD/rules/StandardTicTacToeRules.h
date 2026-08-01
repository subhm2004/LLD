// ============================================================================
//  rules/StandardTicTacToeRules.h  —  CONCRETE STRATEGY: aam tic-tac-toe ke niyam
// ----------------------------------------------------------------------------
//  Jeetne ke 4 tareeke — kisi bhi EK me poori line ek hi symbol se bhari ho:
//
//     ROW          COLUMN        MAIN DIAGONAL   ANTI DIAGONAL
//     X X X        X - -         X - -           - - X
//     - - -        X - -         - X -           - X -
//     - - -        X - -         - - X           X - -
//
//  N×N board pe ye N rows + N columns + 2 diagonals = kul (2N + 2) lines hain.
//  Har line ko check karna padta hai. 3×3 pe 8 lines, 5×5 pe 12 lines.
//
//  ⏱ COMPLEXITY: O(N²) har move pe (har line ke N cells, aur ~2N lines).
//    3×3 pe ye kuch bhi nahi. Par 1000×1000 board pe har move pe 10 lakh cells
//    padhna padta — bekaar hai.
//    Behtar tareeka: sirf ABHI wale move ki row/col/diagonal check karo. Kyunki
//    jeet sirf USI line pe ban sakti hai jahan abhi mark laga hai! Baaki lines
//    to pehle se hi check ho chuki thi. Isse O(N²) se O(N) ho jaata.
//    (Interview me ye optimization batana accha impression banata hai.)
//
//  ✅ MARK (VALUE) SE COMPARE — pehle yahan pointer compare hota tha
//    Pehle: `board->getCell(i, j) != symbol`             (POINTER compare)
//    Ab:    `board->getCell(i, j)->getMark() != mark`    (VALUE compare)
//
//    Purana wala CHAL raha tha (har player ka ek hi Symbol object hota hai, to
//    "same pointer" = "same player"), par bahut NAZUK tha: jis din koi doosra
//    `new Symbol('X')` bana deta, dono ka mark 'X' hota par pointer alag — aur
//    jeet KABHI detect hi nahi hoti. 💀 Ab value se compare hota hai, jo
//    logically sahi hai. Detail neeche `checkWinCondition` me likhi hai.
// ============================================================================
#ifndef TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H
#define TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H

#include <bits/stdc++.h>

#include "TicTacToeRules.h"

using namespace std;

namespace tictactoe_lld {

class StandardTicTacToeRules : public TicTacToeRules {
public:
    // ---- 1. Move valid hai? -------------------------------------------------
    // Bas itna: cell khaali honi chahiye.
    //
    // Out-of-bounds ka kya? Wo Board khud sambhal leta hai — `isCellEmpty()`
    // bahar ke index pe `false` lauta deta hai (yaani "khaali nahi hai"), to ye
    // apne aap invalid ban jaata hai. Isi liye yahan bounds check likhne ki
    // zaroorat NAHI padi. Board ka fail-safe design kaam aa gaya. ✅
    bool isValidMove(Board *board, int row, int col) override {
        return board->isCellEmpty(row, col);
    }

    // ---- 2. Kya ye symbol jeet gaya? ----------------------------------------
    bool checkWinCondition(Board *board, Symbol *symbol) override {
        int size = board->getSize();

        // ---- ✅ Guard 1: null symbol se koi nahi jeet sakta ------------------
        if (symbol == nullptr) {
            return false;
        }

        // ---- ✅ Guard 2: KHAALI symbol ('-') se koi nahi jeet sakta! ---------
        // Ye check ab ZAROORI ho gaya hai. Kyun? Kyunki neeche hum ab MARK se
        // compare karte hain (pointer se nahi — neeche padho). Agar kisi player
        // ka symbol galti se '-' hota (jo Board ka khaali-cell mark hai), to
        // khaali board hi uski "jeet" ban jaata — poori khaali row match kar
        // jaati! 😱 Ye guard wo raasta band kar deta hai.
        //
        // 📌 Ek bug fix karte waqt hamesha poochho: "isse koi NAYA raasta to nahi
        //    khul gaya?" Pointer-compare me ye problem thi hi nahi (khaali cell ka
        //    pointer alag tha). Value-compare pe aate hi ye naya khatra paida ho
        //    gaya — aur usko yahin band karna padta hai.
        char mark = symbol->getMark();
        if (mark == board->getEmptyCell()->getMark()) {
            return false;
        }

        // ⭐ AB POINTER NAHI, MARK (VALUE) SE COMPARE KARTE HAIN
        //
        //   Pehle aisa tha:  `board->getCell(i, j) != symbol`        (pointer)
        //   Ab aisa hai:     `board->getCell(i, j)->getMark() != mark` (value)
        //
        //   Purana wala CHAL to raha tha — kyunki har player ke paas theek EK
        //   Symbol object hai, aur board me wahi pointer store hota hai. To
        //   "same pointer" = "same player". ✅
        //
        //   PAR wo bahut NAZUK tha 💀 — jis din koi doosra `new Symbol('X')`
        //   bana deta (do jagah se player banaya, ya symbol copy kiya), us din
        //   dono ka mark 'X' hota par POINTER alag hote — aur jeet KABHI detect
        //   hi nahi hoti. Game hamesha draw pe khatam hota, aur wajah samajh hi
        //   nahi aati.
        //
        //   Ab value se compare karte hain — jo LOGICALLY sahi hai. "Do 'X' ek
        //   jaise hain" — chahe wo alag object hi kyun na ho. Identity nahi,
        //   VALUE maayne rakhti hai. 🎯

        // ---- (a) SAARI ROWS check karo ----
        // Har row i ke liye: kya uske SAARE cells me yahi mark hai?
        for (int i = 0; i < size; i++) {
            bool rowWin = true;      // maan lo ye row jeet hai...
            for (int j = 0; j < size; j++) {
                if (board->getCell(i, j)->getMark() != mark) {
                    rowWin = false;  // ...ek bhi cell match nahi kiya -> nahi hai
                    break;           // aage dekhne ka fayda nahi, agli row pe jao
                }
            }
            if (rowWin) {
                return true;         // ek row mil gayi -> JEET! Baaki check bekaar
            }
        }

        // ---- (b) SAARE COLUMNS check karo ----
        // Bilkul rows jaisa hi, bas i aur j ki jagah ULAT gayi.
        // Notice: bahar wala loop `j` (column) hai, andar wala `i` (row).
        for (int j = 0; j < size; j++) {
            bool colWin = true;
            for (int i = 0; i < size; i++) {
                if (board->getCell(i, j)->getMark() != mark) {   // (i,j) — i badal raha hai
                    colWin = false;
                    break;
                }
            }
            if (colWin) {
                return true;
            }
        }

        // ---- (c) MAIN DIAGONAL — upar-baayen se neeche-daayen ----
        //    X - -        cells: (0,0), (1,1), (2,2)
        //    - X -        yaani hamesha row == col
        //    - - X        isi liye getCell(i, i) — ek hi loop kaafi hai
        bool mainDiagonalWin = true;
        for (int i = 0; i < size; i++) {
            if (board->getCell(i, i)->getMark() != mark) {
                mainDiagonalWin = false;
                break;
            }
        }
        if (mainDiagonalWin) {
            return true;
        }

        // ---- (d) ANTI DIAGONAL — upar-daayen se neeche-baayen ----
        //    - - X        cells: (0,2), (1,1), (2,0)
        //    - X -        formula: col = size - 1 - row
        //    X - -        check karo: i=0 -> col=2 ✓   i=1 -> col=1 ✓   i=2 -> col=0 ✓
        bool antiDiagonalWin = true;
        for (int i = 0; i < size; i++) {
            if (board->getCell(i, size - 1 - i)->getMark() != mark) {
                antiDiagonalWin = false;
                break;
            }
        }

        // Aakhri check hai, to seedha return kar do — `if (x) return true;`
        // likhne ki zaroorat nahi. Agar ye bhi false hai, to koi jeet nahi.
        return antiDiagonalWin;
    }

    // ---- 3. Draw hua? -------------------------------------------------------
    // Draw = board POORA bhara hai AUR koi jeeta nahi.
    //
    // ⭐ Par yahan "koi jeeta nahi" check hi nahi hai! Sirf "board bhara hai"
    // dekha ja raha hai. Ye BUG nahi hai — kyunki ENGINE hamesha PEHLE win check
    // karta hai, phir draw (TicTacToeGame::play() dekho):
    //     if (checkWinCondition(...))       { ...jeet... }
    //     else if (checkDrawCondition(...)) { ...draw... }
    // To yahan tak pahunchne ka matlab hi hai ki koi jeeta nahi. ✅
    //
    // ⚠ PAR ye ek CHHUPI HUI SHART hai — is class ka sahi chalna ENGINE ke
    // call order pe depend karta hai! Agar kal koi engine me order ulta kar de
    // (draw pehle, win baad me), to jeeti hui game bhi "draw" batayegi. 💀
    // Aisi "implicit coupling" khatarnak hoti hai. Kam se kam ek comment ya
    // assert hona chahiye — ya function ka naam hi `isBoardFull()` rakh dete,
    // jo ki ye ASAL ME kar raha hai. Naam sach bolna chahiye.
    bool checkDrawCondition(Board *board) override {
        int size = board->getSize();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                // Ek bhi khaali cell mili -> game abhi baaki hai
                if (board->getCell(i, j) == board->getEmptyCell()) {
                    return false;
                }
            }
        }
        return true;   // poora board bhara, aur (engine ke hisaab se) koi jeeta nahi -> DRAW

        // 💡 Optimization: har move pe poora board scan karna (O(N²)) bekaar hai.
        // Game ek `moveCount` rakh sakta tha aur bas `moveCount == N*N` check
        // karta — O(1) me draw pata chal jaata.
    }
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_RULES_STANDARDTICTACTOERULES_H
