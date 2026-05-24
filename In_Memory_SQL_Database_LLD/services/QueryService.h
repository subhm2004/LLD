#ifndef IN_MEMORY_SQL_LLD_SERVICES_QUERY_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_QUERY_SERVICE_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "../models/CellValue.h"
#include "SchemaService.h"

namespace in_memory_sql_lld {

class QueryService {
public:
    explicit QueryService(SchemaService* schema) : schema_(schema) {}

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

    std::vector<Row> filterEquals(const std::string& tableName, const std::string& columnName,
                                  const CellValue& expected) const {
        const Table& table = schema_->getTable(tableName);
        const ColumnDefinition* column = table.findColumn(columnName);
        if (column == nullptr) {
            throw std::invalid_argument("unknown column: " + columnName);
        }
        ColumnValueValidator::validateCell(*column, expected);

        std::vector<Row> matches;
        for (const Row& row : table.rows) {
            auto it = row.find(columnName);
            if (it != row.end() && it->second.equals(expected)) {
                matches.push_back(row);
            }
        }
        return matches;
    }

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
            underline << std::string(col.name.size(), '-');
        }
        std::cout << "  " << header.str() << "\n";
        std::cout << "  " << underline.str() << "\n";
    }

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
                line << "NULL";
            } else {
                line << formatValue(it->second);
            }
        }
        std::cout << "  " << line.str() << "\n";
    }

    static std::string formatValue(const CellValue& value) {
        return value.type == ColumnType::STRING ? value.stringValue : std::to_string(value.intValue);
    }

    SchemaService* schema_;
};

}  // namespace in_memory_sql_lld

#endif
