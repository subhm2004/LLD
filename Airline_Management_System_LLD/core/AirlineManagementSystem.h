#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_CORE_AIRLINEMANAGEMENTSYSTEM_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_CORE_AIRLINEMANAGEMENTSYSTEM_H

#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/BookingStatus.h"
#include "../enums/FlightStatus.h"
#include "../enums/SeatStatus.h"
#include "../enums/UserRole.h"
#include "../models/Aircraft.h"
#include "../models/Airport.h"
#include "../models/Booking.h"
#include "../models/CrewMember.h"
#include "../models/Flight.h"
#include "../models/Passenger.h"
#include "../models/Payment.h"
#include "../models/Seat.h"
#include "../models/User.h"
#include "../services/CrewAssignmentService.h"
#include "../services/FlightSearchService.h"
#include "../services/NotificationService.h"
#include "../services/PaymentService.h"
#include "../services/SeatInventoryService.h"
#include "../strategies/IPricingStrategy.h"
#include "../strategies/SeatClassPricingStrategy.h"

namespace airline_mgmt {

class AirlineManagementSystem {
public:
    AirlineManagementSystem() : pricingStrategy_(std::make_shared<SeatClassPricingStrategy>()) {}

    void login(const std::string &userId) {
        if (!users_.count(userId)) {
            throw std::runtime_error("User not found: " + userId);
        }
        activeUserId_ = userId;
        std::cout << "Logged in: " << users_.at(userId).name << " (" << roleName(users_.at(userId).role) << ")\n";
    }

    void registerUser(const User &user) { users_[user.userId] = user; }

    void registerPassenger(const Passenger &passenger) {
        if (passenger.baggageKg < 0) {
            throw std::invalid_argument("baggageKg cannot be negative");
        }
        passengers_[passenger.passengerId] = passenger;
    }

    void addAirport(const Airport &airport) { airports_[airport.code] = airport; }

    void addAircraft(const Aircraft &aircraft) { aircrafts_[aircraft.aircraftId] = aircraft; }

    void addCrewMember(const CrewMember &crew) { crewMembers_[crew.crewId] = crew; }

    void scheduleFlight(const Flight &flight) {
        requireRole(UserRole::ADMIN);
        if (!airports_.count(flight.sourceAirport) || !airports_.count(flight.destinationAirport)) {
            throw std::runtime_error("Invalid source or destination airport");
        }
        flights_[flight.flightId] = flight;
        std::cout << "Flight scheduled: " << flight.flightId << " " << flight.sourceAirport << " -> "
                  << flight.destinationAirport << " day=" << flight.departureDay << "\n";
    }

    void assignAircraftToFlight(const std::string &flightId, const std::string &aircraftId) {
        requireRole(UserRole::AIRLINE_STAFF);
        Flight &flight = getFlightOrThrow(flightId);
        if (!aircrafts_.count(aircraftId)) {
            throw std::runtime_error("Aircraft not found: " + aircraftId);
        }
        flight.aircraftId = aircraftId;
        SeatInventoryService::generateSeatsForFlight(aircrafts_.at(aircraftId), flightId, seats_);
        std::cout << "Aircraft " << aircraftId << " assigned to " << flightId << " | seats created\n";
    }

    void assignCrewToFlight(const std::string &flightId, const std::string &crewId) {
        requireRole(UserRole::AIRLINE_STAFF);
        CrewAssignmentService::assignCrew(flightId, crewId, flights_, crewMembers_, flightCrew_);
        std::cout << "Crew " << crewId << " assigned to " << flightId << "\n";
    }

    std::vector<const Flight *> searchFlights(const std::string &source, const std::string &destination,
                                                int departureDay) const {
        return FlightSearchService::search(flights_, source, destination, departureDay);
    }

    void printSearchResults(const std::string &source, const std::string &destination, int departureDay) const {
        const auto results = searchFlights(source, destination, departureDay);
        std::cout << "Search " << source << " -> " << destination << " on day " << departureDay << " : "
                  << results.size() << " flight(s)\n";
        for (const Flight *f : results) {
            std::cout << "  " << f->flightId << " | dep=" << f->departureTimeMinutes << " min | aircraft="
                      << f->aircraftId << "\n";
        }
    }

    void printAvailableSeats(const std::string &flightId) const {
        const auto available = SeatInventoryService::getAvailableSeats(flightId, seats_);
        std::cout << "Available seats on " << flightId << ": " << available.size() << "\n";
        const std::size_t preview = available.size() < 8 ? available.size() : 8;
        for (std::size_t i = 0; i < preview; ++i) {
            const Seat *seat = available[i];
            std::cout << "  " << seat->seatId << " | class=" << static_cast<int>(seat->seatClass)
                      << " | Rs " << seat->basePrice << "\n";
        }
        if (available.size() > preview) {
            std::cout << "  ... and " << (available.size() - preview) << " more\n";
        }
    }

    std::string createBooking(const std::string &passengerId, const std::string &flightId,
                              const std::vector<std::string> &seatIds) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Passenger &passenger = getPassengerOrThrow(passengerId);
        Flight &flight = getFlightOrThrow(flightId);

        if (flight.status != FlightStatus::SCHEDULED) {
            throw std::runtime_error("Flight not bookable");
        }
        if (flight.aircraftId.empty()) {
            throw std::runtime_error("Aircraft not assigned to flight");
        }
        if (!CrewAssignmentService::hasMinimumCrew(flightId, flightCrew_, crewMembers_)) {
            throw std::runtime_error("Flight crew incomplete — cannot open booking");
        }
        validateBaggage(passenger);
        if (seatIds.empty()) {
            throw std::invalid_argument("Select at least one seat");
        }

        double total = 0.0;
        for (const std::string &seatId : seatIds) {
            Seat &seat = SeatInventoryService::getSeatOrThrow(seatId, seats_);
            if (seat.flightId != flightId) {
                throw std::runtime_error("Seat does not belong to flight: " + seatId);
            }
            if (seat.status != SeatStatus::AVAILABLE) {
                throw std::runtime_error("Seat not available: " + seatId);
            }
            total += pricingStrategy_->calculateSeatFare(seat);
        }

        for (const std::string &seatId : seatIds) {
            seats_.at(seatId).status = SeatStatus::BOOKED;
        }

        const std::string bookingId = "BK_" + std::to_string(++bookingCounter_);
        Booking booking{bookingId, passengerId, flightId, seatIds, BookingStatus::PENDING, total};
        bookings_[bookingId] = booking;

        std::cout << "Booking created (PENDING payment): " << bookingId << " | amount Rs " << total << "\n";
        return bookingId;
    }

    void payForBooking(const std::string &bookingId) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Booking &booking = getBookingOrThrow(bookingId);
        if (booking.status != BookingStatus::PENDING) {
            throw std::runtime_error("Booking is not pending payment");
        }

        PaymentService::processPayment(bookingId, booking.totalAmount, payments_, paymentCounter_);
        booking.status = BookingStatus::CONFIRMED;
        notificationService_.sendBookingConfirmed(passengers_.at(booking.passengerId).email, bookingId,
                                                  booking.totalAmount);
        std::cout << "Payment success — booking confirmed: " << bookingId << "\n";
    }

    void cancelBooking(const std::string &bookingId) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Booking &booking = getBookingOrThrow(bookingId);
        if (booking.status == BookingStatus::CANCELLED || booking.status == BookingStatus::REFUNDED) {
            throw std::runtime_error("Booking already cancelled");
        }

        releaseSeats(booking);
        if (booking.status == BookingStatus::CONFIRMED) {
            PaymentService::refundPayment(bookingId, payments_);
            booking.status = BookingStatus::REFUNDED;
            notificationService_.sendRefund(passengers_.at(booking.passengerId).email, bookingId, booking.totalAmount);
        } else {
            booking.status = BookingStatus::CANCELLED;
        }

        std::cout << "Booking cancelled: " << bookingId << "\n";
    }

    std::string changeFlight(const std::string &bookingId, const std::string &newFlightId,
                             const std::vector<std::string> &newSeatIds) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Booking &oldBooking = getBookingOrThrow(bookingId);
        if (oldBooking.status != BookingStatus::CONFIRMED) {
            throw std::runtime_error("Only confirmed bookings can be changed");
        }

        const std::string passengerId = oldBooking.passengerId;
        const double oldAmount = oldBooking.totalAmount;
        releaseSeats(oldBooking);
        oldBooking.status = BookingStatus::CANCELLED;

        const std::string newBookingId = createBookingUnlocked(passengerId, newFlightId, newSeatIds);
        Booking &newBooking = getBookingOrThrow(newBookingId);
        newBooking.status = BookingStatus::CONFIRMED;
        PaymentService::processPayment(newBookingId, newBooking.totalAmount, payments_, paymentCounter_);

        notificationService_.sendFlightChange(passengers_.at(passengerId).email, newFlightId);
        std::cout << "Flight changed: " << bookingId << " -> " << newBookingId << " | old Rs " << oldAmount
                  << " new Rs " << newBooking.totalAmount << "\n";
        return newBookingId;
    }

    void delayFlight(const std::string &flightId, int newDepartureTimeMinutes) {
        requireRole(UserRole::AIRLINE_STAFF);
        Flight &flight = getFlightOrThrow(flightId);
        flight.status = FlightStatus::DELAYED;
        flight.departureTimeMinutes = newDepartureTimeMinutes;
        std::cout << "Flight delayed: " << flightId << " new dep time=" << newDepartureTimeMinutes << "\n";
    }

private:
    std::unordered_map<std::string, Airport> airports_;
    std::unordered_map<std::string, Aircraft> aircrafts_;
    std::unordered_map<std::string, Flight> flights_;
    std::unordered_map<std::string, Seat> seats_;
    std::unordered_map<std::string, Passenger> passengers_;
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, CrewMember> crewMembers_;
    std::unordered_map<std::string, std::vector<std::string>> flightCrew_;
    std::unordered_map<std::string, Booking> bookings_;
    std::unordered_map<std::string, Payment> payments_;

    std::shared_ptr<IPricingStrategy> pricingStrategy_;
    NotificationService notificationService_;

    mutable std::mutex bookingMutex_;
    int bookingCounter_ = 0;
    int paymentCounter_ = 0;
    std::string activeUserId_;

    std::string createBookingUnlocked(const std::string &passengerId, const std::string &flightId,
                                      const std::vector<std::string> &seatIds) {
        getPassengerOrThrow(passengerId);
        getFlightOrThrow(flightId);
        validateBaggage(getPassengerOrThrow(passengerId));

        double total = 0.0;
        for (const std::string &seatId : seatIds) {
            Seat &seat = SeatInventoryService::getSeatOrThrow(seatId, seats_);
            if (seat.flightId != flightId || seat.status != SeatStatus::AVAILABLE) {
                throw std::runtime_error("Seat unavailable for change: " + seatId);
            }
            total += pricingStrategy_->calculateSeatFare(seat);
        }
        for (const std::string &seatId : seatIds) {
            seats_.at(seatId).status = SeatStatus::BOOKED;
        }
        const std::string bookingId = "BK_" + std::to_string(++bookingCounter_);
        bookings_[bookingId] = Booking{bookingId, passengerId, flightId, seatIds, BookingStatus::PENDING, total};
        return bookingId;
    }

    void releaseSeats(Booking &booking) {
        for (const std::string &seatId : booking.seatIds) {
            if (seats_.count(seatId)) {
                seats_.at(seatId).status = SeatStatus::AVAILABLE;
            }
        }
    }

    static void validateBaggage(const Passenger &passenger) {
        if (passenger.baggageKg > 30.0) {
            throw std::runtime_error("Baggage exceeds airline limit (30 kg)");
        }
    }

    void requireRole(UserRole minimumRole) const {
        if (activeUserId_.empty() || !users_.count(activeUserId_)) {
            throw std::runtime_error("Login required");
        }
        const UserRole role = users_.at(activeUserId_).role;
        if (static_cast<int>(role) < static_cast<int>(minimumRole)) {
            throw std::runtime_error("Insufficient permissions");
        }
    }

    Flight &getFlightOrThrow(const std::string &flightId) {
        auto it = flights_.find(flightId);
        if (it == flights_.end()) {
            throw std::runtime_error("Flight not found: " + flightId);
        }
        return it->second;
    }

    Passenger &getPassengerOrThrow(const std::string &passengerId) {
        auto it = passengers_.find(passengerId);
        if (it == passengers_.end()) {
            throw std::runtime_error("Passenger not found: " + passengerId);
        }
        return it->second;
    }

    Booking &getBookingOrThrow(const std::string &bookingId) {
        auto it = bookings_.find(bookingId);
        if (it == bookings_.end()) {
            throw std::runtime_error("Booking not found: " + bookingId);
        }
        return it->second;
    }

    static std::string roleName(UserRole role) {
        switch (role) {
        case UserRole::PASSENGER:
            return "PASSENGER";
        case UserRole::AIRLINE_STAFF:
            return "AIRLINE_STAFF";
        case UserRole::ADMIN:
            return "ADMIN";
        }
        return "UNKNOWN";
    }
};

} // namespace airline_mgmt

#endif
