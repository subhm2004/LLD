#ifndef IN_MEMORY_SQL_LLD_MODELS_CELL_VALUE_H
#define IN_MEMORY_SQL_LLD_MODELS_CELL_VALUE_H

#include <string>

#include "../enums/ColumnType.h"

namespace in_memory_sql_lld {

struct CellValue {
    ColumnType type{ColumnType::STRING};
    std::string stringValue;
    int intValue{0};

    static CellValue fromString(const std::string& value) {
        return CellValue{ColumnType::STRING, value, 0};
    }

    static CellValue fromInt(int value) { return CellValue{ColumnType::INT, "", value}; }

    bool equals(const CellValue& other) const {
        if (type != other.type) {
            return false;
        }
        return type == ColumnType::STRING ? stringValue == other.stringValue
                                          : intValue == other.intValue;
    }
};

}  // namespace in_memory_sql_lld

#endif
