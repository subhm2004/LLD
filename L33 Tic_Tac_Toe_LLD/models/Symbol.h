// models/Symbol.h — Ek player ka mark (X / O ya custom char). Board cells aur
// players inhi symbols se identify hote hain.
#ifndef TICTACTOE_LLD_MODELS_SYMBOL_H
#define TICTACTOE_LLD_MODELS_SYMBOL_H

#include <bits/stdc++.h>

using namespace std;

namespace tictactoe_lld {

class Symbol {
public:
    explicit Symbol(char mark) : mark_(mark) {}
    char getMark() const { return mark_; }

private:
    char mark_;
};

} // namespace tictactoe_lld

#endif // TICTACTOE_LLD_MODELS_SYMBOL_H
