// ============================================================================
//  models/JsonValue.h  —  COMPOSITE PATTERN ka base (har JSON value ka common roop)
// ----------------------------------------------------------------------------
//  Ye abstract base class hai jise HAR JSON value (object, array, string, number,
//  boolean, null) inherit karti hai. Isse poori JSON ko ek TREE ki tarah handle
//  kar sakte hain jahan har node bas "JsonValue" hai — chahe wo simple ho (number)
//  ya complex (nested object).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ COMPOSITE PATTERN — "leaf aur container ko EK jaisa treat karo"       │
//  │                                                                          │
//  │  JSON naturally recursive/nested hai: ek array ke andar objects, unke     │
//  │  andar arrays... Composite pattern kehta hai: SAB (chhoti value + badi    │
//  │  container) ek COMMON interface (JsonValue) follow karein. Tab:           │
//  │    - JsonArray me `vector<JsonValue*>` rakh sakte ho (kuch bhi daalo)      │
//  │    - `print()` recursively chal jaata hai (array apne har child ka print   │
//  │      bulata, chahe child leaf ho ya nested container)                     │
//  │  Client ko "ye leaf hai ya container" farak karne ki zaroorat hi nahi.    │
//  │                                                                          │
//  │     JsonValue (component)                                                 │
//  │       ├── JsonObject / JsonArray   (COMPOSITE — apne andar aur values)    │
//  │       └── String/Number/Boolean/Null (LEAF — akeli value)                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ DO PURE VIRTUAL methods (har derived ko implement karne HI padenge):
//     getType()  -> ye value kaunse type ki hai (enum)
//     print()    -> is value ko JSON string me chhaap do (recursively)
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: parser `JsonValue*` (base pointer) return karta
//     hai jo asal me derived (JsonObject etc.) hota hai. `delete root` base
//     pointer se hota hai — bina virtual destructor ke derived ka destructor
//     (jo children delete karta hai!) chalta hi nahi -> poora tree leak + UB.
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONVALUE_H
#define JSON_PARSER_LLD_MODELS_JSONVALUE_H

#include "../enums/JsonType.h"

namespace json_parser_lld {

class JsonValue {
public:
    virtual ~JsonValue() = default;

    virtual JsonType getType() const = 0;         // ye value kaunse type ki hai
    virtual void print(int indent = 0) const = 0; // JSON string me print (recursive)
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONVALUE_H
