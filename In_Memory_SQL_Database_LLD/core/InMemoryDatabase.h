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
    InMemoryDatabase() : recordService_(&schema_), queryService_(&schema_) {}

    void createTable(const std::string& tableName,
                     const std::vector<ColumnDefinition>& columns) {
        schema_.createTable(tableName, columns);
    }

    void updateTable(const std::string& tableName,
                     const std::vector<ColumnDefinition>& columns) {
        schema_.updateTable(tableName, columns);
    }

    void deleteTable(const std::string& tableName) { schema_.deleteTable(tableName); }

    void insert(const std::string& tableName, const Row& row) {
        recordService_.insert(tableName, row);
    }

    void printAll(const std::string& tableName) const { queryService_.printAll(tableName); }

    void printWhere(const std::string& tableName, const std::string& columnName,
                    const CellValue& value) const {
        queryService_.printFilter(tableName, columnName, value);
    }

    std::vector<Row> selectWhere(const std::string& tableName, const std::string& columnName,
                                 const CellValue& value) const {
        return queryService_.filterEquals(tableName, columnName, value);
    }

    bool hasTable(const std::string& tableName) const { return schema_.hasTable(tableName); }

    std::vector<std::string> listTables() const { return schema_.listTables(); }

private:
    SchemaService schema_;
    RecordService recordService_;
    QueryService queryService_;
};

}  // namespace in_memory_sql_lld

#endif
