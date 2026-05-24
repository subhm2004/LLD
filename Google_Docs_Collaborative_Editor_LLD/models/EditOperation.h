#ifndef GOOGLE_DOCS_LLD_MODELS_EDIT_OPERATION_H
#define GOOGLE_DOCS_LLD_MODELS_EDIT_OPERATION_H

#include <string>

#include "../enums/EditType.h"

namespace google_docs_lld {

struct EditOperation {
    std::string operationId;
    std::string documentId;
    std::string userId;
    EditType type{EditType::INSERT};
    int position{0};
    std::string text;   // INSERT payload
    int length{0};      // DELETE span length
    int baseRevision{0};
};

}  // namespace google_docs_lld

#endif
