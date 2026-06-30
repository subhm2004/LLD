// ============================================================================
//  14_Cohesion_Coupling.cpp  —  Cohesion & Coupling
// ----------------------------------------------------------------------------
//  COHESION = ek class kitni focused hai (uske members ek hi kaam se related
//  hon -> HIGH cohesion = achha). COUPLING = classes ek doosre par kitni
//  dependent hain (kam dependency -> LOW coupling = achha).
//  Goal: HIGH cohesion + LOW coupling. Yeh maintainable, testable design ki
//  pehchaan hai (SOLID isi ko enforce karta hai).
// ============================================================================
#include <iostream>
#include <string>
using namespace std;

// --- LOW cohesion + HIGH coupling (anti-pattern) ---
class GodOrderProcessor {
    string dbConnection = "fake-db";
public:
    void processOrder(const string& orderId) {
        cout << "[GodClass] validate + charge + email + log — sab ek class me\n";
        cout << "  DB(" << dbConnection << ") save " << orderId << endl;
        cout << "  PaymentGateway charge\n";
        cout << "  SMTP send receipt\n";
        cout << "  File logger\n";
        // Change payment provider → is class ko edit; change email → same class
    }
};

// --- HIGH cohesion + LOW coupling ---
class OrderRepository {
public:
    void save(const string& id) { cout << "  Repo: persisted " << id << endl; }
};

class PaymentService {
public:
    bool charge(const string& id, int amount) {
        cout << "  Payment: charged Rs " << amount << " for " << id << endl;
        return true;
    }
};

class EmailNotifier {
public:
    void sendReceipt(const string& id) { cout << "  Email: receipt for " << id << endl; }
};

class OrderService {  // orchestrates — single responsibility: workflow
    OrderRepository& repo;
    PaymentService& pay;
    EmailNotifier& mail;
public:
    OrderService(OrderRepository& r, PaymentService& p, EmailNotifier& m)
        : repo(r), pay(p), mail(m) {}

    void processOrder(const string& orderId, int amount) {
        cout << "[OrderService] orchestration only\n";
        if (!pay.charge(orderId, amount)) return;
        repo.save(orderId);
        mail.sendReceipt(orderId);
    }
};

int main() {
    cout << "=== Low cohesion / High coupling ===\n";
    GodOrderProcessor bad;
    bad.processOrder("ORD-1");

    cout << "\n=== High cohesion / Low coupling ===\n";
    OrderRepository repo;
    PaymentService pay;
    EmailNotifier mail;
    OrderService good(repo, pay, mail);
    good.processOrder("ORD-2", 499);

    cout << "\nCohesion: class ke andar methods ek hi purpose ke (SRP).\n";
    cout << "Coupling: classes kam jaante hain ek doosre ke internals — interfaces/refs.\n";
    return 0;
}
