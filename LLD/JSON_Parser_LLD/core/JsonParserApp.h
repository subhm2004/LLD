// ============================================================================
//  core/JsonParserApp.h  —  Demo runner (parser ka simple client/facade)
// ----------------------------------------------------------------------------
//  Ye ek chhoti demo class hai jo poora flow ek jagah dikhati hai:
//     JSON string  ->  JsonParser.parse()  ->  JsonValue tree  ->  print()  ->  cleanup
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ POORA LIFECYCLE ek nazar me:                                          │
//  │                                                                          │
//  │   1. JsonParser parser(json)   — parser banao (string ke saath)          │
//  │   2. root = parser.parse()     — tree ban gaya (JsonValue* — heap pe)     │
//  │   3. root->print()             — tree ko wapas JSON string me chhaapo     │
//  │   4. delete root               — ⭐ POORA tree recursively saaf (Composite │
//  │                                  + virtual destructor ka jaadu)          │
//  │                                                                          │
//  │  Aur ye sab ek try/catch me hai — invalid JSON pe parse() exception       │
//  │  phenkta hai (descriptive message ke saath), jise yahan pakad lete hain.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `delete root` ONE line me poora tree free kar deta hai — kyunki har
//     container (object/array) ka destructor apne children ko delete karta hai
//     (recursively). Yahi Composite pattern ka cleanup fayda hai.
//
//  ⚠ Note: yahan `root` raw pointer hai; agar `print()` ke aas-paas exception
//     aati to `root` leak ho sakta. Is demo me print() throw nahi karta, par
//     production me `unique_ptr<JsonValue> root(parser.parse())` behtar hota
//     (RAII cleanup — parser ke andar hum yahi kar chuke hain).
// ============================================================================
#ifndef JSON_PARSER_LLD_CORE_JSONPARSERAPP_H
#define JSON_PARSER_LLD_CORE_JSONPARSERAPP_H

#include <exception>
#include <iostream>
#include <string>

#include "../parser/JsonParser.h"

using namespace std;

namespace json_parser_lld {

class JsonParserApp {
public:
    void runDemo() const {
        // Ek sample JSON jisme sab types hain: string, number, boolean, array,
        // nested object, aur decimal — poora parser exercise ho jaata hai.
        const string json =
            "{\"name\":\"Shubham\",\"age\":25,\"active\":true,\"skills\":[\"C++\",\"LLD\"],\"profile\":{\"city\":"
            "\"Delhi\"},\"score\":99.5}";

        try {
            JsonParser parser(json);
            JsonValue *root = parser.parse(); // string -> tree
            cout << "Parsed JSON: ";
            root->print(); // tree -> JSON string (keys sorted — map ki wajah se)
            cout << "\n";
            delete root; // ⭐ poora tree recursively saaf
        } catch (const exception &ex) {
            // Invalid JSON -> descriptive error (R5). Parser ne andar hi leak-safe cleanup kar diya.
            cout << "Parse Error: " << ex.what() << "\n";
        }
    }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_CORE_JSONPARSERAPP_H
