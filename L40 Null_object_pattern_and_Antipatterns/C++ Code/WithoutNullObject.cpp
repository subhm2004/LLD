// WithoutNullObject.cpp
// Problem: optional dependencies are modelled as nullptr, so every call site
// must guard with `if (dep != nullptr)`. Forget one guard -> crash.
//
// Build & run:
//   g++ -std=c++17 WithoutNullObject.cpp -o without_null_object && ./without_null_object

#include <iostream>
#include <string>
using namespace std;

// --- Optional collaborators (real implementations) ---------------------------

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

// --- Client riddled with null checks -----------------------------------------

class MessageService {
    ILogger* logger;             // may be nullptr (logging optional)
    EncryptionService* crypto;   // may be nullptr (encryption optional)
public:
    MessageService(ILogger* logger, EncryptionService* crypto)
        : logger(logger), crypto(crypto) {}

    void send(const string& to, const string& text) {
        // Defensive guard #1 — easy to forget
        if (logger != nullptr) logger->log("Preparing message to " + to);

        // Defensive guard #2 — if you forget this one, nullptr deref = crash
        string payload = text;
        if (crypto != nullptr) payload = crypto->encrypt(text);

        cout << "Sending to " << to << ": " << payload << "\n";

        if (logger != nullptr) logger->log("Message sent to " + to);
    }
};

int main() {
    cout << "=== Without Null Object (nullptr + guards) ===\n\n";

    ConsoleLogger consoleLogger;
    AESEncryption aes;

    cout << "-- Full feature service (logger + encryption) --\n";
    MessageService full(&consoleLogger, &aes);
    full.send("Alice", "hello");

    cout << "\n-- Stripped service (no logger, no encryption) --\n";
    // Both optional deps are null -> every call site relies on the guards.
    MessageService bare(nullptr, nullptr);
    bare.send("Bob", "hi");

    cout << "\nProblem: business logic is drowned in `if (x != nullptr)`,\n"
            "and a single missing guard becomes a null-pointer crash.\n";
    return 0;
}
