#ifndef IN_MEMORY_SQL_LLD_VALIDATORS_COLUMN_VALUE_VALIDATOR_H
#define IN_MEMORY_SQL_LLD_VALIDATORS_COLUMN_VALUE_VALIDATOR_H

#include <stdexcept>
#include <string>

#include "../models/CellValue.h"
#include "../models/ColumnDefinition.h"

namespace in_memory_sql_lld {

class ColumnValueValidator {
public:
    static void validateSchema(const ColumnDefinition& column) {
        if (column.name.empty()) {
            throw std::invalid_argument("column name cannot be empty");
        }
        if (column.type == ColumnType::STRING && column.minValue.has_value()) {
            throw std::invalid_argument("minValue applies only to int columns: " + column.name);
        }
        if (column.type == ColumnType::INT && column.maxLength.has_value()) {
            throw std::invalid_argument("maxLength applies only to string columns: " + column.name);
        }
        if (column.maxLength.has_value() && column.maxLength.value() <= 0) {
            throw std::invalid_argument("maxLength must be positive: " + column.name);
        }
    }

    static void validateCell(const ColumnDefinition& column, const CellValue& value) {
        if (value.type != column.type) {
            throw std::invalid_argument("type mismatch for column: " + column.name);
        }
        if (column.type == ColumnType::STRING) {
            if (column.maxLength.has_value() &&
                static_cast<int>(value.stringValue.size()) > column.maxLength.value()) {
                throw std::invalid_argument("string too long for column: " + column.name);
            }
            return;
        }
        if (column.minValue.has_value() && value.intValue < column.minValue.value()) {
            throw std::invalid_argument("int below minimum for column: " + column.name);
        }
    }
};

}  // namespace in_memory_sql_lld

#endif
