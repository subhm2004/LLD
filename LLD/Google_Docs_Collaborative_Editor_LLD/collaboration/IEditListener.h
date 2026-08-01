#ifndef GOOGLE_DOCS_LLD_COLLABORATION_I_EDIT_LISTENER_H
#define GOOGLE_DOCS_LLD_COLLABORATION_I_EDIT_LISTENER_H

#include "../models/EditOperation.h"

namespace google_docs_lld {

class IEditListener {
public:
    virtual ~IEditListener() = default;
    virtual void onRemoteEdit(const EditOperation& operation, int newRevision) = 0;
};

}  // namespace google_docs_lld

#endif
