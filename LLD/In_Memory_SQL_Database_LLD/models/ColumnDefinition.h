// ============================================================================
//  models/ColumnDefinition.h  —  Ek column ka SCHEMA (rules kya hain)
// ----------------------------------------------------------------------------
//  Ye batata hai ek column kaisa hona chahiye: uska naam, type, zaroori hai ya
//  nahi, aur uske CONSTRAINTS. Yahi "CREATE TABLE" wali definition hai.
//
//     ColumnDefinition{"id",   INT,    true,  nullopt, 1024}  // required, min 1024
//     ColumnDefinition{"name", STRING, true,  20,      nullopt} // required, max 20 chars
//     ColumnDefinition{"city", STRING, false, 20,      nullopt} // optional
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::optional` KYUN (plain int ki jagah)?                            │
//  │                                                                          │
//  │  Constraint ka hona ya na hona batana hai. Agar plain `int maxLength`     │
//  │  hota, to "0 ka matlab koi limit nahi" jaisa jugaad karna padta —         │
//  │  confusing (0 asli limit bhi to ho sakti hai!).                          │
//  │                                                                          │
//  │  `optional<int>` saaf batata hai:                                        │
//  │     has_value() == false -> ye constraint hai HI NAHI                    │
//  │     has_value() == true  -> constraint lagegi, value() se limit lo       │
//  │  "Value nahi hai" aur "value 0 hai" — dono alag cheez, aur optional ye    │
//  │  farak clearly express karta hai.                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ TYPE-SPECIFIC constraints: `maxLength` sirf STRING ke liye, `minValue` sirf
//     INT ke liye. Galat combination (INT column pe maxLength) ColumnValueValidator
//     ::validateSchema me REJECT hota hai — table banate waqt hi pakda jaata.
//
//  📌 `required` = SQL ka "NOT NULL". Insert me ye column missing ho to error.
//     Optional column missing ho to row me wo key hi nahi hoti -> print me "NULL".
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_MODELS_COLUMN_DEFINITION_H
#define IN_MEMORY_SQL_LLD_MODELS_COLUMN_DEFINITION_H

#include <optional>
#include <string>

#include "../enums/ColumnType.h"

namespace in_memory_sql_lld {

struct ColumnDefinition {
    std::string name;                    // "id", "name", "city"
    ColumnType type{ColumnType::STRING}; // STRING ya INT
    bool required{false};                // SQL ka NOT NULL
    std::optional<int> maxLength;        // ⭐ sirf STRING columns ke liye
    std::optional<int> minValue;         // ⭐ sirf INT columns ke liye
};

}  // namespace in_memory_sql_lld

#endif
