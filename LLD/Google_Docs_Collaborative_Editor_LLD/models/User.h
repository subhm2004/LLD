#ifndef GOOGLE_DOCS_LLD_MODELS_USER_H
#define GOOGLE_DOCS_LLD_MODELS_USER_H

#include <string>

namespace google_docs_lld {

struct User {
    std::string userId;
    std::string displayName;
    std::string email;
};

}  // namespace google_docs_lld

#endif
