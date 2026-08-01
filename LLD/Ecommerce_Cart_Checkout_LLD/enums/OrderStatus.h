// ============================================================================
//  enums/OrderStatus.h  —  Ek order ki zindagi ke stages
// ----------------------------------------------------------------------------
//     CREATED         -> order object bana (abhi kuch hua nahi)
//     PAYMENT_PENDING -> payment gateway ko bheja, jawab ka intezaar
//     CONFIRMED       -> payment ok (ya COD accept) -> order pakka
//     SHIPPED         -> nikal gaya (is demo me use nahi — future lifecycle)
//     DELIVERED       -> pahunch gaya
//     CANCELLED       -> cancel kiya gaya
//     FAILED          -> payment fail -> order fail
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CHECKOUT ka asli flow (CheckoutService me dekho)                      │
//  │                                                                          │
//  │     CREATED ──► PAYMENT_PENDING ──► CONFIRMED   (payment success)        │
//  │                        └─────────► FAILED       (payment fail)          │
//  │                                                                          │
//  │  ⭐ PAYMENT_PENDING wali beech ki state ZAROORI kyun hai?                 │
//  │  Kyunki payment ke DAURAN system crash ho sakta hai. Agar order already   │
//  │  PAYMENT_PENDING me save hai, to baad me pata chalta hai "iska payment    │
//  │  beech me atka tha" -> use reconcile kiya ja sakta (gateway se pucho      │
//  │  paisa kata ya nahi). Seedha CREATED se CONFIRMED jaate to ye jaankari    │
//  │  kho jaati aur "paisa kata par order nahi bana" wali haalat ban sakti.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 ORDER status aur PAYMENT status ALAG hain (PaymentStatus.h dekho) — COD me
//     order CONFIRMED hota hai par payment abhi PENDING (delivery pe milega).
//     Isi liye do alag enums — ek hi enum me dono mix karte to COD express hi na hota.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_ORDERSTATUS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_ORDERSTATUS_H

#include <string>

namespace ecommerce_checkout_lld {

using namespace std;

enum class OrderStatus {
    CREATED,
    PAYMENT_PENDING,
    CONFIRMED,
    SHIPPED,
    DELIVERED,
    CANCELLED,
    FAILED
};

inline string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::CREATED:         return "CREATED";
        case OrderStatus::PAYMENT_PENDING: return "PAYMENT_PENDING";
        case OrderStatus::CONFIRMED:       return "CONFIRMED";
        case OrderStatus::SHIPPED:         return "SHIPPED";
        case OrderStatus::DELIVERED:       return "DELIVERED";
        case OrderStatus::CANCELLED:       return "CANCELLED";
        case OrderStatus::FAILED:          return "FAILED";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif
