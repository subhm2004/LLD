// ============================================================================
//  core/AirlineManagementSystem.h  —  Poore system ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client (main.cpp) ke liye "airline system" hai. Andar 5 services,
//  1 strategy, aur ~10 models ka poora tamasha chalta hai — par bahar se client
//  ko sirf saaf-suthre methods dikhte hain: scheduleFlight, searchFlights,
//  createBooking, payForBooking, cancelBooking, changeFlight...
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — subsystem ki complexity client se chhupao           │
//  │                                                                          │
//  │  Client ko ye NAHI pata:                                                 │
//  │    - seats kaise generate hoti hain     (SeatInventoryService)           │
//  │    - fare kaise nikalta hai              (PricingStrategy)               │
//  │    - crew minimum check kaise hota hai   (CrewAssignmentService)         │
//  │    - payment/refund/notify kaise hota    (Payment/NotificationService)   │
//  │  Wo bas facade ke method bulata hai; facade sahi service ko sahi data ke  │
//  │  saath delegate kar deta hai. Facade = orchestrator (conductor).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE khud SAARE DATA ka MAALIK hai (airports, flights, seats, bookings,
//     payments...). Services zyadatar STATELESS hain (static helpers) aur facade
//     unhe ye data reference se pass karta hai. Isse "single source of truth"
//     ek jagah rehta hai (yahan), aur services ko akele test karna aasan.
//
//  ⭐ IS FILE KE 3 SABSE ZAROORI IDEAS (interview me pooche jaate hain):
//     1. mutex se seat double-booking rukti hai (concurrency — R8)
//     2. "validate-all-then-commit" — seats pehle check, phir book (atomic)
//     3. createBookingUnlocked — re-entrant DEADLOCK se bachne ke liye
//     (teeno neeche apni jagah detail me samjhaaye hain.)
// ============================================================================
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
    // Constructor: default pricing strategy inject karo (Strategy Pattern).
    // Kal ko dusri strategy chahiye to yahi ek line badle — baaki system waisa hi.
    AirlineManagementSystem() : pricingStrategy_(std::make_shared<SeatClassPricingStrategy>()) {}

    // ========================================================================
    //  AUTH — login + registration
    // ========================================================================

    // Login = "active user" set karna. Iske baad requireRole() isi user ka role
    // dekhta hai. User exist na kare to error.
    void login(const std::string &userId) {
        if (!users_.count(userId)) {
            throw std::runtime_error("User not found: " + userId);
        }
        activeUserId_ = userId;
        std::cout << "Logged in: " << users_.at(userId).name << " (" << roleName(users_.at(userId).role) << ")\n";
    }

    void registerUser(const User &user) { users_[user.userId] = user; }

    // Passenger register — baggage negative nahi ho sakta (basic validation).
    void registerPassenger(const Passenger &passenger) {
        if (passenger.baggageKg < 0) {
            throw std::invalid_argument("baggageKg cannot be negative");
        }
        passengers_[passenger.passengerId] = passenger;
    }

    // ---- Infrastructure add (airports, aircrafts, crew) --------------------
    void addAirport(const Airport &airport) { airports_[airport.code] = airport; }
    void addAircraft(const Aircraft &aircraft) { aircrafts_[aircraft.aircraftId] = aircraft; }
    void addCrewMember(const CrewMember &crew) { crewMembers_[crew.crewId] = crew; }

    // ========================================================================
    //  ADMIN — flight schedule karo (Requirement #3)
    // ========================================================================
    void scheduleFlight(const Flight &flight) {
        requireRole(UserRole::ADMIN); // sirf ADMIN — RBAC gate
        // Source aur destination dono valid airports hone chahiye.
        if (!airports_.count(flight.sourceAirport) || !airports_.count(flight.destinationAirport)) {
            throw std::runtime_error("Invalid source or destination airport");
        }
        flights_[flight.flightId] = flight;
        std::cout << "Flight scheduled: " << flight.flightId << " " << flight.sourceAirport << " -> "
                  << flight.destinationAirport << " day=" << flight.departureDay << "\n";
    }

    // ========================================================================
    //  STAFF — aircraft assign (seats generate) (Requirement #3)
    // ========================================================================
    void assignAircraftToFlight(const std::string &flightId, const std::string &aircraftId) {
        requireRole(UserRole::AIRLINE_STAFF); // STAFF ya usse upar (ADMIN bhi)
        Flight &flight = getFlightOrThrow(flightId);
        if (!aircrafts_.count(aircraftId)) {
            throw std::runtime_error("Aircraft not found: " + aircraftId);
        }
        flight.aircraftId = aircraftId;
        // ⭐ Aircraft lagte hi us flight ki asli seats ban jaati hain (capacity ke hisaab se)
        SeatInventoryService::generateSeatsForFlight(aircrafts_.at(aircraftId), flightId, seats_);
        std::cout << "Aircraft " << aircraftId << " assigned to " << flightId << " | seats created\n";
    }

    // ========================================================================
    //  STAFF — crew assign (Requirement #3)
    // ========================================================================
    void assignCrewToFlight(const std::string &flightId, const std::string &crewId) {
        requireRole(UserRole::AIRLINE_STAFF);
        CrewAssignmentService::assignCrew(flightId, crewId, flights_, crewMembers_, flightCrew_);
        std::cout << "Crew " << crewId << " assigned to " << flightId << "\n";
    }

    // ========================================================================
    //  SEARCH — flights dhoondho (Requirement #1)  [koi bhi kar sakta hai]
    // ========================================================================
    std::vector<const Flight *> searchFlights(const std::string &source, const std::string &destination,
                                                int departureDay) const {
        return FlightSearchService::search(flights_, source, destination, departureDay);
    }

    // Search result ko console pe saaf print karne wala helper.
    void printSearchResults(const std::string &source, const std::string &destination, int departureDay) const {
        const auto results = searchFlights(source, destination, departureDay);
        std::cout << "Search " << source << " -> " << destination << " on day " << departureDay << " : "
                  << results.size() << " flight(s)\n";
        for (const Flight *f : results) {
            std::cout << "  " << f->flightId << " | dep=" << f->departureTimeMinutes << " min | aircraft="
                      << f->aircraftId << "\n";
        }
    }

    // Kisi flight ki available seats print karo (pehli 8 ka preview).
    void printAvailableSeats(const std::string &flightId) const {
        const auto available = SeatInventoryService::getAvailableSeats(flightId, seats_);
        std::cout << "Available seats on " << flightId << ": " << available.size() << "\n";
        // Bahut saari seats ho to sirf pehli 8 dikhao (console flood na ho).
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

    // ========================================================================
    //  ⭐⭐ BOOKING — seats lock karo (Requirement #2, #8)
    //  Ye is poori file ka sabse important + subtle function hai.
    // ========================================================================
    std::string createBooking(const std::string &passengerId, const std::string &flightId,
                              const std::vector<std::string> &seatIds) {
        // ⭐ CRITICAL SECTION START — mutex lock.
        //
        // Do log ek saath same seat book karne aayein to ye lock unhe "ek-ek
        // karke" chalata hai (serialize). Pehla poora kaam khatam kare tabhi
        // doosra andar aaye. Isi se SEAT DOUBLE-BOOKING rukti hai (R8).
        // `lock_guard` RAII hai — function se bahar niklo (return ya throw),
        // lock apne aap chhoot jaata hai (unlock bhoolne ka dar nahi).
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Passenger &passenger = getPassengerOrThrow(passengerId);
        Flight &flight = getFlightOrThrow(flightId);

        // ---- Booking "gates" — sab paar karo tabhi booking khulegi -----------
        if (flight.status != FlightStatus::SCHEDULED) {
            throw std::runtime_error("Flight not bookable"); // DELAYED/CANCELLED/DEPARTED pe nahi
        }
        if (flight.aircraftId.empty()) {
            throw std::runtime_error("Aircraft not assigned to flight"); // seats hi nahi bani
        }
        if (!CrewAssignmentService::hasMinimumCrew(flightId, flightCrew_, crewMembers_)) {
            throw std::runtime_error("Flight crew incomplete — cannot open booking"); // cockpit+cabin chahiye
        }
        validateBaggage(passenger); // 30 kg se zyada to yahin ruk jaao
        if (seatIds.empty()) {
            throw std::invalid_argument("Select at least one seat");
        }

        // ⭐ TWO-PHASE (validate-all-then-commit) — ye design zaroori hai:
        //
        //  PHASE 1: saari seats CHECK karo (available hain? isi flight ki hain?)
        //           aur fare jodo. Yahan tak kuch BADLA nahi.
        //  PHASE 2: sab theek nikla to hi seats BOOK karo.
        //
        //  Kyun? Agar ek-ek karke book karte aur beech me 3rd seat unavailable
        //  nikalti, to pehli 2 seats already BOOKED ho chuki hotin — aadhi-adhoori
        //  booking (inconsistent state)! Pehle poora check, phir poora commit =
        //  "ya saari seats, ya ek bhi nahi" (atomicity). Aur ye poora mutex ke
        //  andar hai to beech me koi aur ghus bhi nahi sakta.
        double total = 0.0;
        for (const std::string &seatId : seatIds) {          // ---- PHASE 1: validate + price ----
            Seat &seat = SeatInventoryService::getSeatOrThrow(seatId, seats_);
            if (seat.flightId != flightId) {
                throw std::runtime_error("Seat does not belong to flight: " + seatId);
            }
            if (seat.status != SeatStatus::AVAILABLE) {
                throw std::runtime_error("Seat not available: " + seatId);
            }
            total += pricingStrategy_->calculateSeatFare(seat); // Strategy se fare
        }
        for (const std::string &seatId : seatIds) {          // ---- PHASE 2: commit (book) ----
            seats_.at(seatId).status = SeatStatus::BOOKED;
        }

        // Booking record banao — status PENDING (paisa abhi baaki).
        const std::string bookingId = "BK_" + std::to_string(++bookingCounter_);
        Booking booking{bookingId, passengerId, flightId, seatIds, BookingStatus::PENDING, total};
        bookings_[bookingId] = booking;

        std::cout << "Booking created (PENDING payment): " << bookingId << " | amount Rs " << total << "\n";
        return bookingId;
        // ⭐ CRITICAL SECTION END — lock_guard scope khatam, mutex auto-release.
    }

    // ========================================================================
    //  PAYMENT — booking confirm karo (Requirement #2)
    // ========================================================================
    void payForBooking(const std::string &bookingId) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Booking &booking = getBookingOrThrow(bookingId);
        if (booking.status != BookingStatus::PENDING) {
            throw std::runtime_error("Booking is not pending payment"); // already paid/cancelled
        }

        // Payment process -> booking CONFIRMED -> passenger ko notify.
        PaymentService::processPayment(bookingId, booking.totalAmount, payments_, paymentCounter_);
        booking.status = BookingStatus::CONFIRMED;
        notificationService_.sendBookingConfirmed(passengers_.at(booking.passengerId).email, bookingId,
                                                  booking.totalAmount);
        std::cout << "Payment success — booking confirmed: " << bookingId << "\n";
    }

    // ========================================================================
    //  CANCEL — seats chhodo + (confirmed the to) refund (Requirement #6)
    // ========================================================================
    void cancelBooking(const std::string &bookingId) {
        std::lock_guard<std::mutex> lock(bookingMutex_);

        Booking &booking = getBookingOrThrow(bookingId);
        if (booking.status == BookingStatus::CANCELLED || booking.status == BookingStatus::REFUNDED) {
            throw std::runtime_error("Booking already cancelled"); // dobara cancel na ho
        }

        releaseSeats(booking); // seats wapas AVAILABLE

        // ⭐ Faisla: paisa gaya tha ya nahi?
        //    CONFIRMED tha (paid) -> refund karo -> REFUNDED
        //    PENDING tha (unpaid) -> seedha CANCELLED (refund ka sawaal hi nahi)
        if (booking.status == BookingStatus::CONFIRMED) {
            PaymentService::refundPayment(bookingId, payments_);
            booking.status = BookingStatus::REFUNDED;
            notificationService_.sendRefund(passengers_.at(booking.passengerId).email, bookingId, booking.totalAmount);
        } else {
            booking.status = BookingStatus::CANCELLED;
        }

        std::cout << "Booking cancelled: " << bookingId << "\n";
    }

    // ========================================================================
    //  CHANGE FLIGHT — confirmed booking ko dusri flight/seats pe le jao (R7)
    // ========================================================================
    std::string changeFlight(const std::string &bookingId, const std::string &newFlightId,
                             const std::vector<std::string> &newSeatIds) {
        std::lock_guard<std::mutex> lock(bookingMutex_); // ⭐ lock LE liya

        Booking &oldBooking = getBookingOrThrow(bookingId);
        if (oldBooking.status != BookingStatus::CONFIRMED) {
            throw std::runtime_error("Only confirmed bookings can be changed");
        }

        const std::string passengerId = oldBooking.passengerId;
        const double oldAmount = oldBooking.totalAmount;
        releaseSeats(oldBooking);                       // purani seats chhodo
        oldBooking.status = BookingStatus::CANCELLED;   // purani booking cancel

        // ⭐⭐ YAHAN DEADLOCK KA KHATRA THA — isi liye "Unlocked" variant hai:
        //
        //  Hum abhi bookingMutex_ pakde hue hain (upar lock_guard). Agar yahan
        //  seedha createBooking() bulate, to WO bhi usi mutex ko lock karne ki
        //  koshish karta. Par std::mutex RE-ENTRANT NAHI hai — ek hi thread do
        //  baar lock nahi le sakta -> DEADLOCK (thread khud ka intezaar karta
        //  reh jaata, hamesha). 💀
        //
        //  Hal: createBookingUnlocked() — wahi booking logic, PAR bina lock ke.
        //  Kyunki hum pehle se lock ke andar hain, wo safe hai.
        //  📌 Ye "public locks, private unlocked-core" ek aam concurrency pattern hai.
        const std::string newBookingId = createBookingUnlocked(passengerId, newFlightId, newSeatIds);
        Booking &newBooking = getBookingOrThrow(newBookingId);
        newBooking.status = BookingStatus::CONFIRMED; // change me naya booking turant confirmed
        PaymentService::processPayment(newBookingId, newBooking.totalAmount, payments_, paymentCounter_);

        notificationService_.sendFlightChange(passengers_.at(passengerId).email, newFlightId);
        std::cout << "Flight changed: " << bookingId << " -> " << newBookingId << " | old Rs " << oldAmount
                  << " new Rs " << newBooking.totalAmount << "\n";
        return newBookingId;
    }

    // ========================================================================
    //  STAFF — flight delay karo (Requirement #3)
    // ========================================================================
    void delayFlight(const std::string &flightId, int newDepartureTimeMinutes) {
        requireRole(UserRole::AIRLINE_STAFF);
        Flight &flight = getFlightOrThrow(flightId);
        flight.status = FlightStatus::DELAYED;
        flight.departureTimeMinutes = newDepartureTimeMinutes;
        std::cout << "Flight delayed: " << flightId << " new dep time=" << newDepartureTimeMinutes << "\n";
    }

private:
    // ========================================================================
    //  DATA — facade single source of truth (sab in-memory maps)
    // ========================================================================
    std::unordered_map<std::string, Airport> airports_;
    std::unordered_map<std::string, Aircraft> aircrafts_;
    std::unordered_map<std::string, Flight> flights_;
    std::unordered_map<std::string, Seat> seats_;
    std::unordered_map<std::string, Passenger> passengers_;
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, CrewMember> crewMembers_;
    std::unordered_map<std::string, std::vector<std::string>> flightCrew_; // flightId -> [crewId...]
    std::unordered_map<std::string, Booking> bookings_;
    std::unordered_map<std::string, Payment> payments_;

    std::shared_ptr<IPricingStrategy> pricingStrategy_; // Strategy (injected)
    NotificationService notificationService_;           // notify helper

    // ⭐ `mutable` — mutex ko const method (searchFlights) me bhi lock kar sakein.
    // Lock lena object ki "logical state" nahi badalta, isi liye mutable sahi hai.
    mutable std::mutex bookingMutex_;
    int bookingCounter_ = 0;  // unique booking id
    int paymentCounter_ = 0;  // unique payment id
    std::string activeUserId_; // abhi kaun logged in hai (RBAC ke liye)

    // ---- Booking ka LOCK-FREE core (sirf lock ke andar se bulao!) ----------
    //
    // Ye createBooking() jaisa hi hai PAR mutex lock NAHI leta. Sirf changeFlight()
    // ise bulata hai (jo pehle se lock pakde hue hai) — deadlock se bachne ke liye.
    // ⚠ Ise kabhi seedha public jagah se mat bulana (bina lock ke race condition).
    //
    // 📌 SIMPLIFICATION NOTE: ye version flight-status/crew-minimum dobara check
    //    nahi karta (createBooking karta hai). Change ke waqt naya flight already
    //    ready maana jaata hai. Real system me yahan bhi wahi gates lagane chahiye.
    std::string createBookingUnlocked(const std::string &passengerId, const std::string &flightId,
                                      const std::vector<std::string> &seatIds) {
        getPassengerOrThrow(passengerId);
        getFlightOrThrow(flightId);
        validateBaggage(getPassengerOrThrow(passengerId));

        double total = 0.0;
        for (const std::string &seatId : seatIds) { // validate + price (phase 1)
            Seat &seat = SeatInventoryService::getSeatOrThrow(seatId, seats_);
            if (seat.flightId != flightId || seat.status != SeatStatus::AVAILABLE) {
                throw std::runtime_error("Seat unavailable for change: " + seatId);
            }
            total += pricingStrategy_->calculateSeatFare(seat);
        }
        for (const std::string &seatId : seatIds) { // commit (phase 2)
            seats_.at(seatId).status = SeatStatus::BOOKED;
        }
        const std::string bookingId = "BK_" + std::to_string(++bookingCounter_);
        bookings_[bookingId] = Booking{bookingId, passengerId, flightId, seatIds, BookingStatus::PENDING, total};
        return bookingId;
    }

    // Booking ki saari seats wapas AVAILABLE karo (cancel/change ke waqt).
    void releaseSeats(Booking &booking) {
        for (const std::string &seatId : booking.seatIds) {
            if (seats_.count(seatId)) {
                seats_.at(seatId).status = SeatStatus::AVAILABLE;
            }
        }
    }

    // Baggage limit rule — 30 kg se zyada to booking nahi (business rule).
    static void validateBaggage(const Passenger &passenger) {
        if (passenger.baggageKg > 30.0) {
            throw std::runtime_error("Baggage exceeds airline limit (30 kg)");
        }
    }

    // ========================================================================
    //  ⭐ RBAC — role-based access control ka dil
    // ========================================================================
    //  requireRole(X) = "active user ka role kam se kam X hona chahiye".
    //  UserRole ordinal hai (PASSENGER=0 < STAFF=1 < ADMIN=2), to number compare
    //  se hierarchy apne aap lag jaati hai: ADMIN (2) staff (1) ke kaam bhi kar
    //  sakta hai, par passenger (0) staff ke nahi. (UserRole.h me note padho.)
    void requireRole(UserRole minimumRole) const {
        if (activeUserId_.empty() || !users_.count(activeUserId_)) {
            throw std::runtime_error("Login required"); // pehle login karo
        }
        const UserRole role = users_.at(activeUserId_).role;
        if (static_cast<int>(role) < static_cast<int>(minimumRole)) {
            throw std::runtime_error("Insufficient permissions"); // role chhota hai
        }
    }

    // ---- "get-or-throw" helpers — na mile to saaf error (silent nullptr nahi) ---
    // Ye "fail-fast" hai: galat id do to turant pata chal jaata hai kahan gadbad hui.
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

    // Role enum -> string (login print ke liye).
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
