/**
 * ============================================================================
 *  04_Dependency.cpp  —  DEPENDENCY (object relationship #4, sabse WEAK)
 * ----------------------------------------------------------------------------
 *  Dependency = "ek class dusri ko sirf KISI METHOD ke andar (parameter ya
 *                local variable ki tarah) use karti hai — FIELD me store NAHI
 *                karti. Method khatam, collaboration khatam. 'uses-a temporarily'."
 *
 *  Asli duniya ka example: OrderService aur Logger/PaymentGateway 🛒
 *    OrderService order place karte waqt logger aur payment gateway USE karti
 *    hai — par unhe apne paas RAKHTI nahi (member field nahi). Jaise plumber
 *    kaam ke waqt tumhare tools use karta hai par le ke nahi jaata.
 *
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │  ASSOCIATION vs DEPENDENCY (weakest do — kaise distinguish karein):     │
 *  │                                                                          │
 *  │  Association -> object ko FIELD me rakha jaata (long-term "knows")       │
 *  │                 Teacher ke paas students_ field HAI                      │
 *  │  Dependency  -> object sirf METHOD ke andar use hota (short-term "uses") │
 *  │                 OrderService ke paas logger ka field NAHI — sirf param   │
 *  │                                                                          │
 *  │  Simple test: "kya class me uska field/member hai?"                     │
 *  │    Haan -> Association (ya aggregation/composition)                      │
 *  │    Nahi, sirf method me -> Dependency                                    │
 *  │  UML: Dependency = DASHED arrow ..>  (sabse weak, temporary)             │
 *  └──────────────────────────────────────────────────────────────────────────┘
 *
 *  📌 DEPENDENCY ki pehchaan (is file me):
 *  - OrderService ke paas Logger/PaymentGateway ka KOI FIELD nahi
 *  - Ye dono sirf placeOrder() ke PARAMETERS hain (method scope me hi zinda)
 *  - Method khatam -> collaboration khatam
 *
 *  ⭐ YEHI DEPENDENCY INJECTION ka base hai! Collaborators bahar se method me
 *  pass karne se testing me MOCK/FAKE pass kar sakte ho (real payment gateway
 *  ki jagah fake). Loose coupling — OrderService kisi CONCRETE logger se
 *  bandha nahi. Isliye interviews me "DI" itni important hai.
 */
#include <iostream>
#include <string>
using namespace std;

class Logger {
public:
    void log(const string& msg) const {
        cout << "[Logger] " << msg << "\n";
    }
};

class PaymentGateway {
public:
    bool charge(double amount) const {
        cout << "[PaymentGateway] charged Rs " << amount << "\n";
        return true;
    }
};

class OrderService {
    string orderId;
public:
    explicit OrderService(string id) : orderId(id) {}

    // Dependency: Logger & PaymentGateway sirf PARAMETER hain, member nahi.
    // Method ke bahar OrderService inhe "have" nahi karti.
    void placeOrder(double amount, Logger& logger, PaymentGateway& gateway) const {
        logger.log("Placing order " + orderId);
        if (gateway.charge(amount))
            logger.log("Order " + orderId + " confirmed");
    }
};

int main() {
    OrderService order("ORD-101");

    // Collaborators BAHAR bante hain aur METHOD me pass hote — temporary use.
    Logger fileLogger;
    PaymentGateway razorpay;

    // placeOrder ke andar hi logger/gateway use honge, uske baad "bhool" jaate.
    // (Testing me yahan FakeLogger/FakePaymentGateway pass kar sakte — DI!)
    order.placeOrder(499.0, fileLogger, razorpay);

    // >>> DEPENDENCY KA PROOF <<<
    // OrderService ke paas logger/gateway ka koi field NAHI — usne sirf method
    // ke andar use kiya. Association hota to field me rakha hota.
    cout << "[Dependency] OrderService has no Logger field — only uses in method\n";
    return 0;
}

/**
 * EXPECTED OUTPUT:
 *   [Logger] Placing order ORD-101
 *   [PaymentGateway] charged Rs 499
 *   [Logger] Order ORD-101 confirmed
 *   [Dependency] OrderService has no Logger field — only uses in method
 *
 *  ============================================================================
 *   ⭐ CHAARO RELATIONSHIPS KA FINAL SUMMARY (interview revision):
 *  ----------------------------------------------------------------------------
 *   Strength | Relationship | Real example    | Key sign
 *   ---------+--------------+-----------------+---------------------------------
 *   Weakest  | Dependency   | Order-Logger    | method PARAMETER (no field)
 *      |     | Association  | Teacher-Student | FIELD, knows, not owns
 *      |     | Aggregation  | Car-Engine      | has-a, part reusable/external
 *   Strongest| Composition  | House-Room      | has-a, part DIES with whole
 *
 *   Yaad rakho: Dependency "uses", Association "knows", Aggregation "has
 *   (loosely)", Composition "owns (exclusively)". Coupling weak se strong.
 *  ============================================================================
 */
