#ifndef JSON_PARSER_LLD_CORE_JSONPARSERAPP_H
#define JSON_PARSER_LLD_CORE_JSONPARSERAPP_H

#include <bits/stdc++.h>

#include "../parser/JsonParser.h"

using namespace std;

namespace json_parser_lld {

class JsonParserApp {
public:
    void runDemo() const {
        const string json =
            "{\"name\":\"Shubham\",\"age\":25,\"active\":true,\"skills\":[\"C++\",\"LLD\"],\"profile\":{\"city\":"
            "\"Delhi\"},\"score\":99.5}";

        try {
            JsonParser parser(json);
            JsonValue *root = parser.parse();
            cout << "Parsed JSON: ";
            root->print();
            cout << "\n";
            delete root;
        } catch (const exception &ex) {
            cout << "Parse Error: " << ex.what() << "\n";
        }
    }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_CORE_JSONPARSERAPP_H
