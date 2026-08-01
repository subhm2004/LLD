#ifndef STOCK_EXCHANGE_LLD_MODELS_USER_H
#define STOCK_EXCHANGE_LLD_MODELS_USER_H

#include <string>

namespace stock_exchange_lld {

struct User {
    std::string userId;
    std::string name;
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_MODELS_USER_H
