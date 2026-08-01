#ifndef GOOGLE_DOCS_LLD_MODELS_DOCUMENT_H
#define GOOGLE_DOCS_LLD_MODELS_DOCUMENT_H

#include <string>

namespace google_docs_lld {

struct Document {
    std::string documentId;
    std::string title;
    std::string ownerId;
    std::string content;
    int revision{0};
};

}  // namespace google_docs_lld

#endif
