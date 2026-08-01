// ============================================================================
//  main.cpp  —  JSON Parser ka entry point (CLIENT)
// ----------------------------------------------------------------------------
//  Bas itna: demo app banao aur chalao. Saara asli kaam (parse -> tree -> print
//  -> cleanup) JsonParserApp ke andar hai — client ko sirf ek method bulana hai.
//
//  ⭐ Client ko parser ki complexity (recursive descent, cursor, Composite tree,
//     memory cleanup) ka kuch pata nahi — JsonParserApp sab wrap kar deta hai.
// ============================================================================
#include "core/JsonParserApp.h"

using namespace json_parser_lld;

int main() {
    JsonParserApp app;
    app.runDemo();
    return 0;
}
