#ifndef IN_MEMORY_SQL_LLD_MODELS_TABLE_H
#define IN_MEMORY_SQL_LLD_MODELS_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "CellValue.h"
#include "ColumnDefinition.h"

namespace in_memory_sql_lld {

using Row = std::unordered_map<std::string, CellValue>;

struct Table {
    std::string name;
    std::vector<ColumnDefinition> columns;
    std::vector<Row> rows;

    const ColumnDefinition* findColumn(const std::string& columnName) const {
        for (const ColumnDefinition& col : columns) {
            if (col.name == columnName) {
                return &col;
            }
        }
        return nullptr;
    }
};

}  // namespace in_memory_sql_lld

#endif
