#ifndef GOOGLE_DOCS_LLD_MODELS_COLLABORATOR_ACCESS_H
#define GOOGLE_DOCS_LLD_MODELS_COLLABORATOR_ACCESS_H

#include <string>

#include "../enums/PermissionLevel.h"

namespace google_docs_lld {

struct CollaboratorAccess {
    std::string documentId;
    std::string userId;
    PermissionLevel permission{PermissionLevel::VIEW};
};

}  // namespace google_docs_lld

#endif
