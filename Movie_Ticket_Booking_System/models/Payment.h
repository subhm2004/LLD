#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_PAYMENT_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_PAYMENT_H

#include <string>
#include "../enums/PaymentStatus.h"

namespace movie_ticket_booking {

class Payment {
public:
    Payment(std::string id, std::string bookingId, double amount)
        : paymentId_(id), bookingId_(bookingId), amount_(amount), status_(PaymentStatus::INITIATED) {}

    PaymentStatus getStatus() const { return status_; }
    void markSuccess() { status_ = PaymentStatus::SUCCESS; }
    void markFailed() { status_ = PaymentStatus::FAILED; }

private:
    std::string paymentId_;
    std::string bookingId_;
    double amount_;
    PaymentStatus status_;
};

}

#endif
