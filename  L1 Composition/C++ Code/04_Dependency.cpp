/**
 * DEPENDENCY — temporary use; weakest link
 * OrderService uses Logger only inside a method (parameter / local)
 * Logger is NOT a permanent field of OrderService
 * UML: dashed arrow ..>
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

    // Dependency: Logger only appears as method parameter — not stored
    void placeOrder(double amount, Logger& logger, PaymentGateway& gateway) const {
        logger.log("Placing order " + orderId);
        if (gateway.charge(amount))
            logger.log("Order " + orderId + " confirmed");
    }
};

int main() {
    OrderService order("ORD-101");

    // Dependencies created / passed at call time — temporary collaboration
    Logger fileLogger;
    PaymentGateway razorpay;

    order.placeOrder(499.0, fileLogger, razorpay);

    // OrderService does not "have" Logger — no member field
    cout << "[Dependency] OrderService has no Logger field — only uses in method\n";
    return 0;
}
