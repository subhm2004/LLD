#ifndef MOVIE_TICKET_BOOKING_SYSTEM_EXTERNAL_PAYMENTGATEWAY_H
#define MOVIE_TICKET_BOOKING_SYSTEM_EXTERNAL_PAYMENTGATEWAY_H

namespace movie_ticket_booking {

class PaymentGateway {
public:
    bool processPayment(double amount) const {
        (void)amount;
        return true;
    }
};

}

#endif
