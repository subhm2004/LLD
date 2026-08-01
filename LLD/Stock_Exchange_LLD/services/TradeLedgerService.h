#ifndef STOCK_EXCHANGE_LLD_SERVICES_TRADELEDGERSERVICE_H
#define STOCK_EXCHANGE_LLD_SERVICES_TRADELEDGERSERVICE_H

#include <string>
#include <vector>

#include "../models/Trade.h"

namespace stock_exchange_lld {

class TradeLedgerService {
public:
    void record(const Trade& trade) { trades_.push_back(trade); }

    const std::vector<Trade>& listAll() const { return trades_; }

    std::vector<Trade> listBySymbol(const std::string& symbol) const {
        std::vector<Trade> result;
        for (const Trade& trade : trades_) {
            if (trade.symbol == symbol) {
                result.push_back(trade);
            }
        }
        return result;
    }

private:
    std::vector<Trade> trades_;
};

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_SERVICES_TRADELEDGERSERVICE_H
