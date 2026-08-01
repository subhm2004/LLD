// ============================================================================
//  main.cpp  —  Tic Tac Toe ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Poora game 8 line me: factory se game banao, observer lagao, 2 players daalo,
//  aur `play()` bol do. Game khud chalta rahega jab tak koi jeete ya draw ho.
//
//  ============================================================================
//   IS PROJECT ME 3 DESIGN PATTERNS
//  ----------------------------------------------------------------------------
//   1. STRATEGY  -> TicTacToeRules  : game ke NIYAM engine se alag. Rules bahar
//                                      se INJECT hote hain, to naye variants
//                                      (FourInARow, Misere) ab possible hain.
//   2. OBSERVER  -> IObserver       : game events ki khabar, bina engine ko chhue
//   3. FACTORY   -> TicTacToeGameFactory : GameType dekh ke sahi rules chunti hai
//                                      aur game me plug kar deti hai
//
//   Aur ek BONUS pattern jo naam se nahi dikhta:
//   4. NULL OBJECT -> Board::emptyCell_ : "khaali cell" bhi ek asli Symbol object
//                                          hai, nullptr nahi. Isi liye poore code
//                                          me kahin bhi null check nahi karna padta.
//                                          (Board.h me detail me likha hai.)
//  ============================================================================
//
//   ⭐ CLIENT KITNA KAM JAANTA HAI:
//     Poore system me 8 classes hain. Yahan kitni dikhti hain? Sirf 5, aur unme
//     se 3 to bas `new` karne ke liye. Board, Rules, GameType ka logic — kuch
//     nahi pata client ko. `game->play()` bola, aur poora game chal gaya.
// ============================================================================
#include <bits/stdc++.h>

#include "core/TicTacToeGameFactory.h"
#include "models/TicTacToePlayer.h"
#include "observers/ConsoleNotifier.h"

using namespace std;
using namespace tictactoe_lld;

int main() {
    cout << "=== TIC TAC TOE GAME ===\n";
    cout << "Enter board size (e.g., 3 for 3x3): ";

    // ---- ✅ INPUT VALIDATION — pehle ye tha hi nahi 🐛 -----------------------
    //
    //  Pehle seedha `cin >> boardSize;` likha tha, bina koi check ke. Natija:
    //    boardSize = 0    -> khaali board -> har move invalid -> INFINITE LOOP 🔁
    //                        (test kiya: 3 second me 29 LAKH "Invalid move!")
    //    boardSize = -1   -> Board ke andar `std::length_error` -> CRASH 💥
    //    boardSize = "abc"-> cin fail -> boardSize me KACHRA -> kuch bhi ho sakta hai
    //
    //  Ab dono cheezein check hoti hain, ek hi `if` me:
    //    (a) `!(cin >> boardSize)` -> kya padhna SAFAL hua? (number tha ya "abc"?)
    //    (b) `boardSize < 3`       -> number to hai, par kya wo SAHI hai?
    //
    //  ⭐ `||` short-circuit karta hai: agar (a) fail ho gaya, to (b) check hi
    //  nahi hota — aur ye ZAROORI hai! Kyunki cin fail hone par `boardSize` me
    //  kachra pada hai, aur us kachre ko compare karna bemaani hai.
    //
    //  📌 SABAK: user input pe KABHI bharosa mat karo. Use system ke BAHAR hi
    //  rok do (jaise yahan), taaki galat data andar pahunche hi nahi. Ise
    //  "validate at the boundary" kehte hain — galti ko darwaze pe hi rok lo.
    int boardSize;
    if (!(cin >> boardSize) || boardSize < 3) {
        cerr << "\nGalat board size! Kam se kam 3 hona chahiye (3 = 3x3).\n";
        return 1;   // non-zero = "program fail hua" (shell scripts isi se pata karte hain)
    }

    // ---- FACTORY se game banao ----------------------------------------------
    // Client ko bas type aur size dena hai. Board banana, rules chunna, unhe
    // game me inject karna — sab factory sambhal legi. Client ko `Board` ya
    // `StandardTicTacToeRules` ka naam tak nahi pata.
    //
    // ✅ `try/catch` — pehle ye nahi tha. Factory ab `nullptr` ki jagah THROW
    // karti hai (galat GameType ya galat boardSize pe). Exception ko yahan pakad
    // ke ek saaf message dete hain — program ko ugly crash se bachaate hain.
    //
    // ⭐ Pehle factory `nullptr` lautati thi aur main use CHECK HI NAHI karta tha
    // -> seedha null-pointer crash. Exception ka fayda yahi hai: use IGNORE karna
    // mushkil hai. `nullptr` ko ignore karna bahut aasan hai — aur log kar hi
    // dete hain (jaisa yahan hua tha 😅).
    TicTacToeGame *game = nullptr;
    try {
        game = TicTacToeGameFactory::createGame(STANDARD, boardSize);
    } catch (const exception &e) {
        cerr << "Game banane me dikkat: " << e.what() << "\n";
        return 1;
    }

    // ---- OBSERVER laga do ----------------------------------------------------
    // ⭐ `IObserver*` me rakha hai, `ConsoleNotifier*` me nahi. Ye jaan-boojh ke
    // hai — code ab INTERFACE se baat kar raha hai, concrete class se nahi. Kal
    // ise `new FileLogger()` kar do, ek line badalne se poora game file me log
    // hone lagega. Baaki kuch chhuna hi nahi padega.
    //
    // Aur game bina observer ke bhi chalta — ye line hata do, game phir bhi
    // khelega, bas "[Notification]" wali lines nahi aayengi. Loose coupling. ✅
    IObserver *notifier = new ConsoleNotifier();
    game->addObserver(notifier);

    // ---- PLAYERS banao -------------------------------------------------------
    // Har player ko apna Symbol object milta hai (`new Symbol('X')`), aur player
    // us Symbol ka MAALIK ban jaata hai — uske destructor me wo delete hoga.
    //
    // ⭐ Dono players ke marks ALAG hone chahiye ('X' aur 'O') — warna game ka
    // koi matlab hi nahi (dono ke moves ek jaise dikhte). Rules ab MARK (value)
    // se compare karte hain, pointer se nahi — to asli shart yahi hai ki mark
    // alag ho. (Pehle pointer se compare hota tha; tab do alag Symbol OBJECTS
    // honi zaroori thi, chahe mark same hi ho. Wo nazuk design tha, ab fix hai.)
    //
    // ⚠ Aur dhyaan rahe: koi bhi player '-' ko apna mark na rakhe — wo Board ka
    // "khaali cell" mark hai. (Rules me iska guard laga hua hai, phir bhi.)
    TicTacToePlayer *player1 = new TicTacToePlayer(1, "Aditya", new Symbol('X'));
    TicTacToePlayer *player2 = new TicTacToePlayer(2, "Harshita", new Symbol('O'));
    game->addPlayer(player1);
    game->addPlayer(player2);

    // ---- KHEL SHURU ----------------------------------------------------------
    // Ye ek call poora game chala deti hai — turn rotation, board display, move
    // input, win/draw check, notifications. Sab andar hota hai. Client bas dekhta
    // hai. (Loop tabhi tootega jab koi jeete ya draw ho... ya us input wale bug
    // me phans jaaye. TicTacToeGame.h me padho.)
    game->play();

    // ---- SAFAI (cleanup) -----------------------------------------------------
    // ⭐ ORDER MAYNE RAKHTA HAI — aur yahan sahi hai:
    //
    //   `delete game` PEHLE  -> game apna Board aur Rules delete karta hai.
    //                            Board ki grid me players ke Symbol pointers pade
    //                            hain, par Board unhe CHHUTA bhi nahi (sirf apna
    //                            emptyCell_ delete karta hai). ✅
    //   `delete player1/2` BAAD me -> ab players apne Symbols delete karte hain.
    //
    //   Agar ULTA karte (players pehle, game baad me), to Board ki grid me
    //   DANGLING pointers reh jaate — aur agar Board ka destructor unhe chhuta,
    //   to use-after-free. Yahan Board unhe chhuta nahi, to bach jaate... par
    //   itne nazuk bharose pe code nahi likhna chahiye. 😬
    //
    //   📌 ASLI FIX: `unique_ptr` / `shared_ptr` use karo. Tab ye "kaun pehle,
    //   kaun baad me" wali sar-dardi khatam — lifetime apne aap sambhal jaati
    //   hai, aur `delete` bhoolne ka sawaal hi nahi rehta.
    //
    //   (Waise dhyaan do: agar game beech me `return` kar de — jaise "Need at
    //    least 2 players!" wale case me — tab bhi ye deletes chalte hain, kyunki
    //    play() return karta hai, main nahi. Yahan theek hai. Par agar kahin
    //    exception aa jaaye, to ye saari lines SKIP ho jaayengi aur sab leak
    //    hoga. Smart pointers me ye problem hoti hi nahi.)
    delete game;
    delete player1;
    delete player2;
    delete notifier;
    return 0;
}
