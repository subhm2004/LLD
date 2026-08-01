// ============================================================================
//  services/RecordService.h  —  Rows insert karo (DML — INSERT operation)
// ----------------------------------------------------------------------------
//  SQL ki bhasha me ye "DML" (Data Manipulation Language) hai. Abhi sirf INSERT
//  hai; UPDATE/DELETE natural extensions hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ INSERT ke DO PASS — dono zaroori hain                                 │
//  │                                                                          │
//  │  PASS 1: SCHEMA ke columns pe ghoomo ("jo hona chahiye wo hai kya?")     │
//  │     - column row me nahi hai?                                            │
//  │         required  -> ERROR (missing required column)                     │
//  │         optional  -> skip (row me wo key hi nahi hogi = NULL)            │
//  │     - hai to validate karo (type/length/min) + `normalized` me daalo      │
//  │                                                                          │
//  │  PASS 2: INPUT row ke keys pe ghoomo ("jo diya hai wo valid hai kya?")   │
//  │     - koi aisi key jo schema me hai hi nahi -> ERROR (unknown column)    │
//  │                                                                          │
//  │  ⭐ DO PASS KYUN? Pass 1 akela "unknown column" nahi pakad sakta (wo to   │
//  │  schema ke columns pe ghoom raha hai — extra keys dikhengi hi nahi).     │
//  │  Aur pass 2 akela "missing required" nahi pakad sakta. Dono milke poori   │
//  │  validation dete hain: "sab zaroori hai" AUR "kuch extra nahi".          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `normalized` row kyun banate hain (input row seedha push kyun nahi)?
//     Kyunki normalized me SIRF wahi columns jaate hain jo schema me hain aur
//     validate ho chuke hain. Isse table me kabhi koi ganda/extra data nahi
//     jaata — ye ek "saaf ki hui" copy hai. (Waise pass 2 unknown reject karta
//     hai, par normalized banana defensive + clear intent hai.)
//
//  📌 Ye service SchemaService ka POINTER rakhti hai — tables ki maalik wo hai,
//     ye bas usse table maangke uski rows me push karti hai (single source of truth).
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_SERVICES_RECORD_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_RECORD_SERVICE_H

#include <stdexcept>
#include <string>

#include "../validators/ColumnValueValidator.h"
#include "SchemaService.h"

namespace in_memory_sql_lld {

class RecordService {
public:
    // Schema service ka pointer inject (tables ki maalik wahi hai).
    explicit RecordService(SchemaService* schema) : schema_(schema) {}

    void insert(const std::string& tableName, const Row& row) {
        Table& table = schema_->getTable(tableName); // nahi mili to throw
        Row normalized;                              // sirf valid + known columns

        // ---- PASS 1: schema ke columns pe ghoomo (required + validate) -----
        for (const ColumnDefinition& col : table.columns) {
            auto it = row.find(col.name);
            if (it == row.end()) {
                if (col.required) {
                    throw std::invalid_argument("missing required column: " + col.name);
                }
                continue; // optional missing -> row me key hi nahi (NULL)
            }
            ColumnValueValidator::validateCell(col, it->second); // type/length/min
            normalized[col.name] = it->second;
        }

        // ---- PASS 2: input keys pe ghoomo (unknown column pakdo) ----------
        for (const auto& [key, value] : row) {
            (void)value; // value chahiye nahi, sirf key check karni hai
            if (table.findColumn(key) == nullptr) {
                throw std::invalid_argument("unknown column: " + key);
            }
        }

        table.rows.push_back(normalized); // sab checks paas -> ab insert
    }

private:
    SchemaService* schema_; // tables ka owner (ye service usse maangti hai)
};

}  // namespace in_memory_sql_lld

#endif
