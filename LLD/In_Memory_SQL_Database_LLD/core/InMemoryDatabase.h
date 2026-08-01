// ============================================================================
//  core/InMemoryDatabase.h  —  Poore database ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client ke liye "database" hai. Andar 3 services (Schema/Record/
//  Query) ka kaam chalta hai, par bahar se client ko SQL-jaise saaf methods
//  dikhte hain: createTable, insert, printAll, selectWhere, deleteTable.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE — SQL ke 3 "families" ko ek interface me                       │
//  │                                                                          │
//  │     DDL (structure) -> SchemaService  : createTable/updateTable/delete    │
//  │     DML (write)     -> RecordService  : insert                            │
//  │     Query (read)    -> QueryService   : printAll/selectWhere              │
//  │                                                                          │
//  │  Client ko ye batwara pata nahi — wo bas `db.createTable(...)`,           │
//  │  `db.insert(...)`, `db.selectWhere(...)` bulata hai. Facade har call ko   │
//  │  sahi service pe forward kar deta hai (patli delegation layer).          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ WIRING: services ko schema_ ka POINTER milta hai                      │
//  │                                                                          │
//  │     InMemoryDatabase() : recordService_(&schema_), queryService_(&schema_)│
//  │                                                                          │
//  │  Tables ki MAALIK sirf `schema_` hai. Baaki dono services usi ko point    │
//  │  karti hain — isse data ek jagah rehta (single source of truth), do       │
//  │  copies ka mismatch kabhi nahi hota.                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 COPY BUG JO THI + FIX — "self-referential members" wala classic trap  │
//  │                                                                          │
//  │  PROBLEM: `recordService_`/`queryService_` APNE HI object ke member       │
//  │  (`schema_`) ka pointer rakhte hain. Ab agar database COPY ho jaye:      │
//  │                                                                          │
//  │      InMemoryDatabase db2 = db1;   // implicit copy constructor           │
//  │                                                                          │
//  │  Compiler ka default copy har member ko copy karta — including POINTER    │
//  │  ki VALUE. Yaani db2 ke services ab bhi **db1 ke schema_** ko point karte │
//  │  hain! Natija:                                                           │
//  │     - db2.insert(...) chup-chaap db1 ke tables me likh dega 😱            │
//  │     - db1 destroy ho jaye to db2 ke services DANGLING pointer pakde rahenge│
//  │                                                                          │
//  │  ✅ FIX: copy/move ko `= delete` kar diya. Ab galti se copy karne pe      │
//  │  COMPILE ERROR milega (silent data-corruption ke bajaye).                │
//  │                                                                          │
//  │  💡 Alternative fix: apna copy constructor likho jo copy ke services ko   │
//  │     NAYE (apne) schema_ pe re-point kare. Par is design me database copy  │
//  │     karne ki zaroorat hi nahi, isliye `= delete` sabse saaf hai.         │
//  │  📌 SABAK: jab koi member apne hi bhai-member ka pointer/reference rakhe, │
//  │     to default copy HAMESHA galat hota hai — copy/move pe socho.         │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef IN_MEMORY_SQL_LLD_CORE_IN_MEMORY_DATABASE_H
#define IN_MEMORY_SQL_LLD_CORE_IN_MEMORY_DATABASE_H

#include <string>
#include <vector>

#include "../models/CellValue.h"
#include "../models/ColumnDefinition.h"
#include "../models/Table.h"
#include "../services/QueryService.h"
#include "../services/RecordService.h"
#include "../services/SchemaService.h"

namespace in_memory_sql_lld {

class InMemoryDatabase {
public:
    // ⭐ Services ko schema_ ka pointer do (tables ki maalik wahi hai).
    InMemoryDatabase() : recordService_(&schema_), queryService_(&schema_) {}

    // ⭐ Copy/move BAND — warna copy ke services ORIGINAL ke schema_ ko point
    //    karte rehte (upar 🐛 note padho). Compile-error > silent corruption.
    InMemoryDatabase(const InMemoryDatabase&) = delete;
    InMemoryDatabase& operator=(const InMemoryDatabase&) = delete;
    InMemoryDatabase(InMemoryDatabase&&) = delete;
    InMemoryDatabase& operator=(InMemoryDatabase&&) = delete;

    // ---- DDL: structure (SchemaService ko delegate) ------------------------
    void createTable(const std::string& tableName,
                     const std::vector<ColumnDefinition>& columns) {
        schema_.createTable(tableName, columns);
    }

    void updateTable(const std::string& tableName,
                     const std::vector<ColumnDefinition>& columns) {
        schema_.updateTable(tableName, columns); // sirf khaali table pe (SchemaService rule)
    }

    void deleteTable(const std::string& tableName) { schema_.deleteTable(tableName); }

    // ---- DML: write (RecordService ko delegate) ---------------------------
    void insert(const std::string& tableName, const Row& row) {
        recordService_.insert(tableName, row);
    }

    // ---- Query: read (QueryService ko delegate) ---------------------------
    void printAll(const std::string& tableName) const { queryService_.printAll(tableName); }

    void printWhere(const std::string& tableName, const std::string& columnName,
                    const CellValue& value) const {
        queryService_.printFilter(tableName, columnName, value);
    }

    // SELECT ... WHERE — rows lauta do (print ke bajaye data chahiye to).
    std::vector<Row> selectWhere(const std::string& tableName, const std::string& columnName,
                                 const CellValue& value) const {
        return queryService_.filterEquals(tableName, columnName, value);
    }

    bool hasTable(const std::string& tableName) const { return schema_.hasTable(tableName); }
    std::vector<std::string> listTables() const { return schema_.listTables(); } // SHOW TABLES

private:
    SchemaService schema_;         // ⭐ tables ka asli maalik (single source of truth)
    RecordService recordService_;  // &schema_ point karta hai
    QueryService queryService_;    // &schema_ point karta hai
};

}  // namespace in_memory_sql_lld

#endif
