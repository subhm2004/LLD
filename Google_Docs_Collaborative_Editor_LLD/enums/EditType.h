#ifndef GOOGLE_DOCS_LLD_ENUMS_EDIT_TYPE_H
#define GOOGLE_DOCS_LLD_ENUMS_EDIT_TYPE_H

#include <string>

namespace google_docs_lld {

enum class EditType { INSERT, DELETE };

inline std::string editTypeToString(EditType type) {
    switch (type) {
        case EditType::INSERT:
            return "INSERT";
        case EditType::DELETE:
            return "DELETE";
    }
    return "UNKNOWN";
}

}  // namespace google_docs_lld

#endif
