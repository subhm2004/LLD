// C++17 — LLD Design Pattern: Facade acting as an Error Boundary, catching subsystem exceptions and returning standard std::optional to UI layer.
#include <bits/stdc++.h>
#include <optional>
using namespace std;

/**
 * @class SeatBookingService
 * @brief Subsystem level service jo database ya operations errors throw kar sakti hai.
 */
class SeatBookingService {
public:
    void bookSeat(const string &seatId, const string &userId) {
        if (seatId.empty() || userId.empty()) {
            throw invalid_argument("seatId and userId required");
        }
        if (booked_.count(seatId)) {
            throw runtime_error("Seat already booked: " + seatId);
        }
        booked_.insert(seatId);
    }

private:
    unordered_set<string> booked_;
};

/**
 * @class MovieBookingFacade
 * @brief Facade Pattern jo subsystems ko wrap karta hai aur errors filter out karta hai (Error Boundary).
 * 
 * LLD Best Practice: UI / Controller layer ko direct raw exceptions catch-block handle karne ke liye 
 * force nahi karna chahiye. Facade exceptions ko andar hi wrap karke safely `std::optional` ya failure structure
 * return karta hai taaki interface cleaner aur safe rahe.
 */
class MovieBookingFacade {
public:
    explicit MovieBookingFacade(SeatBookingService &svc) : svc_(svc) {}

    // Method jo seat book karne ki koshish karega.
    // Sahi booking par confirmation message, warna failure par std::nullopt (empty state) return karega.
    optional<string> tryBookSeat(const string &seatId, const string &userId) {
        try {
            svc_.bookSeat(seatId, userId);
            return optional<string>("Booking confirmed for " + seatId);
        } 
        // Validation errors handler
        catch (const invalid_argument &ex) {
            lastError_ = string("VALIDATION: ") + ex.what();
            return nullopt;
        } 
        // Runtime business errors handler
        catch (const runtime_error &ex) {
            lastError_ = string("BUSINESS: ") + ex.what();
            return nullopt;
        } 
        // Catch all block for safety
        catch (...) {
            lastError_ = "UNKNOWN error";
            return nullopt;
        }
    }

    // Aakhri error message fetch karne ke liye getter.
    const string &getLastError() const { return lastError_; }

private:
    SeatBookingService &svc_; // reference to service layer
    string lastError_;        // status errors tracking variable
};

int main() {
    cout << "=== 13 LLD Facade error boundary ===\n";

    SeatBookingService service;
    MovieBookingFacade facade(service);

    // Case 1: First booking success path.
    if (auto msg = facade.tryBookSeat("A1", "user1"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    // Case 2: Duplicate booking (Business rule failure).
    if (auto msg = facade.tryBookSeat("A1", "user2"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    // Case 3: Empty inputs (Validation logic failure).
    if (auto msg = facade.tryBookSeat("", "user3"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    return 0;
}
