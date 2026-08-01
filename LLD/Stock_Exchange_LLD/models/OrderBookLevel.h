#ifndef STOCK_EXCHANGE_LLD_MODELS_ORDERBOOKLEVEL_H
#define STOCK_EXCHANGE_LLD_MODELS_ORDERBOOKLEVEL_H

namespace stock_exchange_lld {

struct OrderBookLevel {
    double price{0.0};
    int totalQuantity{0};
    int orderCount{0};
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_MODELS_ORDERBOOKLEVEL_H
