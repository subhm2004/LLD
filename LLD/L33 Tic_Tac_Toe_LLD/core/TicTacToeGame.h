// ============================================================================
//  core/TicTacToeGame.h  —  GAME ENGINE (orchestrator)
// ----------------------------------------------------------------------------
//  Ye poore game ka "conductor" hai. Khud kuch nahi karta — sabse KARWATA hai:
//
//     Board  ko bolta hai : "ye cell bhar do", "board dikha do"
//     Rules  se poochta hai: "move valid hai?", "koi jeeta?", "draw hua?"
//     Observers ko batata hai: "ye ye ho gaya"
//     Players ko ghumata hai : "ab tumhari baari"
//
//  Notice karo ki engine me GAME KE NIYAM kahin nahi likhe. "3 in a row se jeet"
//  ye baat is file me ek jagah bhi nahi hai! Wo Rules ka kaam hai. Engine sirf
//  `rules_->checkWinCondition(...)` puchta hai aur jawab maan leta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `deque` se TURN ROTATION — bahut pyaari trick hai ye                 │
//  │                                                                          │
//  │  Players ko `deque` (double-ended queue) me rakha hai — `vector` me nahi.│
//  │  Kyun? Kyunki turn ghumana bas do line ka kaam ban jaata hai:            │
//  │                                                                          │
//  │      currentPlayer = players_.front();   // pehla banda = jiski baari hai│
//  │      ...usse move karwao...                                              │
//  │      players_.pop_front();               // aage se nikalo               │
//  │      players_.push_back(currentPlayer);  // peeche daal do               │
//  │                                                                          │
//  │  Queue apne aap ghoomti rehti hai:                                       │
//  │      [Aditya, Harshita] -> [Harshita, Aditya] -> [Aditya, Harshita] ...  │
//  │                                                                          │
//  │  Koi `currentPlayerIndex` variable nahi, koi `index = (index+1) % n`     │
//  │  modulo ki jugaad nahi. Aur sabse badi baat — ye 2 players pe bhi chalta │
//  │  hai aur 5 players pe bhi, BINA ek line badle. 🎯                        │
//  │                                                                          │
//  │  `deque` isi liye chuna (vector nahi): vector me `pop_front()` hota hi   │
//  │  nahi — saare elements ko shift karna padta (O(n)). deque me dono taraf  │
//  │  se add/remove O(1) hai. Sahi data structure = simple code.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠⚠ IS FILE ME EK ASLI, KHATARNAK BUG HAI — `play()` ke andar. Neeche detail
//  me likha hai. Chhota sa lagta hai, par game ko HANG kar deta hai. Test karke
//  confirm kiya hai. Zaroor padhna.
// ============================================================================
#ifndef TICTACTOE_LLD_CORE_TICTACTOEGAME_H
#define TICTACTOE_LLD_CORE_TICTACTOEGAME_H

#include <bits/stdc++.h>

#include "../models/Board.h"
#include "../models/TicTacToePlayer.h"
#include "../observers/IObserver.h"
#include "../rules/StandardTicTacToeRules.h"

using namespace std;

namespace tictactoe_lld {

class TicTacToeGame {
public:
    // ---- ✅ RULES ab BAHAR SE aate hain (DEPENDENCY INJECTION) --------------
    //
    //  Pehle constructor aisa tha:
    //      explicit TicTacToeGame(int boardSize)
    //          : board_(new Board(boardSize)),
    //            rules_(new StandardTicTacToeRules()),   // 👈 HARDCODED!
    //            gameOver_(false) {}
    //
    //  Problem: engine KHUD apne rules bana raha tha. Iska matlab bahar se koi
    //  doosre rules PLUG kar hi nahi sakta tha. `TicTacToeRules` interface bana
    //  to diya tha, par usko use karne ka darwaza hi nahi chhoda tha. 🚪❌
    //  Yaani Strategy pattern aadha hi laga tha — dikhawa tha, kaam nahi karta tha.
    //
    //  Ab rules BAHAR SE aate hain. Isse do bade fayde:
    //
    //  1️⃣ NAYE VARIANTS ab possible hain. Factory decide karti hai kaunse rules
    //     bhejne hain (GameType dekh ke) — FourInARowRules, MisereRules, jo bhi.
    //     Engine ki ek line bhi nahi badlegi.
    //
    //  2️⃣ TESTING aasan ho gayi. Test me ek FakeRules bhej do jo hamesha "win"
    //     bole, aur engine ka flow check kar lo. Pehle ye possible hi nahi tha,
    //     kyunki rules andar hi bante the.
    //     📌 Ye connection yaad rakhna: **hardcoded dependency = untestable code.**
    //
    //  Ise DEPENDENCY INJECTION kehte hain — "apni zaroorat KHUD mat banao,
    //  bahar se MAANGO." Ek chhota sa badlav, par poora pattern zinda ho gaya.
    //
    //  ⚠ OWNERSHIP: `rules` ka pointer bahar se aata hai, par ab uska MAALIK
    //  engine ban jaata hai — destructor me wo ise delete karta hai. Caller
    //  (factory) ise delete NA kare. Aisi "ownership transfer" raw pointers me
    //  hamesha confusing hoti hai — isi liye asli code me `unique_ptr<TicTacToeRules>`
    //  lena chahiye, jo signature me hi saaf bata deta hai ki "main ise le ja raha hu".
    TicTacToeGame(int boardSize, TicTacToeRules *rules)
        : board_(new Board(boardSize)),
          rules_(rules),
          gameOver_(false) {
        // Defensive check — koi galti se nullptr bhej de to yahin pakad lo,
        // baad me `rules_->isValidMove(...)` pe crash hone se behtar hai.
        // Galti ko uske SOURCE ke paas pakdo, jahan wo phatti hai wahan nahi.
        if (rules_ == nullptr) {
            delete board_;   // apna banaya hua board saaf karo, warna leak
            throw invalid_argument("TicTacToeGame: rules nullptr nahi ho sakte");
        }
    }

    ~TicTacToeGame() {
        // Engine board aur rules ka MAALIK hai (dono usne `new` kiye the) ->
        // dono delete karega.
        //
        // Players aur observers ko delete NAHI karta — wo bahar (main.cpp me)
        // bane the, to unka maalik main.cpp hai. Engine unhe sirf "point" karta
        // hai. 📌 "Jo banata hai, wahi delete karta hai."
        delete board_;
        delete rules_;
    }

    void addPlayer(TicTacToePlayer *player) { players_.push_back(player); }
    void addObserver(IObserver *observer) { observers_.push_back(observer); }

    // ---- OBSERVER PATTERN ka dil ---------------------------------------------
    // Saare registered observers ko khabar kar do. Engine ko pata hi nahi ki
    // andar kaun hai — ConsoleNotifier, FileLogger, ya kuch bhi. Bas `update()`
    // bulata hai aur aage badh jaata hai.
    //
    // Khaali list? Koi baat nahi — loop bas nahi chalega, game normal chalta
    // rahega. Engine observers pe depend nahi karta.
    void notify(const string &message) {
        for (auto *observer : observers_) {
            observer->update(message);
        }
    }

    // ========================================================================
    //  play() — POORA GAME LOOP. Yahi is class ka asli kaam hai.
    // ========================================================================
    void play() {
        if (players_.size() < 2) {
            cout << "Need at least 2 players!\n";
            return;
        }

        notify("Tic Tac Toe Game Started!");

        while (!gameOver_) {
            board_->display();

            // ---- Kiski baari? -> queue ka pehla banda -----------------------
            // `front()` sirf DEKHTA hai, nikalta nahi. Nikalna tabhi hai jab
            // move safal ho jaaye (neeche pop_front/push_back dekho). Isi liye
            // galat move karne wale ki baari NAHI jaati — usko dobara mauka
            // milta hai. Bilkul sahi behavior! ✅
            TicTacToePlayer *currentPlayer = players_.front();
            cout << currentPlayer->getName() << " (" << currentPlayer->getSymbol()->getMark()
                 << ") - Enter row and column: ";

            int row, col;

            // ---- ✅ INPUT VALIDATION — yahan pehle INFINITE LOOP wala bug tha ----
            //
            //  Pehle seedha `cin >> row >> col;` likha tha, bina koi check ke.
            //  Problem: `cin` FAIL ho sakta hai, aur fail hone par:
            //     - row/col me KACHRA (uninitialized garbage) reh jaata tha
            //     - galat input buffer me ATKA reh jaata tha (hata nahi)
            //     - agli baar `cin >> row` turant fail hota (buffer wahi hai!)
            //     - ...aur ye HAMESHA ke liye chalta rehta 🔁
            //  Natija: "Invalid move! Try again." ki barsaat. 🌊
            //  (Test kiya tha: 3 SECOND me 6,85,913 baar print hua.)
            //
            //  Ab `cin` ka STATE check karte hain. `cin >> row >> col` ek stream
            //  return karta hai, jise `bool` ki tarah use kiya ja sakta hai —
            //  padhna safal hua to `true`, fail hua to `false`.
            if (!(cin >> row >> col)) {

                // ---- Case 1: input hi khatam ho gaya (Ctrl+D, ya file se input) --
                // Yahan `continue` karne ka koi fayda nahi — aage input hai hi
                // nahi, to loop hamesha ke liye ghoomta rahega. Bahar nikalna
                // hi ekmatr sahi jawab hai.
                if (cin.eof()) {
                    cout << "\nInput khatam ho gaya. Game band kar rahe hain.\n";
                    notify("Game abandoned (input ended).");
                    return;
                }

                // ---- Case 2: user ne number ki jagah kuch aur type kiya ("abc") --
                // Do line me hi poora ilaaj hai:
                cin.clear();   // (a) fail flag hatao — warna cin hamesha "toota"
                               //     hi rahega aur aage koi bhi read fail hoga

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                               // (b) kachra input BUFFER SE PHENKO — agli newline
                               //     tak sab kuch nigal jao. Ye step bhoolna sabse
                               //     common galti hai: `clear()` sirf flag hatata
                               //     hai, ganda data buffer me PADA REHTA hai. Use
                               //     nikala nahi to agla read phir wahi "abc"
                               //     padhega aur phir fail hoga. Infinite loop wapas. 🔁

                cout << "Galat input! Do number daalo (jaise: 1 2)\n";
                continue;      // (c) dobara maango — turn NAHI ghumaya, kyunki
                               //     move hua hi nahi. Usi player ko phir mauka.
            }

            if (rules_->isValidMove(board_, row, col)) {
                // ---- Move safal — board update karo -------------------------
                board_->placeMark(row, col, currentPlayer->getSymbol());
                notify(currentPlayer->getName() + " played (" + to_string(row) + "," + to_string(col) + ")");

                // ---- ⭐ ORDER MATTER KARTA HAI: pehle WIN, phir DRAW ---------
                // Kyun? Aakhri cell bharne se agar koi JEET gaya, to board bhi
                // POORA bhar gaya hai. Yaani dono conditions ek saath true hain!
                //
                // Agar draw pehle check karte, to jeeti hui game "draw" batati. 💀
                // Win pehle check karne se sahi jawab milta hai. ✅
                //
                // (Yahi wajah hai ki StandardTicTacToeRules::checkDrawCondition
                //  sirf "board bhara?" check karta hai — usko maloom hai ki win
                //  pehle hi check ho chuka hoga. Ye "chhupi hui shart" hai, aur
                //  isi liye khatarnak bhi — us file me detail me likha hai.)
                if (rules_->checkWinCondition(board_, currentPlayer->getSymbol())) {
                    board_->display();
                    cout << currentPlayer->getName() << " wins!\n";
                    currentPlayer->incrementScore();
                    notify(currentPlayer->getName() + " wins!");
                    gameOver_ = true;   // loop toot jaayega
                }
                else if (rules_->checkDrawCondition(board_)) {
                    board_->display();
                    cout << "It's a draw!\n";
                    notify("Game is Draw!");
                    gameOver_ = true;
                }
                else {
                    // ---- Game jaari hai -> TURN ROTATE karo ------------------
                    // Ye 2 line hi poora turn management hai. Aage se nikala,
                    // peeche daal diya. Queue apne aap ghoom gayi.
                    //
                    // ⭐ Aur ye rotation SIRF safal move ke baad hota hai —
                    // `else` block ke andar hai. Galat move karne wale ki baari
                    // nahi jaati. Ye behavior "free" me mil gaya, alag se koi
                    // logic likhna hi nahi pada. Sahi jagah code rakhne ka fayda.
                    players_.pop_front();
                    players_.push_back(currentPlayer);
                }
            }
            else {
                // Cell bhari hui hai, ya index board ke bahar hai.
                // (Board ka `isCellEmpty` out-of-bounds pe `false` deta hai, isi
                //  liye yahan alag se bounds check nahi karna pada.)
                //
                // ⭐ Notice: turn rotate NAHI hua. Usi player ko dobara mauka
                // milega. Bilkul sahi hai.
                cout << "Invalid move! Try again.\n";
            }
        }
    }

private:
    Board *board_;                       // engine ka apna (delete karta hai)
    deque<TicTacToePlayer *> players_;   // rotating queue — front = jiski baari hai
    TicTacToeRules *rules_;              // ✅ bahar se INJECT hota hai, par maalik
                                         //    engine hi hai (destructor delete karta hai)
    vector<IObserver *> observers_;      // bahar ke (delete NAHI karta)
    bool gameOver_;                      // loop kab tootega
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_CORE_TICTACTOEGAME_H
