// ============================================================================
//  services/InventoryService.h  —  Stock + RESERVATION system (is LLD ka highlight)
// ----------------------------------------------------------------------------
//  Ye sirf "kitna stock hai" nahi rakhta — ye ek do-phase RESERVATION system hai
//  jo checkout ke dauran stock ko safely "hold" karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ RESERVE -> COMMIT / RELEASE  (2-phase, "saga" jaisa pattern)         │
//  │                                                                          │
//  │   1. reserveItems()  -> stock ko available se HATA ke "reserved" me daalo │
//  │                         (payment se PEHLE — taaki koi aur na le jaye)     │
//  │   2a. commitReservation() -> payment SUCCESS -> reservation khatam        │
//  │                              (stock permanently gaya — sahi hai)          │
//  │   2b. releaseReservation() -> payment FAIL -> stock WAPAS available me    │
//  │                                                                          │
//  │  ⭐ PEHLE RESERVE KYUN (payment ke baad kyun nahi)?                       │
//  │  Warna: 2 log ek hi last item ke liye payment karte, dono ka paisa kat    │
//  │  jaata, par item ek hi hai! Reserve pehle karke hum us race ko rok dete   │
//  │  hain — jiska reserve hua wahi payment karega.                           │
//  │                                                                          │
//  │  ⭐ AUR RELEASE KYUN ZAROORI? Warna payment fail hone pe stock hamesha ke │
//  │  liye "atka" reh jaata (na kisi ko mila, na wapas aaya) — inventory leak. │
//  │  (CheckoutService ke catch(...) me release ka guarantee dekho.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `releaseReservation` IDEMPOTENT hai — ye zaroori design hai           │
//  │                                                                          │
//  │  Agar reservation mila hi nahi to chup-chaap `return` (error nahi).      │
//  │  Kyun important? CheckoutService failure path me release DO baar call     │
//  │  hota hai (ek explicit, ek catch(...) se). Idempotent hone ki wajah se    │
//  │  doosri call NO-OP hai — stock DOUBLE wapas nahi hota. 📌 Warna stock     │
//  │  badhta jaata (phantom inventory)! Ye chhota `return` bahut kuch bachata. │
//  │  (commitReservation ke baad release call ho to bhi no-op — sahi behavior.)│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ THREAD-SAFETY: ye single-threaded design hai. Concurrent checkout ke liye
//     mutex chahiye hoga (warna do threads ek saath stock ghata sakte). Real
//     system me DB row-lock / atomic decrement hota. Interview me ye bolna accha.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_INVENTORYSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_INVENTORYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ecommerce_checkout_lld {

using namespace std;

class InventoryService {
public:
    void setStock(const string &productId, int quantity) {
        if (quantity < 0) {
            throw invalid_argument("Stock cannot be negative");
        }
        availableStock_[productId] = quantity;
    }

    // Product register hi nahi hua -> 0 stock (na ki error) — safe default.
    int getAvailableStock(const string &productId) const {
        auto it = availableStock_.find(productId);
        return it == availableStock_.end() ? 0 : it->second;
    }

    // Saare requested items ka stock hai? Nahi to saaf error (kaunsa product,
    // kitna maanga, kitna hai — sab message me).
    void validateAvailability(const unordered_map<string, int> &requestedItems) const {
        for (const auto &entry : requestedItems) {
            const int available = getAvailableStock(entry.first);
            if (entry.second > available) {
                throw runtime_error("Insufficient stock for product: " + entry.first +
                                    " (requested=" + to_string(entry.second) +
                                    ", available=" + to_string(available) + ")");
            }
        }
    }

    // ---- PHASE 1: stock HOLD karo (payment se pehle) ----------------------
    string reserveItems(const string &userId, const unordered_map<string, int> &requestedItems) {
        validateAvailability(requestedItems); // pehle poora check (partial reserve na ho)
        const string reservationId = "RES_" + userId + "_" + to_string(++reservationCounter_);

        for (const auto &entry : requestedItems) {
            availableStock_[entry.first] -= entry.second;              // available se hatao
            reservedStock_[reservationId][entry.first] = entry.second; // reserved me daalo
        }
        reservationOwner_[reservationId] = userId;
        return reservationId; // ye id checkout ke paas rahegi (commit/release ke liye)
    }

    // ---- PHASE 2a: payment SUCCESS -> reservation khatam (stock gaya) ------
    void commitReservation(const string &reservationId) {
        auto reservationIt = reservedStock_.find(reservationId);
        if (reservationIt == reservedStock_.end()) {
            throw runtime_error("Reservation not found: " + reservationId);
        }
        // Stock wapas NAHI karte — bik gaya. Bas reservation record hata do.
        reservedStock_.erase(reservationIt);
        reservationOwner_.erase(reservationId);
    }

    // ---- PHASE 2b: payment FAIL -> stock WAPAS available me ---------------
    void releaseReservation(const string &reservationId) {
        auto reservationIt = reservedStock_.find(reservationId);
        if (reservationIt == reservedStock_.end()) {
            return; // ⭐ IDEMPOTENT: pehle se released/committed -> no-op (upar note)
        }
        for (const auto &entry : reservationIt->second) {
            availableStock_[entry.first] += entry.second; // wapas available
        }
        reservedStock_.erase(reservationIt);
        reservationOwner_.erase(reservationId);
    }

private:
    unordered_map<string, int> availableStock_;                        // productId -> free qty
    unordered_map<string, unordered_map<string, int>> reservedStock_;  // resId -> {product: qty}
    unordered_map<string, string> reservationOwner_;                   // resId -> userId (audit)
    int reservationCounter_{0};
};

}  // namespace ecommerce_checkout_lld

#endif
