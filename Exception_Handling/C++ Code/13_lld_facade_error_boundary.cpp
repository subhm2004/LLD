// C++17 — LLD pattern: Facade catches exceptions, returns bool/optional to UI layer
#include <bits/stdc++.h>
#include <optional>
using namespace std;

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

// Facade = error boundary — main.cpp ko raw exceptions nahi dikhte
class MovieBookingFacade {
public:
    explicit MovieBookingFacade(SeatBookingService &svc) : svc_(svc) {}

    optional<string> tryBookSeat(const string &seatId, const string &userId) {
        try {
            svc_.bookSeat(seatId, userId);
            return optional<string>("Booking confirmed for " + seatId);
        } catch (const invalid_argument &ex) {
            lastError_ = string("VALIDATION: ") + ex.what();
            return nullopt;
        } catch (const runtime_error &ex) {
            lastError_ = string("BUSINESS: ") + ex.what();
            return nullopt;
        } catch (...) {
            lastError_ = "UNKNOWN error";
            return nullopt;
        }
    }

    const string &getLastError() const { return lastError_; }

private:
    SeatBookingService &svc_;
    string lastError_;
};

int main() {
    cout << "=== 13 LLD Facade error boundary ===\n";

    SeatBookingService service;
    MovieBookingFacade facade(service);

    if (auto msg = facade.tryBookSeat("A1", "user1"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    if (auto msg = facade.tryBookSeat("A1", "user2"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    if (auto msg = facade.tryBookSeat("", "user3"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    return 0;
}
