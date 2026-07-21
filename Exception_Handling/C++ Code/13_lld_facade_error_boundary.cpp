// ============================================================================
//  13_lld_facade_error_boundary.cpp  —  Facade as "Error Boundary" (LLD pattern)
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/13_lld_facade_error_boundary.cpp" -o bin/13_lld_facade_error_boundary
//
//  Ye ek real LLD design pattern hai: subsystem services exceptions THROW karti
//  hain (unka kaam validate + fail-fast). Par UI/Controller layer ko har jagah
//  try/catch likhna nahi chahiye. Beech me ek FACADE baithta hai jo saari
//  exceptions ko andar hi CATCH karke, upar ek SAAF result (optional) lauta ta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ERROR BOUNDARY — "exceptions yahin rukengi, aage nahi jaayengi"       │
//  │                                                                          │
//  │     UI Layer                                                             │
//  │       │  optional<string> maangta hai (has_value? -> ok : error)         │
//  │       ▼                                                                  │
//  │     FACADE (error boundary)  ← saari try/catch YAHAN, ek hi jagah         │
//  │       │  service ko call karta hai, exceptions ko pakad ke optional banata│
//  │       ▼                                                                  │
//  │     Service (throws!)  ← validation/business rules, fail-fast            │
//  │                                                                          │
//  │  Fayda: UI simple rehta (bas has_value check), aur error-handling ek      │
//  │  jagah centralize ho jaati hai (consistent logging, uniform response).   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye files 07 (service throws) + 09 (optional return) + 04 (catch order) ka
//     SANGAM hai — teeno ideas ek real LLD flow me. Facade ke andar catch order
//     bhi sahi hai (invalid_argument -> runtime_error -> catch(...) safety net).
//
//  📌 `lastError_` me error string bhi rakhta hai — taaki UI ko sirf "fail hua"
//     nahi, "kyun fail hua" (VALIDATION/BUSINESS/UNKNOWN) bhi mil sake.
// ============================================================================
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>

using namespace std;

// ---- Subsystem service — ye THROW karti hai (uska kaam validate + fail-fast) --
class SeatBookingService {
public:
    void bookSeat(const string &seatId, const string &userId) {
        if (seatId.empty() || userId.empty()) {
            throw invalid_argument("seatId and userId required"); // validation error
        }
        if (booked_.count(seatId)) {
            throw runtime_error("Seat already booked: " + seatId); // business error
        }
        booked_.insert(seatId);
    }

private:
    unordered_set<string> booked_;
};

// ---- Facade = error boundary. Service ko wrap karta, exceptions ko optional banata --
class MovieBookingFacade {
public:
    explicit MovieBookingFacade(SeatBookingService &svc) : svc_(svc) {}

    // ⭐ Success -> optional me confirmation. Fail -> nullopt + lastError_ set.
    //    UI ko exception dikhti hi nahi — sab yahin handle ho gaya.
    optional<string> tryBookSeat(const string &seatId, const string &userId) {
        try {
            svc_.bookSeat(seatId, userId);
            return optional<string>("Booking confirmed for " + seatId);
        }
        // Catch order (file 04): specific pehle...
        catch (const invalid_argument &ex) {
            lastError_ = string("VALIDATION: ") + ex.what();
            return nullopt;
        }
        catch (const runtime_error &ex) {
            lastError_ = string("BUSINESS: ") + ex.what();
            return nullopt;
        }
        // ...aur catch(...) sabse aakhri me (safety net — kabhi koi anokha error aaya to).
        catch (...) {
            lastError_ = "UNKNOWN error";
            return nullopt;
        }
    }

    const string &getLastError() const { return lastError_; }

private:
    SeatBookingService &svc_; // service ka reference (facade use karta, own nahi)
    string lastError_;        // aakhri failure ka reason (UI ke liye)
};

int main() {
    cout << "=== 13 LLD Facade error boundary ===\n";

    SeatBookingService service;
    MovieBookingFacade facade(service);

    // ⭐ Notice: UI me EK bhi try/catch nahi — sirf has_value check. Saaf.

    // Case 1: pehli booking -> success
    if (auto msg = facade.tryBookSeat("A1", "user1"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    // Case 2: wahi seat dobara -> business error (already booked)
    if (auto msg = facade.tryBookSeat("A1", "user2"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    // Case 3: khaali input -> validation error
    if (auto msg = facade.tryBookSeat("", "user3"); msg.has_value()) {
        cout << "OK: " << *msg << "\n";
    } else {
        cout << "Fail: " << facade.getLastError() << "\n";
    }

    return 0;
}

// Expected output:
//   OK: Booking confirmed for A1
//   Fail: BUSINESS: Seat already booked: A1
//   Fail: VALIDATION: seatId and userId required
