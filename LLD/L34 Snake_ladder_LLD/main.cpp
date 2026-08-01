// ============================================================================
//  main.cpp  —  Snake & Ladder ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  User se poochta hai kaunsa board chahiye, factory se game banata hai, players
//  jodta hai, aur `play()` bol deta hai. Baaki sab andar hota hai.
//
//  ============================================================================
//   IS PROJECT ME 4 DESIGN PATTERNS
//  ----------------------------------------------------------------------------
//   1. STRATEGY -> BoardSetupStrategy : board bharne ke kai tareeke
//                                       (Standard / Random / Custom)
//   2. BRIDGE   -> Board <-> Setup    : Board (abstraction) aur Setup
//                                       (implementation) alag-alag badh sakte
//                                       hain. M×N class explosion se bachav.
//                                       ⭐ Yahi is lecture ka STAR pattern hai.
//   3. FACTORY  -> GameFactory        : game banane ke 6 kadam ek jagah
//   4. OBSERVER -> IObserver          : game events ki khabar, bina engine chhue
//
//   (Rules bhi ek Strategy hai, par wo abhi hardcoded hai — SnakeAndLadderGame.h
//    me detail me likha hai. Use inject karna ek accha exercise hai.)
//  ============================================================================
#include <bits/stdc++.h>

#include "factories/SnakeAndLadderGameFactory.h"
#include "observers/SnakeAndLadderConsoleNotifier.h"

using namespace std;
using namespace snake_ladder_lld;

// ============================================================================
//  ✅ HELPER — ek int safely padho (galat input pe dobara maango)
// ----------------------------------------------------------------------------
//  Pehle poore main me har jagah seedha `cin >> x;` likha tha, bina koi check ke.
//  Agar user number ki jagah "abc" type kar deta:
//     - `cin` FAIL state me chala jaata
//     - `x` me KACHRA reh jaata (uninitialized garbage!)
//     - "abc" buffer me ATKA reh jaata
//     - aur uske baad ka HAR `cin >>` turant fail hota — chup-chaap.
//  Natija: program kachre ke saath aage badhta rehta. Kabhi crash, kabhi bakwas
//  board, kabhi hang. Aur wajah pakadna bahut mushkil.
//
//  Ab har input isi helper se guzarta hai. Do line ka ilaaj:
//     cin.clear()  -> fail flag hatao (warna cin hamesha "toota" rahega)
//     cin.ignore() -> kachra input BUFFER SE PHENKO (ye step bhoolna sabse
//                     common galti hai — clear() sirf flag hatata hai, ganda
//                     data buffer me PADA rehta hai!)
//
//  📌 Ek jagah helper bana ke saat jagah use karna — DRY principle. Aur bug bhi
//     ek hi jagah fix karna padta hai.
// ============================================================================
static int readInt(const string &prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;

        if (cin >> value && value >= minVal && value <= maxVal) {
            return value;   // sahi number mila
        }

        if (cin.eof()) {    // input hi khatam — aage maangne ka koi fayda nahi
            cout << "\nInput khatam ho gaya. Bye!" << endl;
            exit(1);
        }

        cin.clear();                                          // fail flag hatao
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // kachra phenko
        cout << "  Galat input! " << minVal << " se " << maxVal << " ke beech ka number daalo.\n";
    }
}

int main() {
    // ========================================================================
    //  ✅ RANDOM SEED — yahan, program ke SABSE SHURU me, EK BAAR
    // ========================================================================
    //  🐛 PEHLE YE `Dice` ke constructor ke andar chhupa hua tha. Aur wo ek
    //     BAHUT chalak bug tha:
    //
    //       Factory ka order dekho:
    //          board->setupBoard(strategy);   // (1) yahan `rand()` chalta hai
    //          ...
    //          new Dice(6);                   // (2) `srand()` AB chalta hai! 😱
    //
    //       Yaani `rand()` PEHLE, aur `srand()` BAAD me! Bina seed ke `rand()`
    //       hamesha default seed (= 1) use karta hai. Natija: "Random with
    //       Difficulty" wala board HAR RUN ME BILKUL SAME banta tha.
    //
    //       Test karke dekha tha — teen alag run, teeno me EXACTLY ye board:
    //          Snake: 89->34   Snake: 50->21   Snake: 18->2   Ladder: 1->19 ...
    //       "Random" naam ka feature, jo bilkul bhi random nahi. 😅
    //
    //  ✅ Ab seeding yahan hai — kisi bhi `rand()` se PEHLE. Ab har run me naya
    //     board banega, jaisa hona chahiye tha.
    //
    //  📌 DO SABAK:
    //     1. `srand()` poore program me SIRF EK BAAR chalna chahiye, sabse shuru me.
    //     2. Global state (jaisa RNG ka seed) kisi class ke constructor me
    //        chhupana hamesha khatarnak hai — us class ko pata hi nahi hota ki
    //        uske banne se pehle kisne kya kiya tha.
    srand(static_cast<unsigned int>(time(0)));

    cout << "=== SNAKE AND LADDER GAME ===" << endl;
    SnakeAndLadderGame *game = nullptr;

    // ⭐ Poora game banane ka kaam `try` me hai — kyunki ab Board aur Factory
    // galat input pe `throw` karte hain (pehle chup-chaap kachra board bana dete
    // the). Yahan pakad ke ek saaf message dete hain, program ko ugly crash se
    // bachate hain.
    try {
        cout << "Choose game setup:\n1. Standard\n2. Random with Difficulty\n3. Custom" << endl;
        int choice = readInt("Choice (1-3): ", 1, 3);

        if (choice == 1) {
            // ---- STANDARD — classic 10×10 -----------------------------------
            // Koi param nahi — sab kuch fixed hai.
            game = SnakeAndLadderGameFactory::createStandardGame();

        } else if (choice == 2) {
            // ---- RANDOM — size + difficulty ---------------------------------
            // ⭐ `minVal = 4` — kyunki Board ab dimension >= 4 maangta hai.
            //    Chhote board pe setup strategies ka `rand() % (size - 10)`
            //    NEGATIVE ho jaata tha -> board ke bahar saanp, ya infinite loop.
            //    (9-cell board pe "Snake: 10 -> 8" aata tha — test karke dekha!)
            int boardSize = readInt("Enter board dimension (e.g., 10 for 10x10): ", 4, 50);

            int diffChoice = readInt("Choose difficulty: 1.Easy 2.Medium 3.Hard : ", 1, 3);
            Difficulty difficulty = (diffChoice == 1) ? EASY : (diffChoice == 3) ? HARD : MEDIUM;

            game = SnakeAndLadderGameFactory::createRandomGame(boardSize, difficulty);

        } else {
            // ---- CUSTOM — user apni strategy banata hai ----------------------
            int boardSize = readInt("Enter board dimension (e.g., 10 for 10x10): ", 4, 50);
            int cells = boardSize * boardSize;

            cout << "Choose custom setup type:\n1. Random placement using counts\n2. Exact positions" << endl;
            int customChoice = readInt("Choice (1-2): ", 1, 2);

            // ⭐ Upper limit `cells / 2` — thoda soch ke rakha hai. Board me
            // itni jagah honi chahiye ki entities lag sakein. Pehle koi limit hi
            // nahi thi: 100-cell board pe 200 saanp maang lo, aur strategy ka
            // `while` loop HAMESHA ghoomta rehta -> HANG. 🔁 (Test kiya tha.)
            //
            // ✅ Ab do taraf se suraksha hai:
            //    (a) yahan input pe limit, aur
            //    (b) strategy me bounded retries (nahi laga paye to warning)
            // Ek hi kaafi hota, par dono hona behtar hai — "defense in depth".
            int snakes = readInt("Enter number of snakes: ", 0, cells / 2);
            int ladders = readInt("Enter number of ladders: ", 0, cells / 2);

            // `randomPositions_` flag: true = jagah strategy chunegi,
            //                          false = user khud batayega
            bool randomPlacement = (customChoice == 1);
            auto *strategy = new CustomCountBoardSetupStrategy(snakes, ladders, randomPlacement);

            if (!randomPlacement) {
                // ---- Exact positions mode — user khud batata hai ------------
                // ⚠ Yahan koi validation NAHI hai — aur zaroorat bhi nahi!
                // Sari checking `Board::addBoardEntity()` me hoti hai, jo board
                // ka EKMATR darwaza hai. Galat entity wahin reject ho jaati hai
                // (aur delete bhi, to leak bhi nahi hota).
                //
                // Pehle ye bilkul khula tha — kuch bhi chal jaata tha:
                //    Snake(5, 90)   -> saanp jo UPAR le jaaye 😵
                //    Snake(100, 5)  -> JEETNE ke ghar pe saanp -> koi kabhi jeet
                //                      hi nahi sakta -> INFINITE LOOP 💀
                //                      (Test: 4 second me 16 lakh lines!)
                // Ab dono reject ho jaate hain, saaf message ke saath. ✅
                for (int i = 0; i < snakes; i++) {
                    cout << "Enter snake " << i + 1 << " start and end (start > end): ";
                    int start = readInt("", 1, cells);
                    int end = readInt("", 1, cells);
                    strategy->addSnakePosition(start, end);
                }
                for (int i = 0; i < ladders; i++) {
                    cout << "Enter ladder " << i + 1 << " start and end (end > start): ";
                    int start = readInt("", 1, cells);
                    int end = readInt("", 1, cells);
                    strategy->addLadderPosition(start, end);
                }
            }

            game = SnakeAndLadderGameFactory::createCustomGame(boardSize, strategy);

            // ⭐ Strategy ab delete kar sakte hain — aur ye SAFE hai!
            // Kyunki `Board::setupBoard()` strategy ka pointer STORE nahi karta,
            // bas use karke chhod deta hai (BoardSetupBridge.h dekho). Agar Board
            // ise sambhal ke rakhta, to ye delete ek DANGLING pointer bana deta
            // aur baad me crash hota. 💀
            //
            // 📌 "Jo banata hai wahi delete karta hai" — strategy humne banayi,
            //    to hum hi delete karenge. Factory ne nahi banayi, to wo nahi karegi.
            delete strategy;
        }

    } catch (const exception &e) {
        // Board / Factory ne throw kiya (galat size, null strategy, waghairah)
        cerr << "\nGame banane me dikkat: " << e.what() << endl;
        return 1;
    }

    // ---- OBSERVER laga do ----------------------------------------------------
    // ⭐ `IObserver*` me rakha hai, `SnakeAndLadderConsoleNotifier*` me nahi.
    // Ye jaan-boojh ke — code ab INTERFACE se baat karta hai, concrete class se
    // nahi. Kal ise `new FileLogger()` kar do, ek line badalne se poora game file
    // me log hone lagega. Baaki kuch chhuna hi nahi padega. ✅
    IObserver *notifier = new SnakeAndLadderConsoleNotifier();
    game->addObserver(notifier);

    // ---- PLAYERS ------------------------------------------------------------
    // ⭐ Kam se kam 2 players — warna `play()` "Need at least 2 players!" bol ke
    // turant return kar deta. Pehle yahan koi check nahi tha, to user 1 (ya 0!)
    // daal ke game shuru kar sakta tha, aur usko sirf ek line ka error milta.
    int numPlayers = readInt("Enter number of players (2-8): ", 2, 8);

    for (int i = 0; i < numPlayers; i++) {
        string name;
        cout << "Enter name for player " << (i + 1) << ": ";
        cin >> name;

        // ⚠ OWNERSHIP: player yahan `new` hota hai, par uska pointer hum rakhte
        // hi NAHI — seedha `addPlayer()` me de dete hain. Yaani ab uska maalik
        // GAME hai, aur game hi use delete karega (uske destructor me dekho).
        //
        // Isi liye neeche `delete player` kahin nahi hai — aur hona bhi nahi
        // chahiye! Agar likh dete, to game apne destructor me DOBARA delete
        // karta = DOUBLE FREE = crash. 💥
        //
        // 📌 (L33 me ULTA tha — wahan main.cpp players ka pointer rakhta tha aur
        //     khud delete karta tha. Dono tareeke theek hain, par CONSISTENT hona
        //     zaroori hai — aur raw pointers me ye baar-baar sochna padta hai.
        //     `unique_ptr` me signature khud bata deta ki ownership kahan ja rahi.)
        game->addPlayer(new SnakeAndLadderPlayer(i + 1, name));
    }

    // ---- KHEL SHURU ---------------------------------------------------------
    // Ek call, poora game — turn rotation, dice roll, saanp/seedhi, jeet ka check,
    // notifications. Sab andar. Client bas dekhta hai.
    game->play();

    // ---- SAFAI --------------------------------------------------------------
    // `delete game` -> game apne board, dice, rules, AUR SAARE PLAYERS delete
    // karta hai. Ek hi line me poori safai.
    delete game;
    delete notifier;   // ye humne banaya tha (game ne nahi), to hum hi delete karenge
    return 0;

    // 📌 ⚠ Agar `play()` ke beech me exception aa jaaye, to ye dono `delete` SKIP
    //    ho jaayenge aur sab leak hoga. Smart pointers (`unique_ptr`) me ye
    //    problem hoti hi nahi — scope khatam, memory saaf. Har baar, bina bhoole.
}
