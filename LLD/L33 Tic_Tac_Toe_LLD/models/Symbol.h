// ============================================================================
//  models/Symbol.h  —  Ek player ka nishaan (X / O / ya koi bhi char)
// ----------------------------------------------------------------------------
//  Sirf ek `char` ke liye poori class? Zyada nahi lag raha? Chalo dekhte hain.
//
//  Fayde:
//    1. TYPE SAFETY — `Symbol*` aur `char` alag cheezein ban jaate hain. Board
//       ki grid `vector<vector<Symbol*>>` hai, `vector<vector<char>>` nahi. Ab
//       galti se koi random char cell me nahi ghus sakta.
//    2. AAGE BADHNE KI JAGAH — kal ko symbol me color chahiye? Ya emoji? Ya
//       image path? Bas is class me field add karo. Agar plain `char` hota, to
//       poore codebase me har jagah badalna padta.
//    3. IDENTITY — har player ka apna EK Symbol object hota hai (ek hi pointer),
//       aur board me wahi pointer store hota hai. Isse "ye cell kiska hai" ka
//       jawab pointer compare karke mil jaata hai. (Ismein ek catch hai — neeche
//       padho.)
//
//  ⭐ IDENTITY VS VALUE — is design ki sabse zaroori baat:
//
//    Do Symbol objects ko "barabar" kab maanein?
//      IDENTITY se -> wahi object hai kya?    (pointer compare: `a == b`)
//      VALUE se    -> mark same hai kya?      (`a->getMark() == b->getMark()`)
//
//    Pehle `StandardTicTacToeRules` POINTER se compare karta tha. Chalta bhi tha
//    — har player ka theek ek Symbol object hai, to "same pointer" = "same
//    player". Par bahut NAZUK tha: jis din koi doosra `new Symbol('X')` bana
//    deta, dono ka mark 'X' hota par pointer alag — aur jeet kabhi detect hi
//    nahi hoti. 💀
//
//    ✅ Ab wo VALUE se compare karta hai (mark se). Do 'X' ab barabar mane
//    jaate hain, chahe alag object hi kyun na ho — jo LOGICALLY sahi hai.
//
//    📌 Sabak: pointer compare tabhi karo jab tumhe ASAL ME "wahi object" chahiye
//    ho (jaise Board ka `emptyCell_` sentinel — wo waqai ek hi object hai, aur
//    wahan pointer compare bilkul sahi hai). Agar "same VALUE" chahiye, to value
//    se hi compare karo. Dono alag sawaal hain — inhe mat gaddmadd karo.
// ============================================================================
#ifndef TICTACTOE_LLD_MODELS_SYMBOL_H
#define TICTACTOE_LLD_MODELS_SYMBOL_H

#include <bits/stdc++.h>

using namespace std;

namespace tictactoe_lld {

class Symbol {
public:
    // `explicit` -> chupke se `char` ko `Symbol` me convert nahi hone dega.
    // Iske bina koi `Symbol s = 'X';` likh sakta tha. Ab `Symbol s('X');` hi
    // chalega. Single-argument constructor pe `explicit` lagana acchi aadat hai.
    explicit Symbol(char mark) : mark_(mark) {}

    // `const` -> ye function object ka data badal nahi sakta, sirf padh sakta
    // hai. Compiler ise enforce karta hai. Getter pe hamesha `const` lagao.
    char getMark() const { return mark_; }

private:
    // Note: koi setter nahi hai. Ek baar Symbol ban gaya, uska mark badal nahi
    // sakta — yaani ye IMMUTABLE hai. Ye jaan-boojh ke hai: symbol board me
    // shared hai (kai cells ek hi pointer rakhte hain). Agar koi ise beech game
    // me badal deta, to poore board ke saare 'X' ek saath badal jaate. 😱
    char mark_;
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_SYMBOL_H
