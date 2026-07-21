// ============================================================================
//  enums/ColumnType.h  —  Column kis TYPE ka data rakhta hai
// ----------------------------------------------------------------------------
//  Is mini-database me do hi data types support hain:
//     STRING -> text ("Alice", "Delhi")
//     INT    -> whole number (2048, 3000)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TYPE hi poore SCHEMA ENFORCEMENT ka aadhaar hai                       │
//  │                                                                          │
//  │  Har column ka ek type fix hota (ColumnDefinition me), aur har cell ka    │
//  │  bhi apna type hota (CellValue me). Insert ke waqt validator dono match   │
//  │  karta hai — INT column me string daalne ki koshish = error.             │
//  │  Yahi "typed schema" asli SQL databases ka core idea hai.                │
//  │                                                                          │
//  │  Aur type ke hisaab se ALAG constraints lagti hain:                       │
//  │     STRING -> maxLength (kitna lamba)                                     │
//  │     INT    -> minValue  (kam se kam kitna)                                │
//  │  (ColumnDefinition.h + ColumnValueValidator.h me ye rules hain.)         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Real SQL me bahut types hote (VARCHAR, DECIMAL, DATE, BOOLEAN...). Yahan
//     interview-scope me 2 rakhe — design ka point type-system dikhana hai, na ki
//     poora SQL. Naya type add karna = enum me entry + validator me uske rules.
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_ENUMS_COLUMN_TYPE_H
#define IN_MEMORY_SQL_LLD_ENUMS_COLUMN_TYPE_H

#include <string>

namespace in_memory_sql_lld {

enum class ColumnType { STRING, INT };

// Type ko padhne-layak string me (schema printing/errors ke liye).
inline std::string columnTypeToString(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return "string";
        case ColumnType::INT:
            return "int";
        default:
            return "unknown";
    }
}

}  // namespace in_memory_sql_lld

#endif
