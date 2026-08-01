#ifndef STOCK_EXCHANGE_LLD_UTILS_PRICEUTILS_H
#define STOCK_EXCHANGE_LLD_UTILS_PRICEUTILS_H

#include <cmath>

namespace stock_exchange_lld {

inline bool pricesEqual(double a, double b, double epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}

}  // namespace stock_exchange_lld

#endif  // STOCK_EXCHANGE_LLD_UTILS_PRICEUTILS_H
