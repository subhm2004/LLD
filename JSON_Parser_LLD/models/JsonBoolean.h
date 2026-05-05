#ifndef JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H
#define JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H

#include <bits/stdc++.h>

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
        cout << (value_ ? "true" : "false");
    }

private:
    bool value_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONBOOLEAN_H
