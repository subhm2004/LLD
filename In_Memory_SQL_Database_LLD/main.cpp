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

    cout << "--- Create table users ---\n";
    db.createTable("users", {
        ColumnDefinition{"id", ColumnType::INT, true, nullopt, 1024},
        ColumnDefinition{"name", ColumnType::STRING, true, 20, nullopt},
        ColumnDefinition{"city", ColumnType::STRING, false, 20, nullopt},
    });
    cout << "  Tables: users created\n";

    cout << "\n--- Insert records ---\n";
    db.insert("users", {{"id", CellValue::fromInt(2000)}, {"name", CellValue::fromString("Alice")}});
    db.insert("users",
              {{"id", CellValue::fromInt(2048)}, {"name", CellValue::fromString("Bob")},
               {"city", CellValue::fromString("Delhi")}});
    db.insert("users",
              {{"id", CellValue::fromInt(3000)}, {"name", CellValue::fromString("Carol")},
               {"city", CellValue::fromString("Mumbai")}});
    cout << "  3 rows inserted\n";

    try {
        db.insert("users", {{"id", CellValue::fromInt(500)}, {"name", CellValue::fromString("Dan")}});
        cout << "  ERROR: id below min should fail\n";
    } catch (const exception& ex) {
        cout << "  Rejected id < 1024: " << ex.what() << "\n";
    }

    try {
        db.insert("users",
                  {{"id", CellValue::fromInt(4000)},
                   {"name", CellValue::fromString("ThisNameIsWayTooLongForTwenty")}});
        cout << "  ERROR: long name should fail\n";
    } catch (const exception& ex) {
        cout << "  Rejected long string: " << ex.what() << "\n";
    }

    cout << "\n--- Print all records ---\n";
    db.printAll("users");

    cout << "\n--- Filter: city = Mumbai ---\n";
    db.printWhere("users", "city", CellValue::fromString("Mumbai"));

    cout << "\n--- Filter: id = 2000 ---\n";
    db.printWhere("users", "id", CellValue::fromInt(2000));

    cout << "\n--- Update table schema (empty table) ---\n";
    db.createTable("logs", {ColumnDefinition{"code", ColumnType::INT, true, nullopt, 0}});
    db.updateTable("logs", {ColumnDefinition{"code", ColumnType::INT, true, nullopt, 1024}});
    cout << "  logs schema updated (min int now 1024)\n";

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
