// ============================================================================
//  WithoutNullObject.cpp  —  NULL OBJECT ke BINA (problem samajhne ke liye)
// ----------------------------------------------------------------------------
//  Ye file jaan-bujh kar GALAT (dard wala) tareeka dikhati hai, taaki
//  NullObjectPattern.cpp ka fix samajh aaye. Pehle problem feel karo!
//
//  SCENARIO: MessageService hai jo message bhejta hai. Uske paas do
//  OPTIONAL dependencies hain:
//    - Logger     (ho sakta hai ho, ho sakta hai na ho)
//    - Encryption (ho sakta hai ho, ho sakta hai na ho)
//  "Optional" ko model kaise kiya? -> nullptr se. AUR YAHI GALTI HAI!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  NULLPTR WALA APPROACH — code ka haal:                                  │
//  │                                                                          │
//  │    void send(...) {                                                     │
//  │        if (logger != nullptr) logger->log("...");     // guard #1       │
//  │        if (crypto != nullptr) payload = crypto->...;  // guard #2       │
//  │        ...                                                              │
//  │        if (logger != nullptr) logger->log("...");     // guard #3       │
//  │    }                                                                     │
//  │                                                                          │
//  │  Har call site pe `if (x != nullptr)` — aur ye to sirf EK method hai!   │
//  │  10 methods × 2 dependencies = 20+ guards poore codebase me bikhre.     │
//  │                                                                          │
//  │  1) EK guard bhoole = nullptr dereference = CRASH (segfault)            │
//  │  2) Business logic if-checks ke jungle me DOOB gayi — padhna mushkil    │
//  │  3) Compiler ye galti NAHI pakadta — crash RUNTIME pe hota hai,         │
//  │     production me, raat 3 baje                                          │
//  │  4) Naya optional dependency aaya -> HAR call site pe naya guard        │
//  │                                                                          │
//  │  FIX (agli file): nullptr ki jagah ek "kuch-na-karne-wala" object       │
//  │  inject karo jo SAME interface implement karta hai -> saare guards      │
//  │  GAYAB, crash ka khatra ZERO. Ye hai NULL OBJECT PATTERN.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Ye "nullptr + har jagah guards" wala style ek ANTI-PATTERN ki taraf le
//  jaata hai — defensive programming ka overdose, jisme asli logic guards
//  ke neeche dab jaati hai.
//
//  IMPORTANT: Ye file NullObjectPattern.cpp ke saath compare karke padho.
//  Dono me SAME classes (ILogger, ConsoleLogger, EncryptionService,
//  AESEncryption) hain — sirf MessageService ka andar ka design alag hai.
//  Yahi difference pura pattern samjhata hai.
//
//  Build & run:
//    g++ -std=c++17 WithoutNullObject.cpp -o without_null_object && ./without_null_object
// ============================================================================

#include <iostream>
#include <string>
using namespace std;

// ============================================================================
//  SECTION 1: OPTIONAL DEPENDENCIES ke interfaces + real implementations
// ----------------------------------------------------------------------------
//  Interfaces bilkul theek hain — problem inme nahi hai. Problem hai
//  MessageService me jahan "feature OFF" ko nullptr se represent kiya.
// ============================================================================

// --- Logger ka contract ---
// Ye ek abstract base class hai jo sirf ek "blueprint" define karti hai -
// khud iska koi object nahi ban sakta, kyunki log() pure virtual hai (=0)
class ILogger
{
public:
    virtual void log(const string &msg) = 0; // pure virtual — har logger apna log()
                                             // implementation dega, warna
                                             // compile hi nahi hoga

    virtual ~ILogger() {} // virtual dtor — base ptr se delete safe
                          // (agar ye virtual na hota to
                          // ILogger* se delete karne pe
                          // sirf ILogger ka destructor
                          // chalta, child class ka nahi)
};

// --- Real logger: console pe likhta hai ---
// ILogger ka ek concrete implementation - actual me kaam karta hai
class ConsoleLogger : public ILogger
{
public:
    void log(const string &msg) override
    {
        // Simple console output - real project me ye file/database
        // me bhi likh sakta hai
        cout << "[LOG] " << msg << "\n";
    }
};

// --- Encryption ka contract ---
// Isi tarah encryption ke liye bhi ek interface, taaki alag-alag
// encryption implementations (AES, RSA, etc.) same contract follow karein
class EncryptionService
{
public:
    virtual string encrypt(const string &plainText) = 0;
    virtual ~EncryptionService() {}
};

// --- Real encryption: text ko cipher me badalta hai (pretend) ---
class AESEncryption : public EncryptionService
{
public:
    string encrypt(const string &plainText) override
    {
        // Ye asli AES algorithm nahi hai, sirf demo ke liye text ko
        // wrap kar rahe hain taaki dikhe ki "encrypt" hua
        return "<enc:" + plainText + ">"; // asli AES nahi — demo cipher
    }
};

// ============================================================================
//  SECTION 2: CLIENT — MessageService (guards ke jungle me phasa hua)
// ----------------------------------------------------------------------------
//  Dhyan se dekho: send() ki 6 lines me se 3 lines sirf null-checks hain!
//  Business logic (message bhejna) in guards ke neeche dabi hui hai.
// ============================================================================
class MessageService
{
    // Yahan RAW POINTERS use kiye hain, references nahi - isliye
    // ye dono members ho sakta hai valid object point karein, ya
    // phir nullptr ho jaayein. Compiler is baat ko force nahi karta
    // ki inhe hamesha valid hona chahiye - yahi is design ki root
    // problem hai
    ILogger *logger;           // nullptr ho sakta hai (logging optional)
    EncryptionService *crypto; // nullptr ho sakta hai (encryption optional)

public:
    // Constructor me dono pointers seedhe assign kar diye - koi
    // validation nahi ki ye null hain ya nahi, dono valid values
    // accept karta hai
    MessageService(ILogger *logger, EncryptionService *crypto)
        : logger(logger), crypto(crypto) {}

    void send(const string &to, const string &text)
    {
        // GUARD #1 — bhoolna easy hai, koi warning nahi aati
        // Har baar logger use karne se pehle manually check karna
        // pad raha hai ki wo valid hai ya nahi
        if (logger != nullptr)
            logger->log("Preparing message to " + to);

        // GUARD #2 — YE wala bhoole to nullptr->encrypt() = CRASH
        // Socho: naya developer aaya, usne seedha crypto->encrypt(text)
        // likh diya — compile ho gaya, test me logger/crypto diye the to
        // pass bhi ho gaya... production me bare service aayi aur BOOM!
        // Default payload text hi rakha hai (agar crypto null hua to
        // encryption skip karke plain text bhej denge)
        string payload = text;
        if (crypto != nullptr)
            payload = crypto->encrypt(text);

        // Actual message "bhejna" - ye line hi asli business logic
        // hai, lekin upar-neeche guards ke beech dab gayi hai
        cout << "Sending to " << to << ": " << payload << "\n";

        // GUARD #3 — wahi logger wala check PHIR se. Har logging line
        // ke saath ye chipkega. 50 log statements = 50 guards!
        // Yahan bhi same check repeat ho raha hai jo pehle guard me
        // tha - ye DRY violation bhi hai, saath me risk bhi
        if (logger != nullptr)
            logger->log("Message sent to " + to);
    }
};

// ============================================================================
//  SECTION 3: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Do scenarios: full-feature service (sab diya) aur bare service
//  (dono nullptr — ab bas guards ki daya pe zinda hain!)
// ============================================================================
int main()
{
    cout << "=== Without Null Object (nullptr + guards) ===\n\n";

    // Real implementations stack pe bana rahe hain
    ConsoleLogger consoleLogger;
    AESEncryption aes;

    // Scenario A: sab features ON — logger aur encryption dono mile
    // Yahan address-of operator (&) use kar rahe hain kyunki
    // MessageService pointers expect karta hai
    cout << "-- Full feature service (logger + encryption) --\n";
    MessageService full(&consoleLogger, &aes);
    full.send("Alice", "hello");

    // Scenario B: sab features OFF — dono jagah nullptr thok diya
    // Ab send() ke andar ke guards hi crash se bacha rahe hain.
    // EK bhi guard hata do -> segfault. Itna nazuk design!
    cout << "\n-- Stripped service (no logger, no encryption) --\n";
    MessageService bare(nullptr, nullptr);
    bare.send("Bob", "hi");
    // Ye call kaam kar gayi kyunki send() ke andar guards the -
    // lekin agar kal koi developer send() edit karte waqt ek bhi
    // guard bhool gaya, to yehi line crash kar degi

    cout << "\nProblem: business logic is drowned in `if (x != nullptr)`,\n"
            "and a single missing guard becomes a null-pointer crash.\n";
    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  === Without Null Object (nullptr + guards) ===
//
//  -- Full feature service (logger + encryption) --
//  [LOG] Preparing message to Alice
//  Sending to Alice: <enc:hello>
//  [LOG] Message sent to Alice
//
//  -- Stripped service (no logger, no encryption) --
//  Sending to Bob: hi
//
//  Problem: business logic is drowned in `if (x != nullptr)`,
//  and a single missing guard becomes a null-pointer crash.
//
//  Kaam to kar raha hai... par sirf isliye kyunki HAR guard sahi jagah
//  laga hai. Ye "chalta hai" nahi, "abhi tak gira nahi" wala code hai!
//  Fix dekho -> NullObjectPattern.cpp
// ============================================================================