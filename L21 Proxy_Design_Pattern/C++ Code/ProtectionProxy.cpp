// ============================================================================
//  ProtectionProxy.cpp  —  PROXY PATTERN (Structural) : PROTECTION proxy variant
// ----------------------------------------------------------------------------
//  Protection Proxy = "real object tak request TABHI jaane do jab AUTHORIZATION
//                      pass ho — proxy ek SECURITY GUARD 💂 ki tarah kaam kare."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  PROTECTION PROXY ka kaam — ACCESS CONTROL:                             │
//  │                                                                          │
//  │    Yahan: PDF unlock karna sirf PREMIUM users ke liye allowed hai.      │
//  │                                                                          │
//  │    DocumentProxy pehle CHECK karta hai — user premium hai?              │
//  │      - Nahi (Rohan)  -> "Access denied" — real reader tak jaane hi nahi │
//  │                          diya (request wahin block!)                    │
//  │      - Haan (Rashmi) -> RealDocumentReader ko forward — PDF unlock       │
//  │                                                                          │
//  │    Security logic EK jagah (proxy me) — har client me if(premium)       │
//  │    check bikhra nahi. Naya rule (jaise "sirf office hours") add karna   │
//  │    ho to sirf proxy badlega, RealReader untouched. ✅                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES:
//    1. Subject (interface) -> IDocumentReader     : common contract (unlockPDF)
//    2. RealSubject         -> RealDocumentReader  : asli PDF unlock karta hai
//    3. Proxy               -> DocumentProxy       : premium check karke forward
//    4. Client              -> main()              : IDocumentReader* use karta
//
//  📌 KEY IDEA: Proxy aur RealSubject dono SAME interface (IDocumentReader)
//  implement karte hain — isliye client ko farq hi nahi padta ki beech me
//  security guard khada hai. Bas kaam authorized hone pe hota hai.
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

// Interface for Document Reader
class IDocumentReader {
public:
    virtual void unlockPDF(string filePath, string password) = 0;
    virtual ~IDocumentReader() = default;
};

// Concrete Class: Reads the PDF (simulated)
class RealDocumentReader : public IDocumentReader {
public:
    void unlockPDF(string filePath, string password) override {
        cout << "[RealDocumentReader] Unlocking PDF at: " << filePath << "\n";
        cout << "[RealDocumentReader] PDF unlocked successfully with password: " << password << "\n";
        cout << "[RealDocumentReader] Displaying PDF content...\n";
    }
};

// User class with membership status
class User {
public:
    string name;
    bool premium_Membership;
    
    User(string name, bool isPremium) {
        this->name = name;
        this->premium_Membership = isPremium;
    }
};

// Proxy Class: Controls access to RealDocumentReader
class DocumentProxy : public IDocumentReader {
    RealDocumentReader* realReader;
    User* user;
    
public:
    DocumentProxy(User* user) {
        realReader = new RealDocumentReader();
        this->user = user;
    }

    void unlockPDF(string filePath, string password) override {
        // >>> ACCESS CONTROL CHECK (proxy ka asli kaam) <<<
        // Non-premium user? -> request YAHIN block, real reader tak jaane
        // hi nahi diya. Ye "guard at the gate" hai.
        if (!user->premium_Membership) {
            cout << "[DocumentProxy] Access denied. Only premium members can unlock PDFs.\n";
            return;  // real object ko call kiye bina wapas
        }

        // Authorization pass -> ab asli kaam RealDocumentReader ko forward
        realReader->unlockPDF(filePath, password);
    }

    ~DocumentProxy() {
        delete realReader;
    }
};

// Client code
int main() {

    User* user1 = new User("Rohan", false);  // Non Premium User
    User* user2 = new User("Rashmi", true);  // premium user

    cout << "== Rohan (Non-Premium) tries to unlock PDF ==\n";
    IDocumentReader* docReader = new DocumentProxy(user1);
    docReader->unlockPDF("protected_document.pdf", "secret123");
    delete docReader;

    cout << "\n== Rashmi (Premium) unlocks PDF ==\n";
    docReader = new DocumentProxy(user2);
    docReader->unlockPDF("protected_document.pdf", "secret123");
    delete docReader;

    // NOTE: user1/user2 delete nahi hue (chhota demo) — production me cleanup!
    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  == Rohan (Non-Premium) tries to unlock PDF ==
//  [DocumentProxy] Access denied. Only premium members can unlock PDFs.
//                                    ^^ real reader tak pahuncha hi nahi!
//  == Rashmi (Premium) unlocks PDF ==
//  [RealDocumentReader] Unlocking PDF at: protected_document.pdf
//  [RealDocumentReader] PDF unlocked successfully with password: secret123
//  [RealDocumentReader] Displaying PDF content...
//
//  Rohan block, Rashmi allowed — SAME interface, alag access. Yahi
//  protection proxy ka pura point! ✅
// ============================================================================
