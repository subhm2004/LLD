// ============================================================================
//  models/JsonObject.h  —  JSON ka object (Composite ka CONTAINER/COMPOSITE node)
// ----------------------------------------------------------------------------
//  `{ "key": value, ... }` — key-value pairs. Key hamesha string, value koi bhi
//  JsonValue (nested object/array bhi). JsonArray jaisa hi COMPOSITE node, bas
//  yahan children ko KEY se access karte hain (list nahi, map).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::map` (ordered) kyun, `unordered_map` nahi?                       │
//  │                                                                          │
//  │  `map` keys ko SORTED (alphabetical) order me rakhta hai -> print() ka    │
//  │  output DETERMINISTIC hota hai (har baar same order). `unordered_map`     │
//  │  random order deta, output run-to-run badal jaata (testing/demo me bura). │
//  │                                                                          │
//  │  ⚠ TRADE-OFF: JSON spec me keys ka INSERTION order preserve hona chahiye  │
//  │  (jaise input me tha). `map` alphabetical kar deta hai — to output ka     │
//  │  order input se alag ho sakta. Insertion order chahiye to                 │
//  │  `vector<pair<string, JsonValue*>>` use karte. Yahan determinism > exact   │
//  │  order (interview-scope), isliye map. Ye trade-off bolna accha point hai. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ OWNERSHIP: object apne values ka MAALIK hai — destructor har value ko
//     delete karta hai (nested containers recursively saaf). (Keys strings hain,
//     unka apne aap cleanup ho jaata; delete sirf values `kv.second` ka.)
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONOBJECT_H
#define JSON_PARSER_LLD_MODELS_JSONOBJECT_H

#include <iostream>
#include <map>
#include <string>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonObject : public JsonValue {
public:
    // ⭐ Destructor har VALUE ko delete (keys strings hain, auto-clean).
    ~JsonObject() override {
        for (auto &kv : properties_) {
            delete kv.second; // recursive: nested container ka dtor bhi chalega
        }
    }

    JsonType getType() const override { return JsonType::OBJECT; }

    // key -> value daalo (ownership object ke paas). Same key dobara -> overwrite.
    void put(const string &key, JsonValue *value) { properties_[key] = value; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << "{";
        bool first = true;
        for (const auto &kv : properties_) { // map -> keys sorted order me
            if (!first) {
                cout << ", ";
            }
            cout << '"' << kv.first << "\": ";
            kv.second->print(indent); // ⭐ recursive: value ka print (leaf ya nested)
            first = false;
        }
        cout << "}";
    }

private:
    map<string, JsonValue *> properties_; // key -> value (ordered, object owns values)
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONOBJECT_H
