#ifndef STOCK_EXCHANGE_LLD_MODELS_SYMBOL_H
#define STOCK_EXCHANGE_LLD_MODELS_SYMBOL_H

#include <string>

namespace stock_exchange_lld {

struct Symbol {
    std::string ticker;
    std::string name;
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_MODELS_SYMBOL_H
