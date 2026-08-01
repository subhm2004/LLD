// ============================================================================
//  services/CheckoutService.h  —  ⭐ POORE SYSTEM KA DIL (orchestration)
// ----------------------------------------------------------------------------
//  Ye khud koi business rule NAHI banata — ye 6 services ko SAHI ORDER me,
//  sahi rollback ke saath chalata hai. Yahi is LLD ka sabse important file hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ CHECKOUT KA POORA FLOW (step by step)                               │
//  │                                                                          │
//  │   0. IDEMPOTENCY CHECK  -> is clientRequestId ka order pehle se hai?      │
//  │                            haan -> WAHI lauta do (naya payment NAHI)      │
//  │   1. Cart uthao         -> khaali hua to yahi throw                       │
//  │   2. STOCK RESERVE ⚠    -> yahan se "undo" ki zimmedari shuru hoti hai   │
//  │   3. Pricing            -> subtotal -> coupon -> breakup                  │
//  │   4. OrderLines banao   -> cart items ka SNAPSHOT                         │
//  │   5. Order banao        -> status = PAYMENT_PENDING (crash-safe marker)   │
//  │   6. PAYMENT chalao     -> Strategy (card/upi/cod)                        │
//  │   7a. FAIL -> stock RELEASE + order FAILED save + throw                   │
//  │   7b. PASS -> stock COMMIT + txnId + status CONFIRMED                     │
//  │   8. Cart khaali karo + order save                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ ORDER OF OPERATIONS — kyun yahi sequence?                           │
//  │                                                                          │
//  │  RESERVE payment se PEHLE kyun? -> Warna 2 log ek hi last item ka paisa   │
//  │     de dete, item ek hi hota. Reserve pehle = race jeet li.               │
//  │  Order object payment se PEHLE kyun banta hai? -> Taaki payment fail hone │
//  │     pe bhi ek FAILED order RECORD bache. User ko "kya hua tha" dikhta hai,│
//  │     support team ko debug karne ko milta hai. Bina record ke failure       │
//  │     invisible ho jaata.                                                   │
//  │  clearCart sabse AAKHIR me kyun? -> Beech me kuch bhi fail ho to user ka  │
//  │     cart SALAAMAT rahe. Pehle cart khaali kar dete aur payment fail hota  │
//  │     to user ka saara saman gayab! 😱 Destructive kaam hamesha last.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ `try / catch(...)` = ROLLBACK GUARANTEE                             │
//  │                                                                          │
//  │  Reserve ke baad ka SAARA code try me hai. Kahin se bhi exception aaye —  │
//  │  coupon invalid, product gayab, payment failure, ya koi bhi anjaan error  │
//  │  — catch(...) reservation RELEASE karta hai aur phir `throw;` se error    │
//  │  wapas upar bhej deta hai (nigalta nahi).                                 │
//  │                                                                          │
//  │  📌 `throw;` (bare) — SAME exception ko rethrow karta hai, uska type aur  │
//  │     message preserve rehta hai. `throw e;` likhte to slicing ho sakti.    │
//  │                                                                          │
//  │  ⭐ DOUBLE RELEASE ka sawaal: payment-fail wale branch me humne EXPLICITLY │
//  │     release kiya, phir wahi branch throw karta hai -> catch(...) DOBARA   │
//  │     release karta hai. Bug? NAHI —                                        │
//  │     `InventoryService::releaseReservation` IDEMPOTENT hai (reservation    │
//  │     nahi mila to chup-chaap return). Doosri call no-op hai, stock double  │
//  │     wapas NAHI hota. Ye ek deliberate design contract hai — InventoryService│
//  │     ka wo `return` isi liye hai.                                          │
//  │                                                                          │
//  │  ⚠ Explicit release rakha hi kyun jab catch bhi karta hai? Taaki order    │
//  │     FAILED status me save hone se PEHLE stock free ho jaye — order record │
//  │     aur inventory ek dusre se consistent rahein.                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Ek honest gap: agar payment SUCCESS ho gaya par uske BAAD kuch fail ho
//     (commit/save), to catch(...) reservation release kar dega — par paisa to
//     kat chuka hai. Real system me yahan compensating transaction (auto-refund)
//     ya outbox+reconciliation job hota hai. Isi liye PAYMENT_PENDING status
//     exist karta hai — crash ke baad reconcile karne ke liye. (OrderStatus.h)
//
//  ⚠ `int &orderCounter` by-reference aata hai — counter facade ka hai, checkout
//     usko badhata hai. Thoda awkward hai (ideally ek IdGenerator service hoti),
//     par isse CheckoutService stateless rehta hai.
//
//  📌 SAARE members REFERENCES hain (owning nahi). CheckoutService kisi ko
//     banata/delete nahi karta — bas use karta hai. Isi wajah se facade me
//     copy/move `= delete` hai (core/EcommerceCheckoutSystem.h ka note padho).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CHECKOUTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CHECKOUTSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../enums/OrderStatus.h"
#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"
#include "../models/Order.h"
#include "../models/OrderLine.h"
#include "CartService.h"
#include "CouponService.h"
#include "InventoryService.h"
#include "OrderService.h"
#include "PaymentService.h"
#include "PricingService.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CheckoutService {
public:
    // Saari dependencies INJECTED (6 references) — koi bhi khud nahi banti.
    CheckoutService(CartService &cartService,
                    InventoryService &inventoryService,
                    CouponService &couponService,
                    PricingService &pricingService,
                    PaymentService &paymentService,
                    OrderService &orderService)
        : cartService_(cartService),
          inventoryService_(inventoryService),
          couponService_(couponService),
          pricingService_(pricingService),
          paymentService_(paymentService),
          orderService_(orderService) {}

    Order checkout(const string &userId,
                   const string &addressId,
                   PaymentMethod paymentMethod,
                   const string &paymentInstrument,
                   const string &couponCode,
                   const string &clientRequestId,
                   int currentDay,
                   int &orderCounter) {
        // ---- STEP 0: IDEMPOTENCY — ye request pehle process ho chuki hai? ----
        // Double-click / network retry pe naya order aur DOOSRA payment nahi hoga.
        if (!clientRequestId.empty()) {
            const Order *existing = orderService_.findByClientRequestId(clientRequestId);
            if (existing != nullptr) {
                return *existing; // ⭐ purana order hi laut jaata hai
            }
        }

        // ---- STEP 1: Cart uthao (khaali hua to yahi throw — abhi kuch reserve nahi) ----
        const Cart &cart = cartService_.getCartOrThrow(userId);
        const unordered_map<string, int> requestedItems = cartService_.toQuantityMap(cart);

        // ---- STEP 2: ⚠ STOCK RESERVE — yahan se rollback ki zimmedari shuru ----
        // Stock nahi hai to yahi throw (abhi tak koi side-effect nahi hua, safe).
        const string reservationId = inventoryService_.reserveItems(userId, requestedItems);

        try {
            // ---- STEP 3: PRICING (subtotal -> coupon -> final breakup) --------
            const double subtotal = cart.subtotal();
            // Coupon galat/expired -> throw -> catch(...) reservation release karega ✅
            const double discount =
                couponService_.applyCouponOrThrow(couponCode, subtotal, currentDay);
            const PriceBreakup breakup = pricingService_.calculate(subtotal, discount);

            // ---- STEP 4: OrderLines = cart items ka SNAPSHOT ------------------
            // Naam/price COPY hote hain — baad me catalog badle to purana bill na badle.
            vector<OrderLine> lines;
            for (const auto &entry : cart.getItems()) {
                const CartItem &item = entry.second;
                lines.emplace_back(item.getProduct().getProductId(), item.getProduct().getName(),
                                   item.getProduct().getUnitPrice(), item.getQuantity());
            }

            // ---- STEP 5: Order banao — payment se PEHLE (failure ka record bache) ----
            const string orderId = "ORD_" + to_string(++orderCounter);
            Order order(orderId, userId, addressId, std::move(lines), breakup.subtotal,
                        breakup.discount, breakup.shippingFee, breakup.payableAmount, paymentMethod,
                        couponCode);
            order.setClientRequestId(clientRequestId);           // agli baar idempotency ke liye
            order.setStatus(OrderStatus::PAYMENT_PENDING);       // ⭐ "paisa maanga hai, jawab nahi aaya"

            // ---- STEP 6: PAYMENT (Strategy — card/upi/cod) ---------------------
            const PaymentResult paymentResult =
                paymentService_.processPayment(paymentMethod, breakup.payableAmount,
                                               paymentInstrument);

            // ---- STEP 7a: PAYMENT FAIL -> rollback + FAILED order save + throw ----
            if (!paymentResult.isSuccess()) {
                inventoryService_.releaseReservation(reservationId); // stock wapas (order save se pehle)
                order.setStatus(OrderStatus::FAILED);
                order.setPaymentStatus(PaymentStatus::FAILED);
                orderService_.saveOrder(std::move(order));          // ⭐ failure ka RECORD rehta hai
                // Neeche wala throw catch(...) me jayega -> release DOBARA hoga
                // -> par idempotent hai, no-op. (Upar wala note padho.)
                throw runtime_error("Payment failed: " + paymentResult.getMessage());
            }

            // ---- STEP 7b: PAYMENT PASS -> stock permanently consume -----------
            inventoryService_.commitReservation(reservationId);
            order.setPaymentTransactionId(paymentResult.getTransactionId()); // gateway ref

            // ⭐ COD ka special case: order CONFIRMED hai par paisa abhi tak nahi
            //    aaya (delivery pe aayega) -> PaymentStatus PENDING.
            //    Do alag enums hone ka poora fayda yahi hai (PaymentStatus.h).
            if (paymentMethod == PaymentMethod::COD) {
                order.setPaymentStatus(PaymentStatus::PENDING);
                order.setStatus(OrderStatus::CONFIRMED);
            } else {
                order.setPaymentStatus(PaymentStatus::SUCCESS);
                order.setStatus(OrderStatus::CONFIRMED);
            }

            // ---- STEP 8: Cart khaali (SABSE AAKHIR — destructive kaam last) ----
            cartService_.clearCart(userId);
            orderService_.saveOrder(std::move(order)); // ⚠ yahan `order` moved-from ho gaya
            // Isi liye neeche STORE kiya hua order padha jaata hai (moved-from wala nahi).
            Order &savedOrder = orderService_.getOrderOrThrow(orderId);
            return savedOrder; // by-value return -> caller ko apni copy milti hai
        } catch (...) {
            // ⭐⭐ ROLLBACK GUARANTEE: koi bhi exception ho, stock atka nahi rahega.
            inventoryService_.releaseReservation(reservationId);
            throw; // bare rethrow — original exception type/message preserve
        }
    }

private:
    // Saare references — CheckoutService kisi ko OWN nahi karta (sirf use karta hai).
    CartService &cartService_;
    InventoryService &inventoryService_;
    CouponService &couponService_;
    PricingService &pricingService_;
    PaymentService &paymentService_;
    OrderService &orderService_;
};

}  // namespace ecommerce_checkout_lld

#endif
