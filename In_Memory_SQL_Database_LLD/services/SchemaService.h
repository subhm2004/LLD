#ifndef IN_MEMORY_SQL_LLD_SERVICES_SCHEMA_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_SCHEMA_SERVICE_H

#include <stdexcept>
#include <unordered_map>

#include "../models/Table.h"
#include "../validators/ColumnValueValidator.h"

namespace in_memory_sql_lld {

class SchemaService {
public:
    void createTable(const std::string& tableName, const std::vector<ColumnDefinition>& columns) {
        if (tableName.empty()) {
            throw std::invalid_argument("table name cannot be empty");
        }
        if (tables_.count(tableName) > 0) {
            throw std::runtime_error("table already exists: " + tableName);
        }
        validateColumnList(columns);
        tables_[tableName] = Table{tableName, columns, {}};
    }

    void updateTable(const std::string& tableName, const std::vector<ColumnDefinition>& columns) {
        Table& table = getTable(tableName);
        if (!table.rows.empty()) {
            throw std::runtime_error("cannot update schema while table has rows: " + tableName);
        }
        validateColumnList(columns);
        table.columns = columns;
    }

    void deleteTable(const std::string& tableName) {
        if (tables_.erase(tableName) == 0) {
            throw std::runtime_error("table not found: " + tableName);
        }
    }

    bool hasTable(const std::string& tableName) const {
        return tables_.count(tableName) > 0;
    }

    Table& getTable(const std::string& tableName) {
        auto it = tables_.find(tableName);
        if (it == tables_.end()) {
            throw std::runtime_error("table not found: " + tableName);
        }
        return it->second;
    }

    const Table& getTable(const std::string& tableName) const {
        return const_cast<SchemaService*>(this)->getTable(tableName);
    }

    std::vector<std::string> listTables() const {
        std::vector<std::string> names;
        for (const auto& [name, table] : tables_) {
            (void)table;
            names.push_back(name);
        }
        return names;
    }

private:
    static void validateColumnList(const std::vector<ColumnDefinition>& columns) {
        if (columns.empty()) {
            throw std::invalid_argument("table must have at least one column");
        }
        std::unordered_map<std::string, bool> seen;
        for (const ColumnDefinition& col : columns) {
            if (seen.count(col.name) > 0) {
                throw std::invalid_argument("duplicate column: " + col.name);
            }
            seen[col.name] = true;
            ColumnValueValidator::validateSchema(col);
        }
    }

    std::unordered_map<std::string, Table> tables_;
};

}  // namespace in_memory_sql_lld

#endif
