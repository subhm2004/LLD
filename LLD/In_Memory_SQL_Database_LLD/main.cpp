// ============================================================================
//  main.cpp  —  In-Memory SQL Database ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Poora SQL-jaisa flow chala ke dikhata hai:
//    1. CREATE TABLE (typed columns + constraints)
//    2. INSERT (valid rows)
//    3. Constraint violations REJECT (min value, max length)
//    4. SELECT * (print all — missing optional column "NULL" dikhta)
//    5. WHERE filter (string aur int dono pe)
//    6. ALTER TABLE (sirf khaali table pe allowed)
//    7. DROP TABLE + SHOW TABLES
//
//  ⭐ Client ko sirf `InMemoryDatabase` dikhta hai (Facade) — Schema/Record/
//     Query services, validators, storage — kuch nahi. Bas SQL-jaise methods.
//
//  📌 Includes targeted hain (bits/stdc++.h nahi) — ye folder already clean tha 👍
// ============================================================================
#include <iostream>
#include <optional>
#include <string>

#include "core/InMemoryDatabase.h"
#include "enums/ColumnType.h"
#include "models/CellValue.h"
#include "models/ColumnDefinition.h"

using namespace std;
using namespace in_memory_sql_lld;

int main() {
    cout << "====== In-Memory SQL-like Database LLD Demo ======\n\n";

    InMemoryDatabase db;

    // ---- 1. CREATE TABLE --------------------------------------------------
    // ColumnDefinition field order: {name, type, required, maxLength, minValue}
    //   id   -> INT,    required, min 1024   (maxLength = nullopt, int pe nahi lagti)
    //   name -> STRING, required, max 20     (minValue = nullopt, string pe nahi lagti)
    //   city -> STRING, OPTIONAL, max 20     (na do to row me NULL dikhega)
    cout << "--- Create table users ---\n";
    db.createTable("users", {
        ColumnDefinition{"id", ColumnType::INT, true, nullopt, 1024},
        ColumnDefinition{"name", ColumnType::STRING, true, 20, nullopt},
        ColumnDefinition{"city", ColumnType::STRING, false, 20, nullopt},
    });
    cout << "  Tables: users created\n";

    // ---- 2. INSERT (valid rows) -------------------------------------------
    cout << "\n--- Insert records ---\n";
    // Alice: city NAHI diya (optional) -> print me NULL aayega
    db.insert("users", {{"id", CellValue::fromInt(2000)}, {"name", CellValue::fromString("Alice")}});
    db.insert("users",
              {{"id", CellValue::fromInt(2048)}, {"name", CellValue::fromString("Bob")},
               {"city", CellValue::fromString("Delhi")}});
    db.insert("users",
              {{"id", CellValue::fromInt(3000)}, {"name", CellValue::fromString("Carol")},
               {"city", CellValue::fromString("Mumbai")}});
    cout << "  3 rows inserted\n";

    // ---- 3. Constraint violations -> REJECT -------------------------------
    // (a) id = 500 < minValue 1024 -> validator error
    try {
        db.insert("users", {{"id", CellValue::fromInt(500)}, {"name", CellValue::fromString("Dan")}});
        cout << "  ERROR: id below min should fail\n";
    } catch (const exception& ex) {
        cout << "  Rejected id < 1024: " << ex.what() << "\n";
    }

    // (b) name 20 chars se lamba -> validator error
    try {
        db.insert("users",
                  {{"id", CellValue::fromInt(4000)},
                   {"name", CellValue::fromString("ThisNameIsWayTooLongForTwenty")}});
        cout << "  ERROR: long name should fail\n";
    } catch (const exception& ex) {
        cout << "  Rejected long string: " << ex.what() << "\n";
    }

    // ---- 4. SELECT * -------------------------------------------------------
    cout << "\n--- Print all records ---\n";
    db.printAll("users"); // Alice ki city "NULL" dikhegi (optional, di hi nahi thi)

    // ---- 5. WHERE filters (string + int) ----------------------------------
    cout << "\n--- Filter: city = Mumbai ---\n";
    db.printWhere("users", "city", CellValue::fromString("Mumbai"));

    cout << "\n--- Filter: id = 2000 ---\n";
    db.printWhere("users", "id", CellValue::fromInt(2000));

    // ---- 6. ALTER TABLE — sirf KHAALI table pe allowed --------------------
    // logs abhi khaali hai, isliye schema update chal jaayega (min 0 -> 1024).
    cout << "\n--- Update table schema (empty table) ---\n";
    db.createTable("logs", {ColumnDefinition{"code", ColumnType::INT, true, nullopt, 0}});
    db.updateTable("logs", {ColumnDefinition{"code", ColumnType::INT, true, nullopt, 1024}});
    cout << "  logs schema updated (min int now 1024)\n";

    // ---- 7. DROP TABLE + SHOW TABLES --------------------------------------
    cout << "\n--- Delete table ---\n";
    db.deleteTable("logs");
    cout << "  logs deleted. Remaining tables: ";
    for (const string& t : db.listTables()) {
        cout << t << " ";
    }
    cout << "\n";

    cout << "\nDemo complete.\n";
    return 0;
}
