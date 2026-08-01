#ifndef MOVIE_TICKET_SYSTEM_H
#define MOVIE_TICKET_SYSTEM_H

#include "../external/NotificationService.h"
#include "../external/PaymentGateway.h"
#include "../managers/BookingManager.h"
#include "../managers/CatalogManager.h"
#include "../managers/ShowManager.h"
#include "../strategies/StandardPricingStrategy.h"

namespace movie_ticket_booking {

class MovieTicketSystem {
public:
    MovieTicketSystem()
        : bookingManager_(&pricingStrategy_, &paymentGateway_, &notificationService_) {}

    CatalogManager& catalogManager() { return catalogManager_; }
    ShowManager& showManager() { return showManager_; }
    BookingManager& bookingManager() { return bookingManager_; }

private:
    StandardPricingStrategy pricingStrategy_;
    PaymentGateway paymentGateway_;
    NotificationService notificationService_;
    CatalogManager catalogManager_;
    ShowManager showManager_;
    BookingManager bookingManager_;
};

}
#endif // MOVIE_TICKET_SYSTEM_H