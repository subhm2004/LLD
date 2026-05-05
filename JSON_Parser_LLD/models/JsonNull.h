#ifndef JSON_PARSER_LLD_MODELS_JSONNULL_H
#define JSON_PARSER_LLD_MODELS_JSONNULL_H

#include <bits/stdc++.h>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonNull : public JsonValue {
public:
    JsonType getType() const override { return JsonType::NIL; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << "null";
    }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONNULL_H
