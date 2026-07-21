// ============================================================================
//  enums/JsonType.h  —  JSON me kaunse 6 tarah ke values hote hain
// ----------------------------------------------------------------------------
//  JSON spec ke hisaab se ek value in 6 me se koi ek hoti hai:
//     OBJECT  -> { "key": value, ... }   (key-value pairs)
//     ARRAY   -> [ value, value, ... ]   (ordered list)
//     STRING  -> "text"
//     NUMBER  -> 42, -3.14
//     BOOLEAN -> true / false
//     NIL     -> null
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `NIL` kyun (`NULL` nahi)?                                             │
//  │                                                                          │
//  │  `NULL` C/C++ me ek MACRO hai (0 ya nullptr). Use enum value ka naam nahi │
//  │  bana sakte — preprocessor use replace kar dega, compile error. Isliye    │
//  │  JSON ke "null" ke liye enum me `NIL` naam use kiya (JSON output me       │
//  │  phir bhi "null" hi print hota hai — JsonNull.h dekho).                  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Har JsonValue apna type `getType()` se batata hai. Isse code kisi value ka
//     "asli type" pehchan sakta hai (jaise "ye object hai ya array?") bina
//     dynamic_cast ke — enum se O(1) type check.
// ============================================================================
#ifndef JSON_PARSER_LLD_ENUMS_JSONTYPE_H
#define JSON_PARSER_LLD_ENUMS_JSONTYPE_H

namespace json_parser_lld {

enum class JsonType { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NIL };

} // namespace json_parser_lld

#endif // JSON_PARSER_LLD_ENUMS_JSONTYPE_H
