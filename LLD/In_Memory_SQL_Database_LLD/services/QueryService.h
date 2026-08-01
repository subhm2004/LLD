// ============================================================================
//  services/QueryService.h  —  Rows padho + chhaapo (SELECT / WHERE)
// ----------------------------------------------------------------------------
//  SQL ka "read" side: saari rows dikhao (SELECT *), ya ek condition pe filter
//  karo (WHERE col = value). Ye service kuch BADALTI nahi — sirf padhti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `filterEquals` = SQL ka `WHERE column = value`                       │
//  │                                                                          │
//  │  Teen step:                                                              │
//  │   1. Column exist karta hai? nahi -> "unknown column" error              │
//  │   2. Di gayi value us column ke type/rules follow karti hai? (validate)  │
//  │      ⭐ Ye zaroori hai! Warna koi INT column me STRING "abc" se filter    │
//  │      karta aur chup-chaap 0 results milte (galat) — ab saaf error milta. │
//  │   3. Har row pe `CellValue::equals` (type-safe compare) -> matches       │
//  │                                                                          │
//  │  ⚠ Ye FULL TABLE SCAN hai (O(N) har query pe). Real DB me INDEX hota     │
//  │  (B-tree/hash) jisse ye O(log N)/O(1) ho jaata. Interview me ye bolna    │
//  │  accha point hai: "abhi scan hai, index add karna natural extension".    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PRINTING: rows MAP hain (order nahi), par output me column ORDER      │
//  │     consistent chahiye                                                    │
//  │                                                                          │
//  │  Isliye `printRow` row ke map pe nahi, `table.columns` VECTOR pe ghoomta  │
//  │  hai — aur har column ka value row me dhoondhta hai. Isse har row me      │
//  │  columns usi order me chhapte hain jaise schema me hain (header ke saath  │
//  │  aligned). Agar map pe ghoomte to har row ka order alag-alag hota! 😵     │
//  │                                                                          │
//  │  Aur jo column row me nahi mila (optional/missing) -> "NULL" print.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye service bhi SchemaService ka pointer rakhti hai (tables usi ke paas).
//     Sab methods `const` — query kuch modify nahi karti (read-only intent saaf).
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_SERVICES_QUERY_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_QUERY_SERVICE_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../models/CellValue.h"
#include "SchemaService.h"

namespace in_memory_sql_lld {

class QueryService {
public:
    explicit QueryService(SchemaService* schema) : schema_(schema) {}

    // ---- SELECT * — saari rows chhaapo ------------------------------------
    void printAll(const std::string& tableName) const {
        const Table& table = schema_->getTable(tableName);
        printHeader(table);
        if (table.rows.empty()) {
            std::cout << "  (no rows)\n";
            return;
        }
        for (const Row& row : table.rows) {
            printRow(table, row);
        }
    }

    // ---- WHERE column = value — matching rows lauta do --------------------
    std::vector<Row> filterEquals(const std::string& tableName, const std::string& columnName,
                                  const CellValue& expected) const {
        const Table& table = schema_->getTable(tableName);

        // Step 1: column exist karta hai?
        const ColumnDefinition* column = table.findColumn(columnName);
        if (column == nullptr) {
            throw std::invalid_argument("unknown column: " + columnName);
        }
        // Step 2: ⭐ filter value bhi us column ke rules follow kare (upar note)
        ColumnValueValidator::validateCell(*column, expected);

        // Step 3: scan + type-safe compare
        std::vector<Row> matches;
        for (const Row& row : table.rows) {
            auto it = row.find(columnName);
            if (it != row.end() && it->second.equals(expected)) {
                matches.push_back(row);
            }
        }
        return matches;
    }

    // ---- WHERE + print (filterEquals ka display wrapper) ------------------
    void printFilter(const std::string& tableName, const std::string& columnName,
                     const CellValue& expected) const {
        const std::vector<Row> matches = filterEquals(tableName, columnName, expected);
        const Table& table = schema_->getTable(tableName);
        std::cout << "  WHERE " << columnName << " = " << formatValue(expected) << " ("
                  << matches.size() << " row(s))\n";
        printHeader(table);
        if (matches.empty()) {
            std::cout << "  (no matching rows)\n";
            return;
        }
        for (const Row& row : matches) {
            printRow(table, row);
        }
    }

private:
    // Column naam + underline (schema order me).
    static void printHeader(const Table& table) {
        std::ostringstream header;
        std::ostringstream underline;
        bool first = true;
        for (const ColumnDefinition& col : table.columns) {
            if (!first) {
                header << " | ";
                underline << " | ";
            }
            first = false;
            header << col.name;
            underline << std::string(col.name.size(), '-'); // naam jitni lambi dashes
        }
        std::cout << "  " << header.str() << "\n";
        std::cout << "  " << underline.str() << "\n";
    }

    // ⭐ Row ke MAP pe nahi, table ke COLUMNS pe ghoomte hain — taaki order
    //    header jaisa consistent rahe (upar note). Missing column -> "NULL".
    static void printRow(const Table& table, const Row& row) {
        std::ostringstream line;
        bool first = true;
        for (const ColumnDefinition& col : table.columns) {
            if (!first) {
                line << " | ";
            }
            first = false;
            auto it = row.find(col.name);
            if (it == row.end()) {
                line << "NULL"; // optional column jo insert me nahi diya gaya
            } else {
                line << formatValue(it->second);
            }
        }
        std::cout << "  " << line.str() << "\n";
    }

    // CellValue -> printable string (tag dekh ke sahi field).
    static std::string formatValue(const CellValue& value) {
        return value.type == ColumnType::STRING ? value.stringValue : std::to_string(value.intValue);
    }

    SchemaService* schema_; // tables ka owner (read-only use)
};

}  // namespace in_memory_sql_lld

#endif
