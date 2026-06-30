/**
 * ============================================================================
 *  04_Dependency.cpp  —  DEPENDENCY (sabse weak link, temporary use)
 * ----------------------------------------------------------------------------
 *  Dependency = ek class dusri ko sirf KISI METHOD ke andar (parameter ya
 *  local variable ki tarah) use karti hai — usse field me store NAHI karti.
 *  "...uses-a temporarily". Jaise hi method khatam, collaboration khatam.
 *  Yahan OrderService Logger/PaymentGateway ko sirf placeOrder() ke argument
 *  ke roop me leti hai — koi member field nahi. UML: dashed arrow ..>
 *  (Yahi dependency injection ka base hai -> testing me mock pass kar sakte ho.)
 * ============================================================================
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

    // Collaborators call ke waqt bante/pass hote hain — temporary collaboration.
    Logger fileLogger;
    PaymentGateway razorpay;

    order.placeOrder(499.0, fileLogger, razorpay);

    // OrderService ke paas Logger ka koi field nahi — sirf method me use hua.
    cout << "[Dependency] OrderService has no Logger field — only uses in method\n";
    return 0;
}
