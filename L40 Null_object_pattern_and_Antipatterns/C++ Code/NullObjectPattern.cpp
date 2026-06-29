// NullObjectPattern.cpp
// Solution: instead of nullptr, inject a "do nothing" object that implements
// the same interface. The client code calls methods unconditionally — the
// behavior is decided by *which object* is injected, not by `if` checks.
//
// Build & run:
//   g++ -std=c++17 NullObjectPattern.cpp -o null_object && ./null_object

#include <iostream>
#include <string>
using namespace std;

// --- Logger: real + null object ----------------------------------------------

class ILogger {
public:
    virtual void log(const string& msg) = 0;
    virtual ~ILogger() {}
};

class ConsoleLogger : public ILogger {
public:
    void log(const string& msg) override {
        cout << "[LOG] " << msg << "\n";
    }
};

// Null Object: same interface, intentionally does nothing.
class NoOpLogger : public ILogger {
public:
    void log(const string& /*msg*/) override { /* intentional no-op */ }
};

// --- Encryption: real + null object ------------------------------------------

class EncryptionService {
public:
    virtual string encrypt(const string& plainText) = 0;
    virtual ~EncryptionService() {}
};

class AESEncryption : public EncryptionService {
public:
    string encrypt(const string& plainText) override {
        return "<enc:" + plainText + ">"; // pretend cipher
    }
};

// Null Object: identity transform — "no encryption" without a special case.
// (Mirrors WhatsApp_LLD/services/EncryptionService.h -> NoOpEncryptionService)
class NoOpEncryptionService : public EncryptionService {
public:
    string encrypt(const string& plainText) override {
        return plainText; // identity
    }
};

// --- Client: branch-free ------------------------------------------------------

class MessageService {
    ILogger& logger;            // never null
    EncryptionService& crypto;  // never null
public:
    MessageService(ILogger& logger, EncryptionService& crypto)
        : logger(logger), crypto(crypto) {}

    void send(const string& to, const string& text) {
        // No null checks anywhere — always safe to call.
        logger.log("Preparing message to " + to);
        string payload = crypto.encrypt(text);
        cout << "Sending to " << to << ": " << payload << "\n";
        logger.log("Message sent to " + to);
    }
};

int main() {
    cout << "=== Null Object pattern (no-op implementations) ===\n\n";

    ConsoleLogger consoleLogger;
    AESEncryption aes;
    NoOpLogger    noLog;
    NoOpEncryptionService noCrypto;

    cout << "-- Full feature service (logger + encryption) --\n";
    MessageService full(consoleLogger, aes);
    full.send("Alice", "hello");

    cout << "\n-- Stripped service (null objects injected) --\n";
    // Same client code, no guards: logging silently off, encryption = identity.
    MessageService bare(noLog, noCrypto);
    bare.send("Bob", "hi");

    cout << "\nBenefit: client has zero `if (x != nullptr)` checks.\n"
            "Turning a feature off = injecting a No-Op object, not a null.\n";
    return 0;
}
