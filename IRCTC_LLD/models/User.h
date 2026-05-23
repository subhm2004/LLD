#ifndef IRCTC_LLD_MODELS_USER_H
#define IRCTC_LLD_MODELS_USER_H

#include <string>

namespace irctc_lld {

struct User {
    std::string userId;
    std::string name;
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_USER_H
