#ifndef IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H
#define IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H

#include <string>

#include "../enums/BookingStatus.h"
#include "../models/TicketBooking.h"
#include "../models/Train.h"

namespace irctc_lld {

class BookingFactory {
public:
    static TicketBooking createConfirmed(int& bookingCounter, const std::string& userId,
                                         const Train& train, const std::string& seatId,
                                         const std::string& source, const std::string& destination,
                                         int fromIdx, int toIdx) {
        TicketBooking booking;
        booking.bookingId = "PNR" + std::to_string(++bookingCounter);
        booking.userId = userId;
        booking.trainNumber = train.trainNumber;
        booking.runDate = train.runDate;
        booking.seatId = seatId;
        booking.sourceStation = source;
        booking.destinationStation = destination;
        booking.fromStationIdx = fromIdx;
        booking.toStationIdx = toIdx;
        booking.status = BookingStatus::CONFIRMED;
        return booking;
    }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H
