// ============================================================================
//  core/SnakeAndLadderGame.h  —  GAME ENGINE (orchestrator)
// ----------------------------------------------------------------------------
//  Poore game ka "conductor". Khud kuch nahi karta — sabse KARWATA hai:
//
//     Dice   se poochta hai  : "kitna aaya?"
//     Rules  se poochta hai  : "chaal valid hai? kahan pahuncha? jeet gaya?"
//     Board  se poochta hai  : "is ghar pe saanp hai ya seedhi?"
//     Observers ko batata hai: "ye ye ho gaya"
//     Players ko ghumata hai : "ab tumhari baari"
//
//  ⭐ Notice: engine me khel ke NIYAM kahin nahi likhe. "Exact roll chahiye"
//  wali baat is file me ek jagah bhi nahi hai! Wo Rules ka kaam hai. Engine bas
//  `rules_->isValidMove(...)` puchta hai aur jawab maan leta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `deque` se TURN ROTATION — L33 wali hi pyaari trick                  │
//  │                                                                          │
//  │      currentPlayer = players_.front();   // pehla banda = jiski baari    │
//  │      ...uski chaal chalwao...                                            │
//  │      players_.pop_front();               // aage se nikalo               │
//  │      players_.push_back(currentPlayer);  // peeche daal do               │
//  │                                                                          │
//  │  Queue apne aap ghoomti rehti hai:                                       │
//  │      [A, B, C] -> [B, C, A] -> [C, A, B] -> [A, B, C] ...                │
//  │                                                                          │
//  │  Koi `currentPlayerIndex`, koi `(i+1) % n` ki jugaad nahi. Aur ye 2      │
//  │  players pe bhi chalta hai aur 8 pe bhi — bina ek line badle. 🎯         │
//  │                                                                          │
//  │  `deque` isi liye (vector nahi): vector me `pop_front()` hota hi nahi —  │
//  │  saare elements shift karne padte (O(n)). deque me dono taraf O(1).      │
//  │  Sahi data structure chuno, code khud simple ho jaata hai.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SNAKE & LADDER vs TIC-TAC-TOE ka sabse bada farak:
//     Tic-tac-toe me har chaal me SKILL hai — kahan mark lagana, tum sochte ho.
//     Yahan? Kuch bhi nahi! 😄 Bas pasa phenko aur kismat dekho. Isi liye is
//     game me "Enter dabao" ke alawa koi input hi nahi hai. Poora khel kismat
//     ka hai — aur design me bhi ye dikhta hai: koi "move validation" ki
//     zaroorat hi nahi, kyunki player kuch CHUNTA hi nahi.
// ============================================================================
#ifndef SNAKE_LADDER_LLD_CORE_GAME_H
#define SNAKE_LADDER_LLD_CORE_GAME_H

#include <bits/stdc++.h>

#include "../models/Dice.h"
#include "../models/SnakeAndLadderPlayer.h"
#include "../observers/IObserver.h"
#include "../rules/SnakeAndLadderRules.h"

using namespace std;

namespace snake_ladder_lld {

class SnakeAndLadderGame {
private:
    Board *board_;                            // engine ka apna (delete karta hai)
    Dice *dice_;                              // engine ka apna (delete karta hai)
    deque<SnakeAndLadderPlayer *> players_;   // rotating queue — front = jiski baari
    SnakeAndLadderRules *rules_;              // engine ka apna (delete karta hai)
    vector<IObserver *> observers_;           // BAHAR ke (delete NAHI karta!)
    bool gameOver_;

public:
    SnakeAndLadderGame(Board *board, Dice *dice)
        : board_(board),
          dice_(dice),
          rules_(new StandardSnakeAndLadderRules()),   // ⚠ dekho neeche
          gameOver_(false) {}

    // ⚠ STRATEGY ADHOORA — rules yahan HARDCODE ho gaye (L33 wali hi kami).
    //    `new StandardSnakeAndLadderRules()` — engine KHUD apne rules bana raha
    //    hai, to bahar se koi doosre rules (BounceBackRules, DoubleSixRules)
    //    PLUG kar hi nahi sakta. Interface bana diya, par darwaza nahi chhoda. 🚪❌
    //
    //    FIX hota (bilkul waise hi jaise L33 me kiya tha):
    //        SnakeAndLadderGame(Board *board, Dice *dice, SnakeAndLadderRules *rules)
    //            : board_(board), dice_(dice), rules_(rules), gameOver_(false) {}
    //    Aur Factory decide kare kaunse rules bhejne hain.
    //
    //    Ise abhi jaan-boojh ke NAHI badla — L34 ka focus BRIDGE pattern pe hai,
    //    aur board-setup wala hissa pehle se hi sahi tarah inject hota hai. Ye
    //    ek accha exercise hai: khud fix karo, L33 ke pattern ko follow karke. 🎯

    ~SnakeAndLadderGame() {
        delete rules_;
        delete board_;
        delete dice_;

        // ⭐ Players ko engine DELETE karta hai! (L33 se ULTA — wahan main.cpp
        // karta tha.) Kyun? Kyunki main.cpp me players seedha `addPlayer(new ...)`
        // me bana ke de diye jaate hain — main.cpp unka pointer rakhta hi nahi!
        // To delete karne ka koi raasta hi nahi bachta. Ownership engine ko hi
        // deni padi.
        //
        // 📌 Ye "ownership transfer" hai. Raw pointers me ye HAMESHA confusing
        //    rehta hai — signature dekh ke pata hi nahi chalta ki `addPlayer`
        //    pointer LE JA raha hai ya sirf DEKH raha hai. `unique_ptr` se ye
        //    saaf ho jaata:  void addPlayer(unique_ptr<SnakeAndLadderPlayer> p);
        //    Signature khud bol deta: "main ise le ja raha hu."
        for (auto *p : players_) delete p;

        // Observers ko delete NAHI karte — wo main.cpp me bane the, wahi karega.
        // "Jo banata hai wahi delete karta hai" — bas yahan players wala case ulta
        // hai, kyunki wahan banane ke turant baad ownership de di gayi thi.
    }

    void addPlayer(SnakeAndLadderPlayer *player) { players_.push_back(player); }
    void addObserver(IObserver *observer) { observers_.push_back(observer); }

    // OBSERVER ka dil — sabko khabar kar do. Engine ko pata hi nahi ki andar kaun
    // hai (console notifier? file logger?). Bas `update()` bulata hai.
    void notify(const string &message) { for (auto *o : observers_) o->update(message); }

    void displayPlayerPositions() {
        cout << "\n=== Current Positions ===" << endl;
        for (auto *player : players_) cout << player->getName() << ": " << player->getPosition() << endl;
        cout << "======================" << endl;
    }

    // ========================================================================
    //  play() — POORA GAME LOOP
    // ========================================================================
    void play() {
        if (players_.size() < 2) { cout << "Need at least 2 players!" << endl; return; }

        notify("Game started");
        board_->display();

        // ⭐ Ye line zaroori hai! Isse pehle main.cpp me `cin >> name` chala tha,
        // jo newline ('\n') ko buffer me CHHOD deta hai. Agar ise saaf na karein,
        // to neeche wala `cin.get()` us bachi hui newline ko hi "Enter" samajh
        // lega — aur pehla player bina Enter dabaye hi chaal chal dega! 😅
        //
        // 📌 `cin >> x` newline chhod deta hai, `cin.get()`/`getline()` use padh
        //    lete hain. In dono ko mix karo to hamesha ye ignore() lagana padta
        //    hai. C++ ka classic dard.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        while (!gameOver_) {
            // ---- Kiski baari? -> queue ka pehla banda ----------------------
            SnakeAndLadderPlayer *currentPlayer = players_.front();
            cout << "\n" << currentPlayer->getName() << "'s turn. Press Enter to roll dice...";
            cin.get();
            // 💡 Yahan input FAIL hone ka koi khatra nahi hai (L33 wale bug jaisa).
            // Kyunki hum kuch PADH nahi rahe — bas Enter ka intezaar kar rahe hain.
            // Input khatam (EOF) ho jaaye to `cin.get()` turant lautta hai aur game
            // KHUD-B-KHUD chalta rehta hai (pasa to random hai, user ka input to
            // chahiye hi nahi!) — aur jeet ke saath khatam ho jaata hai. Test kiya. ✅

            int diceValue = dice_->roll();
            cout << "Rolled: " << diceValue << endl;

            int currentPos = currentPlayer->getPosition();

            if (rules_->isValidMove(currentPos, diceValue, board_->getBoardSize())) {

                // ⭐ DO alag positions — ye farak samajhna zaroori hai:
                //    `intermediate` = pasa ke hisaab se jahan GIRA    (62)
                //    `newPos`       = saanp/seedhi ke BAAD jahan hai  (19)
                //
                // Dono chahiye:
                //    newPos       -> player ki asli nayi jagah (jahan wo ab hai)
                //    intermediate -> ye BATANE ke liye ki saanp/seedhi KAHAN mili
                //                    ("Snake at 62!" — 19 nahi, 62 bolna hai!)
                int intermediate = currentPos + diceValue;
                int newPos = rules_->calculateNewPosition(currentPos, diceValue, board_);
                currentPlayer->setPosition(newPos);

                // Us ghar pe kuch tha kya? Ho to user ko batao.
                BoardEntity *entity = board_->getEntity(intermediate);
                if (entity != nullptr) {
                    // ⚠ String compare se faisla — thoda kaccha hai. Ek typo
                    // ("SNAKE " ya "Snake") aur logic chup-chaap galat ho jaayega,
                    // compiler kuch nahi bolega. Behtar: `entity->isSnake()` ya
                    // entity khud apna message banaye. (BoardEntity.h me likha hai.)
                    if (entity->name() == "SNAKE") {
                        cout << "Oh no! Snake at " << intermediate << "! Going down to " << newPos << endl;
                    } else {
                        cout << "Great! Ladder at " << intermediate << "! Going up to " << newPos << endl;
                    }
                }

                notify(currentPlayer->getName() + " played. New Position : " + to_string(newPos));
                displayPlayerPositions();

                if (rules_->checkWinCondition(newPos, board_->getBoardSize())) {
                    cout << "\n" << currentPlayer->getName() << " wins!" << endl;
                    currentPlayer->incrementScore();
                    notify("Game Ended. Winner is : " + currentPlayer->getName());
                    gameOver_ = true;   // loop toot jaayega
                } else {
                    // Game jaari — turn rotate karo (aage se nikalo, peeche daalo)
                    players_.pop_front();
                    players_.push_back(currentPlayer);
                }

            } else {
                // ---- "EXACT ROLL CHAHIYE" — chaal reject ----------------------
                // 97 pe ho, 5 aaya -> 102 board se bahar -> chaal nahi chalegi.
                // Wahi ke wahi ruko, aur baari agle ko de do.
                //
                // ⭐ Notice: yahan turn ROTATE HOTA HAI (tic-tac-toe se ULTA!).
                // Kyunki yahan player ne koi GALTI nahi ki — pasa hi galat aaya.
                // Uski baari "barbaad" ho gayi, aur asli khel me bhi yahi hota
                // hai. Tic-tac-toe me galat move par turn nahi ghumta tha, kyunki
                // wahan galti PLAYER ki hoti thi. Farak samajh aaya? 🎯
                cout << "Need exact roll to reach " << board_->getBoardSize() << "!" << endl;
                players_.pop_front();
                players_.push_back(currentPlayer);
            }
        }

        // 💡 Ye loop pehle HAMESHA ke liye ghoom sakta tha! Agar jeetne wale ghar
        // (100) pe koi SAANP hota, to koi kabhi 100 pe ruk hi nahi paata ->
        // `checkWinCondition` kabhi true nahi hota -> game khatam hi nahi hota. 💀
        // (Test kiya tha: 4 second me 16 LAKH lines output, koi vijeta nahi.)
        //
        // ✅ Ab `Board::addBoardEntity()` aisi entity ko board pe chadhne hi nahi
        // deta. Aakhri ghar hamesha khaali rehta hai, to jeet hamesha mumkin hai.
        // 📌 Bug yahan DIKHA tha, par fix Board me hua — jad pe. Hamesha yahi karo.
    }
};

}

#endif
