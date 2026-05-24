#ifndef IN_MEMORY_SQL_LLD_SERVICES_RECORD_SERVICE_H
#define IN_MEMORY_SQL_LLD_SERVICES_RECORD_SERVICE_H

#include <stdexcept>

#include "../validators/ColumnValueValidator.h"
#include "SchemaService.h"

namespace in_memory_sql_lld {

class RecordService {
public:
    explicit RecordService(SchemaService* schema) : schema_(schema) {}

    void insert(const std::string& tableName, const Row& row) {
        Table& table = schema_->getTable(tableName);
        Row normalized;

        for (const ColumnDefinition& col : table.columns) {
            auto it = row.find(col.name);
            if (it == row.end()) {
                if (col.required) {
                    throw std::invalid_argument("missing required column: " + col.name);
                }
                continue;
            }
            ColumnValueValidator::validateCell(col, it->second);
            normalized[col.name] = it->second;
        }

        for (const auto& [key, value] : row) {
            (void)value;
            if (table.findColumn(key) == nullptr) {
                throw std::invalid_argument("unknown column: " + key);
            }
        }

        table.rows.push_back(normalized);
    }

private:
    SchemaService* schema_;
};

}  // namespace in_memory_sql_lld

#endif
