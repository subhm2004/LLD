// ============================================================================
//  NullObjectPattern.cpp  —  NULL OBJECT DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Null Object = "nullptr dene ki jagah ek 'KUCH-NA-KARNE-WALA' object do
//                 jo SAME interface implement karta hai — client bina kisi
//                 null-check ke methods call karta rahe, kabhi crash nahi."
//
//  Asli duniya ka example: TV ka MUTE button
//    - Mute = speaker HATANA nahi hai (nullptr) — balki ek "silent speaker"
//      lagana hai jo sab commands leta hai par awaaz nahi karta.
//    - TV ka baaki code same chalta hai — use pata bhi nahi mute hai ya nahi.
//  Waise hi: logging OFF karni hai? Logger HATAO mat (nullptr) — NoOpLogger
//  lagao jo log() calls chupchap kha jaata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  NULL OBJECT KYUN? — WithoutNullObject.cpp ka dard yaad karo:           │
//  │                                                                          │
//  │    NULLPTR APPROACH (pehle):        NULL OBJECT (ab):                   │
//  │    ─────────────────────────        ──────────────────                  │
//  │    if (logger != nullptr)           logger.log("...");                  │
//  │        logger->log("...");          // bas. seedha call. hamesha safe.  │
//  │                                                                          │
//  │    if (crypto != nullptr)           payload = crypto.encrypt(text);     │
//  │        payload = crypto->...;       // "encryption off" = NoOp inject   │
//  │    else payload = text;             //  hua tha — text as-is wapas      │
//  │                                                                          │
//  │  Behavior ab `if` checks se decide NAHI hota — is baat se hota hai ki   │
//  │  KAUNSA OBJECT INJECT kiya. Ye polymorphism ka sabse saaf use hai:      │
//  │  "conditional logic ko object selection se REPLACE kar do."             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES is file me:
//    1. AbstractObject  -> ILogger, EncryptionService : common contract
//    2. RealObject      -> ConsoleLogger              : asli kaam karta hai
//                       -> AESEncryption              : asli encryption
//    3. NullObject      -> NoOpLogger                 : log() = khali (no-op)
//                       -> NoOpEncryptionService      : encrypt() = text as-is
//    4. Client          -> MessageService             : bina kisi null-check ke
//                                                       seedha methods call karta hai
//
//  ============================================================================
//   NULLPTR vs NULL OBJECT — comparison table ek nazar me
//  ----------------------------------------------------------------------------
//   Sawaal                    | nullptr approach       | Null Object approach
//   --------------------------+------------------------+------------------------
//   "Feature OFF" kaise?      | nullptr pass karo      | No-Op object pass karo
//   Client me null-checks?    | HAR call site pe        | ZERO
//   Guard bhool gaye to?      | CRASH (segfault)        | Crash possible hi nahi
//   Business logic readable?  | if-checks me doobi      | Ekdum saaf
//   Naya optional dependency? | Har jagah naye guards   | Bas ek No-Op class
//   Member kaise rakha?       | ILogger* (null ho       | ILogger& (reference —
//                             |  sakta hai)             |  KABHI null nahi!)
//
//   SABSE BADA RULE — NULL OBJECT ka behavior "NEUTRAL" hona chahiye:
//   No-Op ka matlab hai "aisa karo jaise feature hai hi nahi":
//     - NoOpLogger::log()      -> kuch mat karo (khali body)
//     - NoOpEncryption::encrypt-> text JAISA HAI waisa lauta do (identity)
//   GALAT null object: encrypt() me "" (khali string) lauta dena — ye
//   neutral nahi, DATA LOSS hai! Null object ko system ka flow todna
//   nahi chahiye — bas us feature ko "transparent" banana chahiye.
//
//   L8 CONNECTION: Strategy pattern wale Robot me NoWalk/NoTalk/NoFly
//   strategies bhi yahi Null Object flavour thi — "nahi kar sakta" bhi
//   ek valid behavior class hai, nullptr nahi!
//
//   KAB MAT USE KARO: jab "dependency ka missing hona" ek ERROR hai
//   jo client ko PATA chalna chahiye. Null Object galtiyon ko chupa
//   sakta hai — payment gateway missing ho to silently "no-op" karna
//   disaster hai, wahan exception/validation hi sahi hai!
//
//  Build & run:
//    g++ -std=c++17 NullObjectPattern.cpp -o null_object && ./null_object
// ============================================================================

#include <iostream>
#include <string>
using namespace std;

// ============================================================================
//  SECTION 1: LOGGER — interface + real + NULL OBJECT
// ============================================================================

// --- AbstractObject: logger ka contract ---
// Ye ek pure abstract class hai (interface jaisa) - iska matlab hai koi bhi
// class jo ILogger ko inherit karegi, usko log() function mandatory
// implement karna hi padega. "= 0" likhne se ye function pure virtual
// ban jata hai, matlab is class ka khud ka object bhi nahi ban sakta.
class ILogger
{
public:
    // Pure virtual function - har child class ko apna khud ka
    // implementation dena hi padega, warna compile error aayega
    virtual void log(const string &msg) = 0; // har logger ko ye dena hi hoga

    // Virtual destructor rakhna zaroori hai jab bhi hum base class
    // pointer/reference se delete kar rahe hon - warna sirf base class
    // ka destructor chalega, child class ka nahi (memory leak ho sakta hai)
    virtual ~ILogger() {} // base ptr se delete ke liye virtual
};

// --- RealObject: asli logger — console pe likhta hai ---
// Ye ILogger ka ek concrete (real) implementation hai jo actually
// kaam karta hai - console pe message print karta hai
class ConsoleLogger : public ILogger
{
public:
    void log(const string &msg) override
    {
        // override keyword se compiler check karta hai ki ye function
        // sach me parent class ke virtual function ko replace kar raha hai
        cout << "[LOG] " << msg << "\n";
    }
};

// --- NULL OBJECT: same interface, JAAN-BUJH KE kuch nahi karta ---
// Ye class hi pattern ka hero hai! log() aata hai, chupchap kha jaata
// hai — na error, na output, na crash. "Logging OFF" ab ek OBJECT hai,
// nullptr nahi.
// (Parameter ka naam comment me likha hai — /*msg*/ — taaki compiler
//  "unused parameter" warning na de. Chhota sa pro tip!)
class NoOpLogger : public ILogger
{
public:
    // Function ka body jaan bujh kar khali rakha hai - ye "intentional
    // no-op" hai, bug nahi. Comment isliye likha hai taaki koi aur
    // developer future me confuse na ho ki ye khali kyu hai
    void log(const string & /*msg*/) override { /* intentional no-op */ }
};

// ============================================================================
//  SECTION 2: ENCRYPTION — interface + real + NULL OBJECT
// ============================================================================

// --- AbstractObject: encryption ka contract ---
// Isi tarah encryption ke liye bhi ek common interface bana rahe hain
// taaki real encryption aur no-op encryption dono isi contract ko follow karein
class EncryptionService
{
public:
    // Ye function ek plain text lega aur encrypted (ya as-is) text
    // return karega - actual implementation child classes decide karengi
    virtual string encrypt(const string &plainText) = 0;
    virtual ~EncryptionService() {}
};

// --- RealObject: asli encryption (pretend AES) ---
// Real duniya me yahan actual AES encryption algorithm ka code hota,
// yahan demo ke liye hum sirf text ko simulate kar rahe hain
class AESEncryption : public EncryptionService
{
public:
    string encrypt(const string &plainText) override
    {
        // Demo cipher - real AES nahi hai, bas pattern samjhane ke liye
        // text ko "<enc: ... >" format me wrap kar rahe hain
        return "<enc:" + plainText + ">"; // demo cipher
    }
};

// --- NULL OBJECT: "encryption OFF" = IDENTITY transform ---
// Dhyan do — ye khali string return NAHI karta (wo data loss hota!),
// text ko JAISA HAI waisa lauta deta hai. Yahi "neutral behavior" ka
// perfect example hai: pipeline chalti rahe, bas encryption skip ho jaaye.
// (Is repo ke WhatsApp_LLD me bhi yahi cheez hai -> NoOpEncryptionService)
class NoOpEncryptionService : public EncryptionService
{
public:
    string encrypt(const string &plainText) override
    {
        // Identity function - jo input aaya wahi output kar diya, koi
        // transformation nahi. Ye ek "neutral" no-op ka sahi example hai
        return plainText; // identity — jo aaya wahi wapas
    }
};

// ============================================================================
//  SECTION 3: CLIENT — MessageService (guards se AZAAD!)
// ----------------------------------------------------------------------------
//  WithoutNullObject.cpp se compare karo — wahan send() me 3 guards the,
//  yahan EK BHI NAHI. Business logic ab saaf-saaf dikh rahi hai.
//
//  DESIGN DETAIL: members POINTER nahi, REFERENCE hain (ILogger&).
//  Reference C++ me kabhi null ho hi nahi sakta — matlab "null aa
//  sakta hai kya?" wala sawaal COMPILE-TIME pe hi khatam! Type system
//  khud guarantee de raha hai. Ye Null Object + references ka combo
//  sabse solid hai.
// ============================================================================
class MessageService
{
    // References use kiye hain, pointers nahi - isliye ye guarantee
    // milti hai ki logger aur crypto kabhi bhi null/invalid nahi honge.
    // Object banate waqt hi ye references initialize karni padengi
    // (constructor me), aur baad me change nahi ho sakti (rebind nahi hoti)
    ILogger &logger;           // reference = KABHI null nahi
    EncryptionService &crypto; // reference = KABHI null nahi

public:
    // Constructor me dono dependencies inject ki jaa rahi hain
    // (Dependency Injection pattern) - ye decide karta hai ki
    // real object use hoga ya null object, ye MessageService ko
    // pata hi nahi chalega, use sirf interface se matlab hai
    MessageService(ILogger &logger, EncryptionService &crypto)
        : logger(logger), crypto(crypto) {}

    void send(const string &to, const string &text)
    {
        // ZERO null-checks! Har call bina soche-samjhe safe hai.
        // Logging ON hai ya OFF? — MessageService ko MATLAB HI NAHI.
        // Wo decision to main() me ho chuka (kaunsa object inject hua).

        // Step 1: message bhejne se pehle log karo
        logger.log("Preparing message to " + to);

        // Step 2: text ko encrypt karo (real ho ya no-op, dono ka
        // interface same hai isliye ye line hamesha safe hai)
        string payload = crypto.encrypt(text);

        // Step 3: actual message "bhejo" (yahan demo ke liye print
        // kar rahe hain, real app me network call hoti)
        cout << "Sending to " << to << ": " << payload << "\n";

        // Step 4: message bhejne ke baad bhi log karo
        logger.log("Message sent to " + to);
    }
};

// ============================================================================
//  SECTION 4: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Wahi do scenarios jo WithoutNullObject.cpp me the — par ab "features
//  OFF" ka matlab nullptr nahi, NULL OBJECTS inject karna hai.
// ============================================================================
int main()
{
    cout << "=== Null Object pattern (no-op implementations) ===\n\n";

    // Saare objects — real bhi, null bhi. Null objects bhi NORMAL
    // objects hi hain, bas unka kaam "kuch na karna" hai. Stack pe
    // banaye hain taaki lifetime automatically manage ho jaaye
    ConsoleLogger consoleLogger;
    AESEncryption aes;
    NoOpLogger noLog;
    NoOpEncryptionService noCrypto;

    // Scenario A: sab features ON — real objects inject kiye
    // MessageService ko real ConsoleLogger aur real AESEncryption di
    cout << "-- Full feature service (logger + encryption) --\n";
    MessageService full(consoleLogger, aes);
    full.send("Alice", "hello");

    // Scenario B: sab features OFF — null objects inject kiye.
    // SAME MessageService code, SAME send() — bas objects alag!
    // Logging chupchap band, encryption = text as-is. Na guard, na crash.
    // Notice karo: MessageService class ka code bilkul bhi nahi badla,
    // sirf constructor me alag objects pass kar diye - ye hi Null
    // Object Pattern ki asli power hai
    cout << "\n-- Stripped service (null objects injected) --\n";
    MessageService bare(noLog, noCrypto);
    bare.send("Bob", "hi");

    cout << "\nBenefit: client has zero `if (x != nullptr)` checks.\n"
            "Turning a feature off = injecting a No-Op object, not a null.\n";
    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  === Null Object pattern (no-op implementations) ===
//
//  -- Full feature service (logger + encryption) --
//  [LOG] Preparing message to Alice
//  Sending to Alice: <enc:hello>
//  [LOG] Message sent to Alice
//
//  -- Stripped service (null objects injected) --
//  Sending to Bob: hi
//
//  Benefit: client has zero `if (x != nullptr)` checks.
//  Turning a feature off = injecting a No-Op object, not a null.
//
//  OUTPUT WithoutNullObject.cpp jaisa hi hai — PAR ANDAR ka design
//  bilkul alag: wahan 3 guards crash se bacha rahe the, yahan crash
//  ka sawaal hi paida nahi hota. Same result, solid design.
// ============================================================================