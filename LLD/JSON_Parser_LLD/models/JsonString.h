// ============================================================================
//  models/JsonString.h  —  JSON ka string (Composite ka LEAF node)
// ----------------------------------------------------------------------------
//  Ek text value rakhta hai. Print me quotes ke saath (`"text"`) — kyunki JSON
//  me strings hamesha double-quotes me hote hain.
//
//  📌 LEAF node. JSON me KEYS bhi strings hote hain ({"name": ...}), isliye
//     parser `parseString()` ko keys aur values dono ke liye use karta hai
//     (JsonParser.h me parseObject dekho — key parseString se aati hai).
//
//  💡 `explicit` constructor: `JsonString s = "hi";` galti se convert na ho jaaye
//     — explicit se sirf `JsonString s("hi")` chalega (intentional construction).
//     Chhoti safety, badi clarity.
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONSTRING_H
#define JSON_PARSER_LLD_MODELS_JSONSTRING_H

#include <iostream>
#include <string>
#include <utility>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonString : public JsonValue {
public:
    explicit JsonString(string value) : value_(std::move(value)) {} // move -> copy bachti

    JsonType getType() const override { return JsonType::STRING; }
    const string &value() const { return value_; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << '"' << value_ << '"'; // JSON style (quotes ke saath)
    }

private:
    string value_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONSTRING_H
