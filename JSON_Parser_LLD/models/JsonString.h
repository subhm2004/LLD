#ifndef JSON_PARSER_LLD_MODELS_JSONSTRING_H
#define JSON_PARSER_LLD_MODELS_JSONSTRING_H

#include <bits/stdc++.h>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonString : public JsonValue {
public:
    explicit JsonString(string value) : value_(std::move(value)) {}

    JsonType getType() const override { return JsonType::STRING; }
    const string &value() const { return value_; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << '"' << value_ << '"';
    }

private:
    string value_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONSTRING_H
