// ============================================================================
//  models/JsonBoolean.h  —  JSON ka `true`/`false` (Composite ka LEAF node)
// ----------------------------------------------------------------------------
//  Ek bool value rakhta hai. Print me `true` ya `false` (C++ ka `1`/`0` nahi —
//  JSON me lowercase words hote hain).
//
//  📌 LEAF node (JsonValue.h Composite tree). `value()` getter se andar ka bool
//     nikaal sakte ho (jaise koi code JSON tree ko traverse karke boolean padhe).
// ============================================================================
#ifndef JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H
#define JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H

#include <iostream>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonBoolean : public JsonValue {
public:
    explicit JsonBoolean(bool value) : value_(value) {}

    JsonType getType() const override { return JsonType::BOOLEAN; }
    bool value() const { return value_; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << (value_ ? "true" : "false"); // JSON style (lowercase words)
    }

private:
    bool value_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H
