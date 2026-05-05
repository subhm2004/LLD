#ifndef JSON_PARSER_LLD_MODELS_JSONARRAY_H
#define JSON_PARSER_LLD_MODELS_JSONARRAY_H

#include <bits/stdc++.h>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonArray : public JsonValue {
public:
    ~JsonArray() override {
        for (JsonValue *value : values_) {
            delete value;
        }
    }

    JsonType getType() const override { return JsonType::ARRAY; }
    void add(JsonValue *value) { values_.push_back(value); }

    void print(int indent = 0) const override {
        (void)indent;
        cout << "[";
        for (size_t i = 0; i < values_.size(); ++i) {
            values_[i]->print(indent);
            if (i + 1 < values_.size()) {
                cout << ", ";
            }
        }
        cout << "]";
    }

private:
    vector<JsonValue *> values_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONARRAY_H
