// ============================================================================
//  parser/JsonParser.h  —  RECURSIVE DESCENT parser (string -> JsonValue tree)
// ----------------------------------------------------------------------------
//  Ye class ek JSON string ko padh ke uska in-memory TREE (JsonValue*) banati hai.
//  Technique: "Recursive Descent" — har JSON grammar rule ka ek function, aur wo
//  functions ek doosre ko RECURSIVELY bulate hain (kyunki JSON khud nested hai).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ RECURSIVE DESCENT — grammar ka har rule = ek function                 │
//  │                                                                          │
//  │     parseValue()  -> pehla char dekho, decide karo kya hai, sahi          │
//  │                      function ko bhej do:                                 │
//  │        '{' -> parseObject()   '[' -> parseArray()   '"' -> parseString()  │
//  │        digit/- -> parseNumber()   t/f/n -> true/false/null                │
//  │                                                                          │
//  │     parseObject() apne values ke liye parseValue() bulata hai...          │
//  │     parseArray() apne elements ke liye parseValue() bulata hai...         │
//  │     ...aur parseValue() phir parseObject/parseArray bula sakta hai!       │
//  │                                                                          │
//  │  Yahi MUTUAL RECURSION nested JSON ko handle karti hai:                   │
//  │     {"a": [1, {"b": 2}]}  -> object -> array -> object -> number          │
//  │  Grammar recursive hai, to parser bhi recursive — natural mapping.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CURSOR (`pos_`) — ek "ungli" jo string me aage badhti hai            │
//  │                                                                          │
//  │  Parser input string ko ek baar, left-to-right padhta hai. `pos_` batata  │
//  │  hai "abhi kahan hain". Helpers:                                          │
//  │    peek() -> abhi wala char DEKHO (aage mat badho)                        │
//  │    get()  -> abhi wala char lo AUR aage badho (pos_++)                    │
//  │    expect(c) -> agla char `c` hona HI chahiye, warna error                │
//  │    skipWhitespace() -> spaces/newlines chhod do                           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 MEMORY LEAK FIX (error paths) — unique_ptr se exception-safety        │
//  │                                                                          │
//  │  PEHLE: parseObject/parseArray me container `new` se banta tha, aur agar   │
//  │  beech me exception aa jaati (invalid JSON), to wo already-bana container  │
//  │  (aur key) LEAK ho jaata — delete kabhi nahi hota. Valid JSON pe koi       │
//  │  problem nahi, par INVALID JSON pe har error = ek leak.                   │
//  │                                                                          │
//  │  ✅ FIX: container ko `unique_ptr` me rakha. Ab agar kahin bhi exception   │
//  │  aayi, unique_ptr ka destructor use apne aap saaf kar deta (RAII, stack   │
//  │  unwinding). Success par `.release()` se ownership caller ko wapas.       │
//  │  (Composite model classes waise hi raw-pointer rakhti hain — sirf parser  │
//  │   ke andar RAII use kiya, jahan exceptions aati hain.)                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ SCOPE (requirements): basic escapes (\" \\ \/ \n \t) support hain, par
//     \uXXXX unicode, exponent numbers (1e5), comments, trailing commas — nahi
//     (out of scope). Correctness-first interview parser.
// ============================================================================
#ifndef JSON_PARSER_LLD_PARSER_JSONPARSER_H
#define JSON_PARSER_LLD_PARSER_JSONPARSER_H

#include <cctype>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "../models/JsonArray.h"
#include "../models/JsonBoolean.h"
#include "../models/JsonNull.h"
#include "../models/JsonNumber.h"
#include "../models/JsonObject.h"
#include "../models/JsonString.h"

using namespace std;

namespace json_parser_lld {

class JsonParser {
public:
    explicit JsonParser(string input) : input_(std::move(input)), pos_(0) {}

    // ---- ENTRY POINT: poori string parse karo, root JsonValue* lauta do ------
    JsonValue *parse() {
        skipWhitespace();
        JsonValue *value = parseValue();
        skipWhitespace();
        // ⭐ Poori string khatam honi chahiye. Agar value ke baad kuch aur bacha
        //    (jaise "{}garbage"), to wo invalid hai. `value` ko delete karke error.
        if (pos_ != input_.size()) {
            delete value; // trailing garbage -> jo bana use saaf karo
            throw runtime_error("Unexpected trailing characters");
        }
        return value;
    }

private:
    string input_; // poori JSON string
    size_t pos_;   // cursor — abhi kahan padh rahe hain

    // ---- Cursor helpers -----------------------------------------------------
    void skipWhitespace() {
        while (pos_ < input_.size() && isspace(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    char peek() const { // dekho, aage mat badho
        if (pos_ >= input_.size()) {
            throw runtime_error("Unexpected end of input");
        }
        return input_[pos_];
    }

    char get() { // lo aur aage badho
        if (pos_ >= input_.size()) {
            throw runtime_error("Unexpected end of input");
        }
        return input_[pos_++];
    }

    void expect(char expected) { // agla char `expected` hona hi chahiye
        const char c = get();
        if (c != expected) {
            throw runtime_error(string("Expected '") + expected + "', got '" + c + "'");
        }
    }

    // ========================================================================
    //  ⭐ parseValue — "pehla char dekho, sahi parser ko bhejo" (dispatcher)
    // ========================================================================
    JsonValue *parseValue() {
        skipWhitespace();
        const char c = peek();
        if (c == '{') {
            return parseObject(); // { ... }
        }
        if (c == '[') {
            return parseArray(); // [ ... ]
        }
        if (c == '"') {
            return parseString(); // "..."
        }
        if (c == '-' || isdigit(static_cast<unsigned char>(c))) {
            return parseNumber(); // 42, -3.14
        }
        // Literals: true / false / null (poora word match karo, aur cursor aage badhao)
        if (startsWith("true")) {
            pos_ += 4;
            return new JsonBoolean(true);
        }
        if (startsWith("false")) {
            pos_ += 5;
            return new JsonBoolean(false);
        }
        if (startsWith("null")) {
            pos_ += 4;
            return new JsonNull();
        }
        throw runtime_error("Invalid JSON value at position " + to_string(pos_));
    }

    // ========================================================================
    //  parseObject — { "key": value, "key": value, ... }
    // ========================================================================
    JsonObject *parseObject() {
        auto obj = std::make_unique<JsonObject>(); // ⭐ unique_ptr -> exception-safe (leak fix)
        expect('{');
        skipWhitespace();

        if (peek() == '}') { // khaali object {}
            get();
            return obj.release(); // ownership caller ko
        }

        while (true) {
            skipWhitespace();
            // Key bhi ek string hai. unique_ptr -> agar aage exception aayi to key auto-delete.
            std::unique_ptr<JsonString> key(parseString());
            skipWhitespace();
            expect(':'); // key ke baad colon
            skipWhitespace();

            JsonValue *value = parseValue(); // ⭐ agar throw hua -> key & obj dono auto-saaf
            obj->put(key->value(), value);   // value ki ownership obj ko
            // key is iteration ke end pe auto-delete (unique_ptr scope) — original `delete key` jaisa

            skipWhitespace();
            const char c = get();
            if (c == '}') { // object khatam
                break;
            }
            if (c != ',') { // ya to comma (aur pairs), ya close brace
                throw runtime_error("Expected ',' or '}' in object"); // obj auto-delete (unique_ptr)
            }
        }
        return obj.release();
    }

    // ========================================================================
    //  parseArray — [ value, value, ... ]
    // ========================================================================
    JsonArray *parseArray() {
        auto arr = std::make_unique<JsonArray>(); // ⭐ unique_ptr -> exception-safe (leak fix)
        expect('[');
        skipWhitespace();

        if (peek() == ']') { // khaali array []
            get();
            return arr.release();
        }

        while (true) {
            JsonValue *value = parseValue(); // ⭐ throw hua -> arr auto-saaf
            arr->add(value);                 // ownership arr ko

            skipWhitespace();
            const char c = get();
            if (c == ']') { // array khatam
                break;
            }
            if (c != ',') {
                throw runtime_error("Expected ',' or ']' in array"); // arr auto-delete
            }
        }
        return arr.release();
    }

    // ========================================================================
    //  parseString — "text" (escapes ke saath)
    // ========================================================================
    JsonString *parseString() {
        expect('"'); // opening quote
        string result;

        while (true) {
            const char c = get();
            if (c == '"') { // closing quote -> string khatam
                break;
            }
            if (c == '\\') { // escape sequence -> agla char dekho
                const char esc = get();
                if (esc == '"' || esc == '\\' || esc == '/') {
                    result.push_back(esc); // \" \\ \/ -> waisa hi char
                } else if (esc == 'n') {
                    result.push_back('\n'); // newline
                } else if (esc == 't') {
                    result.push_back('\t'); // tab
                } else {
                    throw runtime_error("Unsupported escape sequence"); // \u etc. out of scope
                }
            } else {
                result.push_back(c); // normal char
            }
        }
        return new JsonString(result);
    }

    // ========================================================================
    //  parseNumber — 42, -3.14 (integer + decimal; exponent out of scope)
    // ========================================================================
    JsonNumber *parseNumber() {
        const size_t start = pos_; // number ka shuruaati position yaad rakho
        if (input_[pos_] == '-') { // optional minus
            ++pos_;
        }
        while (pos_ < input_.size() && isdigit(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_; // integer part ke digits
        }
        if (pos_ < input_.size() && input_[pos_] == '.') { // optional decimal part
            ++pos_;
            while (pos_ < input_.size() && isdigit(static_cast<unsigned char>(input_[pos_]))) {
                ++pos_;
            }
        }
        // start..pos_ ka substring lo aur double me convert karo.
        const double value = stod(input_.substr(start, pos_ - start));
        return new JsonNumber(value);
    }

    // Kya cursor pos_ se `token` (jaise "true") shuru hota hai?
    bool startsWith(const string &token) const { return input_.substr(pos_, token.size()) == token; }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_PARSER_JSONPARSER_H
