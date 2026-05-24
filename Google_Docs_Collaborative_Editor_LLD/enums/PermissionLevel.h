#ifndef GOOGLE_DOCS_LLD_ENUMS_PERMISSION_LEVEL_H
#define GOOGLE_DOCS_LLD_ENUMS_PERMISSION_LEVEL_H

#include <string>

namespace google_docs_lld {

enum class PermissionLevel { VIEW, COMMENT, EDIT };

inline std::string permissionLevelToString(PermissionLevel level) {
    switch (level) {
        case PermissionLevel::VIEW:
            return "VIEW";
        case PermissionLevel::COMMENT:
            return "COMMENT";
        case PermissionLevel::EDIT:
            return "EDIT";
    }
    return "UNKNOWN";
}

inline bool canEdit(PermissionLevel level) { return level == PermissionLevel::EDIT; }

inline bool canComment(PermissionLevel level) {
    return level == PermissionLevel::EDIT || level == PermissionLevel::COMMENT;
}

}  // namespace google_docs_lld

#endif
