// ============================================================================
//  models/CellValue.h  —  Ek cell ki VALUE (table ke ek khaane ka data)
// ----------------------------------------------------------------------------
//  Ek cell ya to string rakh sakta hai ya int. Isliye ye ek "tagged union" jaisa
//  design hai: ek `type` tag + dono possible values ke fields.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TAGGED UNION pattern — "type batao, phir sahi field padho"           │
//  │                                                                          │
//  │     type = STRING  -> `stringValue` valid hai (intValue ignore karo)      │
//  │     type = INT     -> `intValue` valid hai (stringValue ignore karo)      │
//  │                                                                          │
//  │  `type` hi TAG hai jo batata hai kaunsa field asli data rakhta hai. Bina  │
//  │  tag ke pata hi na chalta ki cell me kya hai (dono fields hamesha maujood │
//  │  hain, bas ek hi "meaningful" hota).                                     │
//  │                                                                          │
//  │  💡 C++17 me `std::variant<string,int>` isi kaam ka modern tareeka hai    │
//  │     (type-safe, memory bhi kam). Yahan explicit struct rakha kyunki       │
//  │     padhne me saaf hai aur beginners ko tagged-union ka concept dikhta.   │
//  │     ⚠ Trade-off: `variant` galat field padhne se rok deta, ye struct nahi │
//  │     (discipline pe depend) — interview me ye bolna accha point hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACTORY helpers (`fromString` / `fromInt`) — inse cell banana SAFE hai,
//     kyunki wo type tag aur sahi field EK SAATH set karte hain. Manually
//     `CellValue{INT, "abc", 0}` banate to tag aur data mismatch ho sakta tha.
//     Isi liye demo code hamesha in factories se hi values banata hai.
//
//  📌 `equals()` pehle TYPE compare karta hai, phir value — kyunki INT 5 aur
//     STRING "5" barabar NAHI hone chahiye (type-safe comparison). Ye WHERE
//     filter (QueryService) me use hota hai.
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_MODELS_CELL_VALUE_H
#define IN_MEMORY_SQL_LLD_MODELS_CELL_VALUE_H

#include <string>

#include "../enums/ColumnType.h"

namespace in_memory_sql_lld {

struct CellValue {
    ColumnType type{ColumnType::STRING}; // ⭐ TAG — kaunsa field valid hai
    std::string stringValue;             // type == STRING pe meaningful
    int intValue{0};                     // type == INT pe meaningful

    // ---- Safe factories: tag + data ek saath set (mismatch impossible) -----
    static CellValue fromString(const std::string& value) {
        return CellValue{ColumnType::STRING, value, 0};
    }

    static CellValue fromInt(int value) { return CellValue{ColumnType::INT, "", value}; }

    // ⭐ Type-safe comparison: pehle type match, phir sahi field ki value.
    // (INT 5 != STRING "5" — ye WHERE filter me zaroori hai.)
    bool equals(const CellValue& other) const {
        if (type != other.type) {
            return false;
        }
        return type == ColumnType::STRING ? stringValue == other.stringValue
                                          : intValue == other.intValue;
    }
};

}  // namespace in_memory_sql_lld

#endif
