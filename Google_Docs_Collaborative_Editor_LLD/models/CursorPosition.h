#ifndef GOOGLE_DOCS_LLD_MODELS_CURSOR_POSITION_H
#define GOOGLE_DOCS_LLD_MODELS_CURSOR_POSITION_H

#include <string>

namespace google_docs_lld {

struct CursorPosition {
    std::string userId;
    std::string displayName;
    int index{0};
};

}  // namespace google_docs_lld

#endif
