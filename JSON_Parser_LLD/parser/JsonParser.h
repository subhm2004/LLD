#ifndef JSON_PARSER_LLD_PARSER_JSONPARSER_H
#define JSON_PARSER_LLD_PARSER_JSONPARSER_H

#include <bits/stdc++.h>

#include "../models/JsonArray.h"
#include "../models/JsonBoolean.h"
#include "../models/JsonNull.h"
#include "../models/JsonNumber.h"
#include "../models/JsonObject.h"
#include "../models/JsonString.h"

using namespace std;

namespace json_parser_lld {

class JsonParser {
public:
    explicit JsonParser(string input) : input_(std::move(input)), pos_(0) {}

    JsonValue *parse() {
        skipWhitespace();
        JsonValue *value = parseValue();
        skipWhitespace();
        if (pos_ != input_.size()) {
            delete value;
            throw runtime_error("Unexpected trailing characters");
        }
        return value;
    }

private:
    string input_;
    size_t pos_;

    void skipWhitespace() {
        while (pos_ < input_.size() && isspace(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    char peek() const {
        if (pos_ >= input_.size()) {
            throw runtime_error("Unexpected end of input");
        }
        return input_[pos_];
    }

    char get() {
        if (pos_ >= input_.size()) {
            throw runtime_error("Unexpected end of input");
        }
        return input_[pos_++];
    }

    void expect(char expected) {
        const char c = get();
        if (c != expected) {
            throw runtime_error(string("Expected '") + expected + "', got '" + c + "'");
        }
    }

    JsonValue *parseValue() {
        skipWhitespace();
        const char c = peek();
        if (c == '{') {
            return parseObject();
        }
        if (c == '[') {
            return parseArray();
        }
        if (c == '"') {
            return parseString();
        }
        if (c == '-' || isdigit(static_cast<unsigned char>(c))) {
            return parseNumber();
        }
        if (startsWith("true")) {
            pos_ += 4;
            return new JsonBoolean(true);
        }
        if (startsWith("false")) {
            pos_ += 5;
            return new JsonBoolean(false);
        }
        if (startsWith("null")) {
            pos_ += 4;
            return new JsonNull();
        }
        throw runtime_error("Invalid JSON value at position " + to_string(pos_));
    }

    JsonObject *parseObject() {
        JsonObject *obj = new JsonObject();
        expect('{');
        skipWhitespace();

        if (peek() == '}') {
            get();
            return obj;
        }

        while (true) {
            skipWhitespace();
            JsonString *key = parseString();
            skipWhitespace();
            expect(':');
            skipWhitespace();

            JsonValue *value = parseValue();
            obj->put(key->value(), value);
            delete key;

            skipWhitespace();
            const char c = get();
            if (c == '}') {
                break;
            }
            if (c != ',') {
                delete obj;
                throw runtime_error("Expected ',' or '}' in object");
            }
        }
        return obj;
    }

    JsonArray *parseArray() {
        JsonArray *arr = new JsonArray();
        expect('[');
        skipWhitespace();

        if (peek() == ']') {
            get();
            return arr;
        }

        while (true) {
            JsonValue *value = parseValue();
            arr->add(value);

            skipWhitespace();
            const char c = get();
            if (c == ']') {
                break;
            }
            if (c != ',') {
                delete arr;
                throw runtime_error("Expected ',' or ']' in array");
            }
        }
        return arr;
    }

    JsonString *parseString() {
        expect('"');
        string result;

        while (true) {
            const char c = get();
            if (c == '"') {
                break;
            }
            if (c == '\\') {
                const char esc = get();
                if (esc == '"' || esc == '\\' || esc == '/') {
                    result.push_back(esc);
                } else if (esc == 'n') {
                    result.push_back('\n');
                } else if (esc == 't') {
                    result.push_back('\t');
                } else {
                    throw runtime_error("Unsupported escape sequence");
                }
            } else {
                result.push_back(c);
            }
        }
        return new JsonString(result);
    }

    JsonNumber *parseNumber() {
        const size_t start = pos_;
        if (input_[pos_] == '-') {
            ++pos_;
        }
        while (pos_ < input_.size() && isdigit(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
        if (pos_ < input_.size() && input_[pos_] == '.') {
            ++pos_;
            while (pos_ < input_.size() && isdigit(static_cast<unsigned char>(input_[pos_]))) {
                ++pos_;
            }
        }
        const double value = stod(input_.substr(start, pos_ - start));
        return new JsonNumber(value);
    }

    bool startsWith(const string &token) const { return input_.substr(pos_, token.size()) == token; }
};

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_PARSER_JSONPARSER_H
