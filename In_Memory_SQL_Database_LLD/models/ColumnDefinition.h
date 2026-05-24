#ifndef IN_MEMORY_SQL_LLD_MODELS_COLUMN_DEFINITION_H
#define IN_MEMORY_SQL_LLD_MODELS_COLUMN_DEFINITION_H

#include <optional>
#include <string>

#include "../enums/ColumnType.h"

namespace in_memory_sql_lld {

struct ColumnDefinition {
    std::string name;
    ColumnType type{ColumnType::STRING};
    bool required{false};
    std::optional<int> maxLength;   // string only
    std::optional<int> minValue;    // int only
};

}  // namespace in_memory_sql_lld

#endif
