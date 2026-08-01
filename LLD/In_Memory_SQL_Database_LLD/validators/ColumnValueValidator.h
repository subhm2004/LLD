// ============================================================================
//  validators/ColumnValueValidator.h  —  Saare schema + data rules ek jagah
// ----------------------------------------------------------------------------
//  Ye class do ALAG waqt pe validate karti hai — aur yahi iska sabse important
//  design point hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO LEVEL KI VALIDATION (kab kya check hota hai)                       │
//  │                                                                          │
//  │  1. validateSchema(column)  — TABLE BANATE waqt (CREATE TABLE)            │
//  │     "Ye column definition khud sahi hai kya?"                            │
//  │       - naam khaali to nahi                                              │
//  │       - STRING column pe minValue? -> GALAT (wo int ke liye hai)         │
//  │       - INT column pe maxLength?  -> GALAT (wo string ke liye hai)       │
//  │       - maxLength positive honi chahiye                                  │
//  │                                                                          │
//  │  2. validateCell(column, value) — ROW INSERT karte waqt (INSERT)         │
//  │     "Ye value is column ke rules follow karti hai kya?"                 │
//  │       - type match (INT column me STRING nahi)                          │
//  │       - STRING: length <= maxLength                                     │
//  │       - INT: value >= minValue                                          │
//  │                                                                          │
//  │  ⭐ FAYDA: galat SCHEMA table banate waqt hi pakda jaata (data aane se    │
//  │  pehle), aur galat DATA insert pe. Dono galtiyan alag-alag waqt pe, par   │
//  │  RULES ek hi class me — kahin aur bikhri nahi. (Single Responsibility.)  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SAB `static` — ye stateless "pure rules" hain (input do, pass ya throw).
//     Object banane ki zaroorat nahi, aur akele test karna trivial.
//
//  ⭐ FAIL-FAST: rule toota to turant `invalid_argument` throw — koi bool return
//     nahi. Isse galat data database me GHUS hi nahi sakta, aur error message me
//     saaf likha hota hai kaunsa column aur kya problem.
//
//  📌 Naya constraint chahiye (jaise `maxValue`, `regex pattern`)? Sirf yahan
//     check add karo + ColumnDefinition me field — services untouched.
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_VALIDATORS_COLUMN_VALUE_VALIDATOR_H
#define IN_MEMORY_SQL_LLD_VALIDATORS_COLUMN_VALUE_VALIDATOR_H

#include <stdexcept>
#include <string>

#include "../models/CellValue.h"
#include "../models/ColumnDefinition.h"

namespace in_memory_sql_lld {

class ColumnValueValidator {
public:
    // ---- LEVEL 1: schema khud sahi hai? (CREATE/UPDATE TABLE ke waqt) ------
    static void validateSchema(const ColumnDefinition& column) {
        if (column.name.empty()) {
            throw std::invalid_argument("column name cannot be empty");
        }
        // ⭐ Type-specific constraint galat column pe to nahi lagi:
        if (column.type == ColumnType::STRING && column.minValue.has_value()) {
            throw std::invalid_argument("minValue applies only to int columns: " + column.name);
        }
        if (column.type == ColumnType::INT && column.maxLength.has_value()) {
            throw std::invalid_argument("maxLength applies only to string columns: " + column.name);
        }
        // maxLength 0 ya negative ka koi matlab nahi (kuch bhi store na hota).
        if (column.maxLength.has_value() && column.maxLength.value() <= 0) {
            throw std::invalid_argument("maxLength must be positive: " + column.name);
        }
    }

    // ---- LEVEL 2: value column ke rules follow karti hai? (INSERT ke waqt) --
    static void validateCell(const ColumnDefinition& column, const CellValue& value) {
        // Check 1: TYPE match (INT column me string daalne ki koshish?)
        if (value.type != column.type) {
            throw std::invalid_argument("type mismatch for column: " + column.name);
        }

        // Check 2 (STRING): lambai limit ke andar?
        if (column.type == ColumnType::STRING) {
            if (column.maxLength.has_value() &&
                static_cast<int>(value.stringValue.size()) > column.maxLength.value()) {
                throw std::invalid_argument("string too long for column: " + column.name);
            }
            return; // string ke liye bas itna
        }

        // Check 3 (INT): minimum se kam to nahi?
        if (column.minValue.has_value() && value.intValue < column.minValue.value()) {
            throw std::invalid_argument("int below minimum for column: " + column.name);
        }
    }
};

}  // namespace in_memory_sql_lld

#endif
