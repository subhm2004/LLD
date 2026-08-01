#ifndef STOCK_EXCHANGE_LLD_MODELS_TRADE_H
#define STOCK_EXCHANGE_LLD_MODELS_TRADE_H

#include <string>

namespace stock_exchange_lld {

struct Trade {
    std::string tradeId;
    std::string symbol;
    std::string buyOrderId;
    std::string sellOrderId;
    double price{0.0};
    int quantity{0};
    long long executedAtEpochMs{0};
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_MODELS_TRADE_H
