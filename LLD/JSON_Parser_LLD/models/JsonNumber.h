// ============================================================================
//  models/JsonNumber.h  —  JSON ka number (Composite ka LEAF node)
// ----------------------------------------------------------------------------
//  Ek number rakhta hai. JSON me integer aur float ka farak nahi hota (dono
//  bas "number" hain), isliye sabko `double` me store karte hain (42 aur 3.14
//  dono double me fit ho jaate).
//
//  ⚠ LIMITATION: `double` me bahut bade integers (jaise 64-bit IDs) exactly
//     store nahi hote (precision loss). Real JSON libraries integer aur double
//     ko alag rakhti hain. Yahan interview-scope me double kaafi hai — par ye
//     trade-off bolna accha point hai.
//
//  📌 LEAF node. `value()` se andar ka double milta hai. Parser `parseNumber()`
//     me `stod` se string ko double banata hai (JsonParser.h dekho).
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONNUMBER_H
#define JSON_PARSER_LLD_MODELS_JSONNUMBER_H

#include <iostream>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonNumber : public JsonValue {
public:
    explicit JsonNumber(double value) : value_(value) {}

    JsonType getType() const override { return JsonType::NUMBER; }
    double value() const { return value_; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << value_; // 42.0 -> "42" (cout default double formatting)
    }

private:
    double value_; // integer + float dono (JSON me farak nahi)
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONNUMBER_H
