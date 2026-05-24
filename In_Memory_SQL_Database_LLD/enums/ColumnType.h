#ifndef IN_MEMORY_SQL_LLD_ENUMS_COLUMN_TYPE_H
#define IN_MEMORY_SQL_LLD_ENUMS_COLUMN_TYPE_H

#include <string>

namespace in_memory_sql_lld {

enum class ColumnType { STRING, INT };

inline std::string columnTypeToString(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return "string";
        case ColumnType::INT:
            return "int";
        default:
            return "unknown";
    }
}

}  // namespace in_memory_sql_lld

#endif
