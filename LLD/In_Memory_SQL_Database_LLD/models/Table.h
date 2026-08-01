// ============================================================================
//  models/Table.h  —  Ek table = SCHEMA (columns) + DATA (rows)
// ----------------------------------------------------------------------------
//  Ek table do cheezein rakhta hai:
//     columns -> uska SCHEMA (kaunse columns, kya rules) — ordered vector
//     rows    -> asli DATA (har row ek map: columnName -> CellValue)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `Row = unordered_map<string, CellValue>` — row ek MAP kyun hai?       │
//  │                                                                          │
//  │  Fayda: OPTIONAL columns natural handle ho jaate. Agar "city" nahi diya,  │
//  │  to us row ke map me "city" key hi NAHI hogi — yaani "NULL" ka matlab     │
//  │  "key missing". Vector hota to har row me sab columns ke liye placeholder │
//  │  rakhna padta (aur "empty" vs "actually empty string" confuse hota).      │
//  │  Aur column ko naam se O(1) me nikal sakte hain.                          │
//  │                                                                          │
//  │  ⚠ Trade-off: map me har row me column NAAM repeat hota (memory) aur      │
//  │  ORDER nahi hota. Isi liye printing ke waqt hum `columns` vector ke order │
//  │  me ghoomte hain (QueryService::printRow) — taaki output ka column order  │
//  │  schema jaisa consistent rahe. Real DB me row ek compact array hota +     │
//  │  column-index mapping alag (memory efficient) — ye trade-off bolna accha. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `columns` VECTOR hai (map nahi) — kyunki column ORDER matter karta hai
//     (SELECT me kis order me chhape). Order chahiye + itne columns me linear
//     `findColumn` bilkul theek hai.
//
//  📌 `findColumn` nahi mila to `nullptr` — caller check karke "unknown column"
//     error deta hai (RecordService/QueryService me dekho).
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_MODELS_TABLE_H
#define IN_MEMORY_SQL_LLD_MODELS_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "CellValue.h"
#include "ColumnDefinition.h"

namespace in_memory_sql_lld {

// Ek row = column-naam se value ka map (missing key == NULL — upar note).
using Row = std::unordered_map<std::string, CellValue>;

struct Table {
    std::string name;                       // "users"
    std::vector<ColumnDefinition> columns;  // SCHEMA (order matters — printing)
    std::vector<Row> rows;                  // DATA (insert order me)

    // Column naam se dhoondho. Nahi mila -> nullptr (caller error deta hai).
    const ColumnDefinition* findColumn(const std::string& columnName) const {
        for (const ColumnDefinition& col : columns) {
            if (col.name == columnName) {
                return &col;
            }
        }
        return nullptr;
    }
};

}  // namespace in_memory_sql_lld

#endif
