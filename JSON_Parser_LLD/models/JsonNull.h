// ============================================================================
//  models/JsonNull.h  —  JSON ka `null` (Composite ka LEAF node)
// ----------------------------------------------------------------------------
//  Sabse simple JSON value: null. Iske paas koi data nahi (bas "khaali" hai),
//  isliye koi member variable nahi — sirf type batana aur "null" print karna.
//
//  📌 Ye ek LEAF hai (JsonValue.h ka Composite tree dekho) — iske andar aur
//     values nahi hoti (object/array ke ulat). `print()` me `indent` ignore hota
//     (`(void)indent`) kyunki leaf ko indentation ki zaroorat nahi.
//
//  💡 `null` ke liye ek poora object banana zaroori hai (nullptr nahi) — kyunki
//     Composite tree me har node ek JsonValue hona chahiye. "null bhi ek value hai"
//     (missing value se alag). Isi liye JsonNull class, na ki plain nullptr.
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONNULL_H
#define JSON_PARSER_LLD_MODELS_JSONNULL_H

#include <iostream>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonNull : public JsonValue {
public:
    JsonType getType() const override { return JsonType::NIL; }

    void print(int indent = 0) const override {
        (void)indent; // leaf — indent ki zaroorat nahi
        cout << "null";
    }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONNULL_H
