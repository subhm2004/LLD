// ============================================================================
//  PaymentGatewayApplication.cpp — PAYMENT GATEWAY (single-file MONOLITH)
// ----------------------------------------------------------------------------
//  Poora multi-gateway payment system EK file me — original version. Modular
//  folder-based version parent folder me hai (gateways/, proxy/, retry/,
//  factories/, services/, controllers/). Detail: ../design_patterns_used.md
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  EK PAYMENT KA SAFAR (5 patterns milke):                                │
//  │   Controller [SINGLETON] -> GatewayFactory [FACTORY] -> Proxy [PROXY]   │
//  │      -> real Gateway [TEMPLATE METHOD: validate->initiate->confirm]     │
//  │      -> BankingSystem [STRATEGY], retry pe RetryStrategy [STRATEGY]     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS FILE KA LAYOUT: PaymentRequest -> BankingSystem (Strategy) ->
//  PaymentGateway (Template Method) + concrete gateways -> RetryStrategy
//  (Strategy) -> Proxy -> Factories -> Service -> Controller -> main.
//
//  5 PATTERNS: Template Method + Strategy + Proxy + Factory + Singleton.
//  Modular version me ye sab alag files me clean hain; ye monolith "sab ek
//  jagah" reference ke liye hai. Dono ka behavior same.
// ============================================================================
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

// ----------------------------
// Data structure for payment details
// ----------------------------
// PaymentRequest struct transaction ki saari details ko represent karti hai.
// Isme payer, payee, transfer amount aur currency define ki jaati hai.
struct PaymentRequest {
  string sender;     // Payer (paisa bhejne wala) ka naam
  string reciever;   // Payee (paisa receive karne wala) ka naam
  double amount;     // Kitna amount transfer karna hai
  string currency;   // Transaction ki currency type (e.g. INR, USD)

  // Constructor: Request fields ko easily initialize karne ke liye
  PaymentRequest(const string &sender, const string &reciever, double amt,
                 const string &curr) {
    this->sender = sender;
    this->reciever = reciever;
    this->amount = amt;
    this->currency = curr;
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Strategy Pattern]
// ----------------------------------------------------------------------------
// BankingSystem ek abstraction hai jo banking API commands ko execute karta hai.
// PaytmBankingSystem aur RazorpayBankingSystem iske concrete implementations hain.
// PaymentGateway is BankingSystem interface ko compile-time ya runtime par hold karta
// hai (Composition over Inheritance). Is tarah se actual transaction processing ki 
// low-level logic (Strategy) swappable ho jaati hai.
// ----------------------------------------------------------------------------

// Banking System interface (Abstract Strategy)
class BankingSystem {
public:
  // processPayment: pure virtual function jo concrete banking strategy execute karega
  virtual bool processPayment(double amount) = 0;
  virtual ~BankingSystem() {}
};

// Paytm-specific bank communication backend (Concrete Strategy 1)
class PaytmBankingSystem : public BankingSystem {
public:
  PaytmBankingSystem() {}
  bool processPayment(double amount) override {
    // 70% success rate simulate karne ke liye rand() function ka use kiya hai
    int r = rand() % 100;
    return r < 70; // Agar random value < 70 hai to success (70% probability)
  }
};

// Razorpay-specific bank communication backend (Concrete Strategy 2)
class RazorpayBankingSystem : public BankingSystem {
public:
  RazorpayBankingSystem() {}
  bool processPayment(double amount) override {
    cout << "[BankingSystem-Razorpay] Processing payment of " << amount
         << "...\n";
    // 80% success rate simulate karne ke liye check
    int r = rand() % 100;
    return r < 80; // Agar random value < 80 hai to success (80% probability)
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Template Method Pattern]
// ----------------------------------------------------------------------------
// PaymentGateway abstract base class hai jo poor payment pipeline ka structural flow 
// define karti hai. Iska function `processPayment` ek template method hai, jo 
// step-by-step transaction flow execute karta hai:
// validatePayment() -> initiatePayment() -> confirmPayment()
// Subclasses (PaytmGateway, RazorpayGateway) in concrete hooks/methods ko override 
// karti hain, par main payment execution order (skeleton) predefined rehta hai.
// ----------------------------------------------------------------------------

// Abstract base class for Payment Gateway (Template Method Pattern base)
class PaymentGateway {
protected:
  // Strategy composition: actual bank strategy use karne ke liye pointer
  BankingSystem *bankingSystem;

public:
  PaymentGateway() { bankingSystem = nullptr; }
  virtual ~PaymentGateway() { delete bankingSystem; }

  // [TEMPLATE METHOD]
  // Yeh standard payment flow sequence define karta hai. Ise subclasses change nahi
  // kar sakti hain, bas iske andar call hone wale hooks ko implement karti hain.
  virtual bool processPayment(PaymentRequest *request) {
    // Step 1: Validation checks (gateway wise)
    if (!validatePayment(request)) {
      cout << "[PaymentGateway] Validation failed for " << request->sender
           << ".\n";
      return false;
    }
    // Step 2: Payment initiation logic trigger
    if (!initiatePayment(request)) {
      cout << "[PaymentGateway] Initiation failed for " << request->sender
           << ".\n";
      return false;
    }
    // Step 3: Transaction confirmation
    if (!confirmPayment(request)) {
      cout << "[PaymentGateway] Confirmation failed for " << request->sender
           << ".\n";
      return false;
    }
    return true; // Saare steps clear hone par payment success!
  }

  // Template method ke individual steps (Hooks to be implemented by concrete classes)
  virtual bool validatePayment(PaymentRequest *request) = 0;
  virtual bool initiatePayment(PaymentRequest *request) = 0;
  virtual bool confirmPayment(PaymentRequest *request) = 0;
};

// ----------------------------------------------------------------------------
// Concrete Paytm Gateway (Template implementation)
// ----------------------------------------------------------------------------
class PaytmGateway : public PaymentGateway {
public:
  // Constructor: Paytm banking system logic initialize karta hai
  PaytmGateway() { bankingSystem = new PaytmBankingSystem(); }

  // Paytm-specific validation rules
  bool validatePayment(PaymentRequest *request) override {
    cout << "[Paytm] Validating payment for " << request->sender << ".\n";

    // Paytm rule: Currency INR honi chahiye aur amount positive
    if (request->amount <= 0 || request->currency != "INR") {
      return false;
    }
    return true;
  }

  // Paytm-specific payment initiation (banking system method call)
  bool initiatePayment(PaymentRequest *request) override {
    cout << "[Paytm] Initiating payment of " << request->amount << " "
         << request->currency << " for " << request->sender << ".\n";

    return bankingSystem->processPayment(request->amount);
  }

  // Paytm-specific confirmation simulation (Hum isko fail proof return true kar rahe hain)
  bool confirmPayment(PaymentRequest *request) override {
    cout << "[Paytm] Confirming payment for " << request->sender << ".\n";
    return true;
  }
};

// ----------------------------------------------------------------------------
// Concrete Razorpay Gateway (Template implementation)
// ----------------------------------------------------------------------------
class RazorpayGateway : public PaymentGateway {
public:
  // Constructor: Razorpay banking system logic initialize karta hai
  RazorpayGateway() { bankingSystem = new RazorpayBankingSystem(); }

  // Razorpay-specific validation rules
  bool validatePayment(PaymentRequest *request) override {
    cout << "[Razorpay] Validating payment for " << request->sender << ".\n";

    // Razorpay rule: Sirf amount > 0 check karta hai (multi-currency allowed)
    if (request->amount <= 0) {
      return false;
    }
    return true;
  }

  // Razorpay-specific payment initiation
  bool initiatePayment(PaymentRequest *request) override {
    cout << "[Razorpay] Initiating payment of " << request->amount << " "
         << request->currency << " for " << request->sender << ".\n";

    return bankingSystem->processPayment(request->amount);
  }

  // Razorpay-specific confirmation simulation
  bool confirmPayment(PaymentRequest *request) override {
    cout << "[Razorpay] Confirming payment for " << request->sender << ".\n";
    return true;
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Proxy Pattern]
// ----------------------------------------------------------------------------
// PaymentGatewayProxy real PaymentGateway ko wrap karta hai aur dynamic control
// enforce karta hai (yahan Retry Logic implement ho raha hai). Client jab processPayment
// call karega, to proxy execution handle karegi aur agar failure hota hai to 
// pre-configured loops me retry optimize karegi. 
// ----------------------------------------------------------------------------
class PaymentGatewayProxy : public PaymentGateway {
private:
  PaymentGateway *realGateway; // Wrapped Real Payment Gateway object
  int retries;                 // Maximum retry limit

public:
  // Constructor
  PaymentGatewayProxy(PaymentGateway *gateway, int maxRetries) {
    realGateway = gateway;
    retries = maxRetries;
  }
  // Destructor: Wrapper clear hone par original gateway memory release karna
  ~PaymentGatewayProxy() { delete realGateway; }

  // Proxy wrapper logic: adds retry feature transparency
  bool processPayment(PaymentRequest *request) override {
    bool result = false;
    // Retry loop trigger
    for (int attempt = 0; attempt < retries; attempt++) {
      if (attempt > 0) {
        cout << "[Proxy] Retrying payment (attempt " << (attempt + 1)
             << ") for " << request->sender << ".\n";
      }
      // Real gateway request processing trigger
      result = realGateway->processPayment(request);
      if (result)
        break; // Agar transaction successful hai, to maze se exit karo!
    }
    // Agar maximum retries perform karne ke baad bhi false mila
    if (!result) {
      cout << "[Proxy] Payment failed after " << (retries) << " attempts for "
           << request->sender << ".\n";
    }
    return result;
  }

  // Baki wrapper methods: directly forwards queries/actions to real gateway
  bool validatePayment(PaymentRequest *request) override {
    return realGateway->validatePayment(request);
  }
  bool initiatePayment(PaymentRequest *request) override {
    return realGateway->initiatePayment(request);
  }
  bool confirmPayment(PaymentRequest *request) override {
    return realGateway->confirmPayment(request);
  }
};

// ----------------------------------------------------------------------------
// [DESIGN PATTERN: Singleton Pattern & Factory Pattern]
// ----------------------------------------------------------------------------
// GatewayFactory ek design parameter based Simple Factory aur ek Singleton class hai.
// getInstance() se iska single instance globally use ho sakta hai.
// getGateway method client requirement ke hisab se product generate karta hai and isko
// automatic retry support features ke sath wrap karke proxy form me process karta hai.
// ----------------------------------------------------------------------------
enum class GatewayType { PAYTM, RAZORPAY };

class GatewayFactory {
private:
  static GatewayFactory instance; // Private static singleton instance

  // Private constructor and delete copy/assignment checks
  GatewayFactory() {}
  GatewayFactory(const GatewayFactory &) = delete;
  GatewayFactory &operator=(const GatewayFactory &) = delete;

public:
  // Global access function
  static GatewayFactory &getInstance() { return instance; }

  // Simple Factory method to resolve & wrap gateway proxy
  PaymentGateway *getGateway(GatewayType type) {
    if (type == GatewayType::PAYTM) {
      PaymentGateway *paymentGateway = new PaytmGateway();
      // Paytm ke fail rates ko handle karne ke liye 3 retries setup
      return new PaymentGatewayProxy(paymentGateway, 3);
    } else {
      PaymentGateway *paymentGateway = new RazorpayGateway();
      // Razorpay ke fail rates ko handle karne ke liye 5 retries setup
      return new PaymentGatewayProxy(paymentGateway, 5);
    }
  }
};

// Define static member instance
GatewayFactory GatewayFactory::instance;

// ----------------------------
// [DESIGN PATTERN: Singleton Pattern]
// ----------------------------
// PaymentService application ke main backend payment transaction logic processing ko
// configure & wrap karta hai. Yeh design pattern client code ko single unified gateway
// processor provide karne me help karta hai.
// ----------------------------
class PaymentService {
private:
  static PaymentService instance; // Static instance holder
  PaymentGateway *gateway;         // Configured gateway

  PaymentService() { gateway = nullptr; }
  ~PaymentService() { delete gateway; }

  // Restricting constructor cloning to preserve Singleton structure
  PaymentService(const PaymentService &) = delete;
  PaymentService &operator=(const PaymentService &) = delete;

public:
  static PaymentService &getInstance() { return instance; }

  // Set gateway dynamically at runtime
  void setGateway(PaymentGateway *g) {
    if (gateway)
      delete gateway; // Pehle allocate hue gateway memory ko release karna
    gateway = g;
  }

  // Execution flow call delegator
  bool processPayment(PaymentRequest *request) {
    if (!gateway) {
      cout << "[PaymentService] No payment gateway selected.\n";
      return false;
    }
    return gateway->processPayment(request);
  }
};

// Define static instance
PaymentService PaymentService::instance;

// ----------------------------
// [DESIGN PATTERN: Singleton Pattern]
// ----------------------------
// PaymentController Client interactions and flow controller unit ki tarah act karta hai.
// Yeh service config and factory objects initialization ko smooth channelize karta hai.
// ----------------------------
class PaymentController {
private:
  static PaymentController instance; // Single global controller

  PaymentController() {}
  PaymentController(const PaymentController &) = delete;
  PaymentController &operator=(const PaymentController &) = delete;

public:
  static PaymentController &getInstance() { return instance; }

  // Orchestrate the payment transaction
  bool handlePayment(GatewayType type, PaymentRequest *req) {
    // 1. Factory se proxy-wrapped gateway construct karna
    PaymentGateway *paymentGateway =
        GatewayFactory::getInstance().getGateway(type);

    // 2. Gateway setup to execution pipeline
    PaymentService::getInstance().setGateway(paymentGateway);

    // 3. Process execution command trigger
    return PaymentService::getInstance().processPayment(req);
  }
};

// Define static instance
PaymentController PaymentController::instance;

// ----------------------------
// Main: Client execution entry
// ----------------------------
int main() {
  // Random time base seed system status resolution simulation ke liye
  srand(static_cast<unsigned>(time(nullptr)));

  // Test Case 1: Paytm simulation check
  PaymentRequest *req1 = new PaymentRequest("Hardik", "Shubham", 1000.0, "INR");

  cout << "Processing via Paytm\n";
  cout << "------------------------------\n";
  bool res1 =
      PaymentController::getInstance().handlePayment(GatewayType::PAYTM, req1);
  cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n\n";

  // Test Case 2: Razorpay simulation check
  PaymentRequest *req2 = new PaymentRequest("Shubham", "Aditya", 500.0, "USD");

  cout << "Processing via Razorpay\n";
  cout << "------------------------------\n";
  bool res2 = PaymentController::getInstance().handlePayment(
      GatewayType::RAZORPAY, req2);
  cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
  cout << "------------------------------\n";

  // Memory leaks check clean-ups
  delete req1;
  delete req2;

  return 0;
}
