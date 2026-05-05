#ifndef JSON_PARSER_LLD_MODELS_JSONNUMBER_H
#define JSON_PARSER_LLD_MODELS_JSONNUMBER_H

#include <bits/stdc++.h>

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
        cout << value_;
    }

private:
    double value_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONNUMBER_H
