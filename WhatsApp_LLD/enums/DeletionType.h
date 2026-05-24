#ifndef DELETION_TYPE_H
#define DELETION_TYPE_H

#include <string>

namespace whatsapp_lld {

enum class DeletionType { DELETE_FOR_ME, DELETE_FOR_EVERYONE };

inline std::string deletionTypeToString(DeletionType type) {
    return type == DeletionType::DELETE_FOR_ME ? "DELETE_FOR_ME" : "DELETE_FOR_EVERYONE";
}

}  // namespace whatsapp_lld

#endif
