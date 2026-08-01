// ============================================================================
//  models/JsonArray.h  —  JSON ka array (Composite ka CONTAINER/COMPOSITE node)
// ----------------------------------------------------------------------------
//  `[ value, value, ... ]` — ek ORDERED list of JsonValues. Iske andar kuch bhi
//  ho sakta hai: numbers, strings, ya nested objects/arrays (kyunki sab JsonValue
//  hain — Composite pattern ka fayda).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ COMPOSITE NODE — apne andar aur JsonValues rakhta hai                 │
//  │                                                                          │
//  │  `vector<JsonValue*>` — har element ek base pointer. Isi liye ek array me │
//  │  mixed cheezein ho sakti hain: [1, "hi", true, {...}]. print() recursively │
//  │  har child ka print() bulata hai — child leaf ho ya nested container, code│
//  │  ek jaisa (Composite ki khoobi).                                         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OWNERSHIP: array apne children ka MAALIK hai (destructor delete karta) │
//  │                                                                          │
//  │  `~JsonArray` har child ko `delete` karta hai. Aur wo child agar khud ek   │
//  │  container hai (nested array/object), to uska destructor uske children ko │
//  │  delete karega... yaani `delete root` se POORA TREE recursively saaf ho    │
//  │  jaata hai. Yahi Composite + virtual destructor ka combined jaadu hai.    │
//  │                                                                          │
//  │  💡 Modern C++ me `vector<unique_ptr<JsonValue>>` behtar hota (manual      │
//  │     delete hi na padta). Yahan raw+delete interview-simple rakha hai, par  │
//  │     PARSER ke andar exception-safety ke liye unique_ptr use kiya hai       │
//  │     (JsonParser.h me leak-fix note dekho).                                │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONARRAY_H
#define JSON_PARSER_LLD_MODELS_JSONARRAY_H

#include <cstddef>
#include <iostream>
#include <vector>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonArray : public JsonValue {
public:
    // ⭐ Destructor har child ko delete -> nested containers recursively saaf.
    ~JsonArray() override {
        for (JsonValue *value : values_) {
            delete value;
        }
    }

    JsonType getType() const override { return JsonType::ARRAY; }

    // Naya element daalo (ownership array ke paas aa jaati hai).
    void add(JsonValue *value) { values_.push_back(value); }

    void print(int indent = 0) const override {
        (void)indent;
        cout << "[";
        for (size_t i = 0; i < values_.size(); ++i) {
            values_[i]->print(indent);       // ⭐ recursive: child ka print (leaf ya container)
            if (i + 1 < values_.size()) {
                cout << ", ";
            }
        }
        cout << "]";
    }

private:
    vector<JsonValue *> values_; // ordered children (array owns these)
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONARRAY_H
