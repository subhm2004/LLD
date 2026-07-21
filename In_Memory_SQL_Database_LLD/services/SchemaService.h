// ============================================================================
//  services/SchemaService.h  —  Tables ka SCHEMA manage karo (DDL operations)
// ----------------------------------------------------------------------------
//  SQL ki bhasha me ye "DDL" (Data Definition Language) hai — CREATE TABLE,
//  ALTER TABLE, DROP TABLE. Ye service tables ko OWN karti hai (storage) aur
//  unka structure manage karti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ YE SERVICE TABLES KI MAALIK HAI (single source of truth)             │
//  │                                                                          │
//  │  `tables_` map yahan hai — RecordService aur QueryService khud tables     │
//  │  nahi rakhte, wo is service ka POINTER leke usse tables maangte hain.     │
//  │  Isse data ek jagah rehta (do copies ka mismatch nahi hota).             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SABSE AHEM RULE: `updateTable` sirf KHAALI table pe                   │
//  │                                                                          │
//  │  Agar table me rows already hain aur hum schema badal dein (jaise column  │
//  │  hata dein ya type badal dein), to purani rows us naye schema ko follow   │
//  │  nahi karengi -> data corrupt/inconsistent. Isliye:                       │
//  │     rows non-empty -> updateTable REJECT (error)                          │
//  │                                                                          │
//  │  Real DB me ALTER TABLE hota hai par wo purane data ko MIGRATE karta hai  │
//  │  (default values bharna, type convert karna) — wo bada kaam hai. Yahan    │
//  │  simple + safe rule: khaali table hi badlo. 📌 Interview me ye trade-off  │
//  │  bolna accha point hai.                                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `validateColumnList` do cheez check karta hai: (a) DUPLICATE column naam
//     (`seen` map se) — SQL me do same-naam columns allowed nahi, aur (b) har
//     column ka apna schema valid (ColumnValueValidator ko delegate).
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_SERVICES_SCHEMA_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_SCHEMA_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Table.h"
#include "../validators/ColumnValueValidator.h"

namespace in_memory_sql_lld {

class SchemaService {
public:
    // ---- CREATE TABLE ------------------------------------------------------
    void createTable(const std::string& tableName, const std::vector<ColumnDefinition>& columns) {
        if (tableName.empty()) {
            throw std::invalid_argument("table name cannot be empty");
        }
        if (tables_.count(tableName) > 0) {
            throw std::runtime_error("table already exists: " + tableName); // duplicate reject
        }
        validateColumnList(columns);                    // schema pehle validate (fail-fast)
        tables_[tableName] = Table{tableName, columns, {}}; // rows khaali se shuru
    }

    // ---- ALTER TABLE — ⭐ sirf KHAALI table pe (upar note) -----------------
    void updateTable(const std::string& tableName, const std::vector<ColumnDefinition>& columns) {
        Table& table = getTable(tableName);
        if (!table.rows.empty()) {
            // Purani rows naye schema ko follow nahi karengi -> data corrupt hota.
            throw std::runtime_error("cannot update schema while table has rows: " + tableName);
        }
        validateColumnList(columns);
        table.columns = columns;
    }

    // ---- DROP TABLE --------------------------------------------------------
    void deleteTable(const std::string& tableName) {
        // erase() hataye gaye elements ki count deta hai — 0 matlab tha hi nahi.
        if (tables_.erase(tableName) == 0) {
            throw std::runtime_error("table not found: " + tableName);
        }
    }

    bool hasTable(const std::string& tableName) const { return tables_.count(tableName) > 0; }

    // Table nikaalo (modify karne ke liye). Nahi mila -> error (fail-fast).
    Table& getTable(const std::string& tableName) {
        auto it = tables_.find(tableName);
        if (it == tables_.end()) {
            throw std::runtime_error("table not found: " + tableName);
        }
        return it->second;
    }

    // Read-only version. ⭐ const_cast se non-const version reuse kiya taaki
    // wahi lookup+error logic DO baar na likhna pade (classic C++ idiom).
    // Safe hai: object truly const nahi, aur hum sirf const& lauta rahe hain.
    const Table& getTable(const std::string& tableName) const {
        return const_cast<SchemaService*>(this)->getTable(tableName);
    }

    // Saare table naam (SHOW TABLES jaisa).
    std::vector<std::string> listTables() const {
        std::vector<std::string> names;
        for (const auto& [name, table] : tables_) {
            (void)table; // structured binding me table use nahi kar rahe — warning silence
            names.push_back(name);
        }
        return names;
    }

private:
    // Column list valid hai? (duplicate naam + har column ka apna schema)
    static void validateColumnList(const std::vector<ColumnDefinition>& columns) {
        if (columns.empty()) {
            throw std::invalid_argument("table must have at least one column");
        }
        std::unordered_map<std::string, bool> seen; // duplicate detect karne ke liye
        for (const ColumnDefinition& col : columns) {
            if (seen.count(col.name) > 0) {
                throw std::invalid_argument("duplicate column: " + col.name);
            }
            seen[col.name] = true;
            ColumnValueValidator::validateSchema(col); // rules validator ko delegate
        }
    }

    std::unordered_map<std::string, Table> tables_; // ⭐ tables ka asli ghar (single source)
};

}  // namespace in_memory_sql_lld

#endif
