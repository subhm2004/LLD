#ifndef JSON_PARSER_LLD_MODELS_JSONOBJECT_H
#define JSON_PARSER_LLD_MODELS_JSONOBJECT_H

#include <bits/stdc++.h>

#include "JsonValue.h"

using namespace std;

namespace json_parser_lld {

class JsonObject : public JsonValue {
public:
    ~JsonObject() override {
        for (auto &kv : properties_) {
            delete kv.second;
        }
    }

    JsonType getType() const override { return JsonType::OBJECT; }
    void put(const string &key, JsonValue *value) { properties_[key] = value; }

    void print(int indent = 0) const override {
        (void)indent;
        cout << "{";
        bool first = true;
        for (const auto &kv : properties_) {
            if (!first) {
                cout << ", ";
            }
            cout << '"' << kv.first << "\": ";
            kv.second->print(indent);
            first = false;
        }
        cout << "}";
    }

private:
    map<string, JsonValue *> properties_;
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_MODELS_JSONOBJECT_H
