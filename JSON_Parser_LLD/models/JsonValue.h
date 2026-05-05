#ifndef JSON_PARSER_LLD_MODELS_JSONVALUE_H
#define JSON_PARSER_LLD_MODELS_JSONVALUE_H

#include <bits/stdc++.h>

#include "../enums/JsonType.h"

using namespace std;

namespace json_parser_lld {

class JsonValue {
public:
    virtual ~JsonValue() = default;
    virtual JsonType getType() const = 0;
    virtual void print(int indent = 0) const = 0;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONVALUE_H
