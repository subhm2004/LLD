#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PAYMENT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PAYMENT_H

#include <string>

#include "../enums/PaymentStatus.h"

namespace airline_mgmt {

struct Payment {
    std::string paymentId;
    std::string bookingId;
    double amount;
    PaymentStatus status;
};

} // namespace airline_mgmt

#endif
