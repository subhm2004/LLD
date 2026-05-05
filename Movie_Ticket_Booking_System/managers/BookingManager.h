#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_BOOKINGMANAGER_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_BOOKINGMANAGER_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "../external/PaymentGateway.h"
#include "../external/NotificationService.h"
#include "../factories/BookingFactory.h"
#include "../models/Booking.h"
#include "../models/Show.h"
#include "../models/User.h"
#include "../strategies/PricingStrategy.h"

namespace movie_ticket_booking {

class BookingManager {
public:
    BookingManager(const PricingStrategy* pricingStrategy,
                   const PaymentGateway* paymentGateway,
                   const NotificationService* notificationService)
        : pricingStrategy_(pricingStrategy),
          paymentGateway_(paymentGateway),
          notificationService_(notificationService),
          bookingCounter_(0) {}

    ~BookingManager() {
        for (auto& kv : bookings_) delete kv.second;
    }

    Booking* createBooking(User& user, Show& show, const std::vector<std::string>& seatIds) {
        double totalAmount = reserveSeatsAndGetAmount(show, seatIds);
        std::string bookingId = "BOOK_" + std::to_string(++bookingCounter_);
        Booking* booking = bookingFactory_.createBooking(bookingId, user.getId(), show.getId(), seatIds, totalAmount);

        if (!paymentGateway_->processPayment(totalAmount)) {
            booking->setStatus(BookingStatus::CANCELLED);
            throw std::runtime_error("Payment failed");
        }

        booking->setStatus(BookingStatus::CONFIRMED);
        bookings_[bookingId] = booking;
        notificationService_->sendBookingConfirmation(user.getEmail(), bookingId);
        return booking;
    }

private:
    const PricingStrategy* pricingStrategy_;
    const PaymentGateway* paymentGateway_;
    const NotificationService* notificationService_;
    BookingFactory bookingFactory_;
    std::unordered_map<std::string, Booking*> bookings_;
    int bookingCounter_;

    double reserveSeatsAndGetAmount(Show& show, const std::vector<std::string>& seatIds) {
        double amount = 0.0;
        std::vector<Seat>& seats = show.getSeats();

        for (const std::string& seatId : seatIds) {
            bool found = false;
            for (Seat& seat : seats) {
                if (seat.getSeatId() == seatId) {
                    found = true;
                    if (seat.isBooked()) throw std::runtime_error("Seat already booked: " + seatId);
                    seat.markBooked();
                    amount += pricingStrategy_->getSeatPrice(seat.getSeatType());
                    break;
                }
            }
            if (!found) throw std::runtime_error("Invalid seat: " + seatId);
        }
        return amount;
    }
};

}

#endif
