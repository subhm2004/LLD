// ============================================================================
//  MementoPattern.cpp  —  MEMENTO DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Memento = "object ke state ka SNAPSHOT lo, sambhal ke rakho, aur zaroorat
//             pade to WAPAS restore kar do — object ke PRIVATE internals ko
//             bahar expose kiye BINA."
//
//  Asli duniya ka example: database transaction ka safar:
//    - BEGIN    -> current state ka backup (snapshot) ban gaya
//    - changes  -> insert/update/delete — sab kachha (abhi undo ho sakta hai)
//    - COMMIT   -> sab pakka, backup phenk do
//    - ROLLBACK -> error aayi! backup se PURANA state wapas
//  Bilkul game ke save-point jaisa 🎮 — boss fight se pehle SAVE karo,
//  mar gaye to LOAD karo — wapas wahi ke wahi, jaise kuch hua hi nahi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  MEMENTO KYUN? — bina iske undo/rollback ka haal:                        │
//  │                                                                          │
//  │    // caller ko object ke PRIVATE members chahiye hote:                  │
//  │    auto savedRecords = db.records;     // ❌ private data bahar aa gaya! │
//  │    ...changes...                                                         │
//  │    db.records = savedRecords;          // ❌ bahar se internals me haath!│
//  │                                                                          │
//  │  1) Encapsulation TOOTI — records ko public karna padega                 │
//  │  2) Har caller ko Database ka internal structure pata hona chahiye       │
//  │  3) Kal `records` ki jagah do alag fields ho gaye -> SAB callers tootenge│
//  │  4) Save/restore ka logic poore codebase me bikhar jaayega               │
//  │                                                                          │
//  │  MEMENTO se: Database KHUD apna snapshot banata hai (createMemento) aur  │
//  │  KHUD hi restore karta hai (restoreFromMemento). Bahar walon ko bas ek   │
//  │  SEALED ENVELOPE 💌 milta hai — pakad ke rakho, par khol nahi sakte.     │
//  │  records private tha, private hi rahega. Ye ENCAPSULATION ki jeet hai.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//    1. Originator   -> Database           : jiska state save/restore hota hai;
//                                            memento SIRF yahi banata/padhta hai
//    2. Memento      -> DatabaseMemento    : state ka snapshot — sealed envelope,
//                                            bas data pakadta hai, koi logic nahi
//    3. Caretaker    -> TransactionManager : memento STORE karta hai, kab save/
//                                            restore ho ye decide karta hai —
//                                            PAR envelope KHOLTA NAHI!
//
//  ============================================================================
//   TRANSACTION FLOW TABLE — poora lifecycle ek nazar me
//   (row = action, columns = backup aur database pe kya asar hua)
//  ----------------------------------------------------------------------------
//   Action            | Backup ka kya hua          | Database ka kya hua
//   ------------------+----------------------------+---------------------------
//   beginTransaction  | naya snapshot bana ✅      | koi change nahi
//                     | (purana pada tha to pehle  |
//                     |  delete — leak nahi!)      |
//   insert/update/    | kuch nahi                  | data badla — par KACHHA
//   remove            |                            | (rollback se ud sakta hai)
//   commitTransaction | backup DELETE (ab zaroorat | changes PAKKE ho gaye ✅
//                     |  nahi — sab pakka hai)     |
//   rollbackTransaction| backup se restore karwaya,| WAPAS snapshot wala state ✅
//                     |  phir delete               | (baad ke sab changes gayab —
//                     |                            |  jaise hue hi nahi the!)
//   rollback (bina    | kuch nahi (nullptr check   | koi change nahi — polite
//    begin kiye)      |  crash se bachata hai)     | "No backup available!" msg
//
//   📌 SABSE BADA RULE — CARETAKER ENVELOPE KABHI NAHI KHOLTA:
//   TransactionManager memento ko sirf PAKADTA hai — na getState() call
//   karta hai, na andar ka data padhta hai. Restore HAMESHA Database
//   (Originator) khud karta hai. Agar Caretaker data padhne lage to wahi
//   encapsulation toot gayi jise bachane ke liye pattern laaye the!
//   (Strict version: getState() ko private karke `friend class Database;`
//    likho — tab Caretaker CHAH KAR BHI envelope nahi khol sakta.
//    Interview me ye improvement zaroor bolna — full marks point hai.)
// ============================================================================
#include <iostream>
#include <string>
#include <map>
#include <memory>

using namespace std;

// ============================================================================
//  SECTION 1: MEMENTO — DatabaseMemento (snapshot ka dibba 📦)
// ----------------------------------------------------------------------------
//  Iska EK hi kaam: database ke state ki COPY apne andar sambhal ke rakhna.
//  Na koi business logic, na koi operation — pure data holder.
//
//  NOTE: Yahan getState() public hai (demo simple rakhne ke liye) —
//  STRICT version me ise private karke `friend class Database;` likhte
//  hain, taaki SIRF Database (Originator) hi andar dekh sake aur
//  Caretaker chahkar bhi envelope na khol paye. Interview me ye
//  improvement zaroor bolna!
// ============================================================================
class DatabaseMemento
{
private:
    // State ki COPY (reference/pointer nahi!) — isliye baad me original
    // database kitna bhi badle, ye snapshot waisa ka waisa safe rahega.
    map<string, string> data;

public:
    // Constructor snapshot ke waqt ka pura data copy kar leta hai
    DatabaseMemento(const map<string, string> &dbData)
    {
        this->data = dbData;
    }

    // Restore ke waqt Originator isi se data wapas lega.
    // const hai — memento IMMUTABLE hona chahiye (snapshot badalta nahi!)
    map<string, string> getState() const
    {
        return data;
    }
};

// ============================================================================
//  SECTION 2: ORIGINATOR — Database (asli maal jiska backup lena hai)
// ----------------------------------------------------------------------------
//  Ye wo object hai jiska state save/restore hota hai. Do khaas methods:
//    createMemento()       → apne CURRENT state ka snapshot bana ke do
//    restoreFromMemento()  → diye gaye snapshot pe WAPAS chale jao
//  Baaki insert/update/remove normal business operations hain.
//
//  DHYAN DO: records private hai aur private hi rahega — snapshot lene
//  ke liye kisi ko bhi records directly access nahi karna padta.
//  Originator KHUD apna snapshot banata hai. Yahi encapsulation ki
//  jeet hai!
// ============================================================================
class Database
{
private:
    // Actual data — simple key-value store (map). Ye PRIVATE hai aur
    // Memento pattern ke baad bhi private hi hai!
    map<string, string> records;

public:
    // -------------------- NORMAL OPERATIONS --------------------
    // In teeno ka Memento se koi lena-dena nahi — ye bas database ka
    // rozmarra ka kaam hai (jise transaction me wrap karenge).

    // Naya record daalo (ya existing overwrite karo)
    void insert(const string &key, const string &value)
    {
        records[key] = value;
        cout << "Inserted: " << key << " = " << value << endl;
    }

    // Existing record update karo — pehle check karo key hai bhi ya nahi
    void update(const string &key, const string &value)
    {
        if (records.find(key) != records.end())
        {
            records[key] = value;
            cout << "Updated: " << key << " = " << value << endl;
        }
        else
        {
            cout << "Key not found for update: " << key << endl;
        }
    }

    // Record delete karo
    void remove(const string &key)
    {
        auto it = records.find(key);
        if (it != records.end())
        {
            records.erase(it);
            cout << "Deleted: " << key << endl;
        }
        else
        {
            cout << "Key not found for deletion: " << key << endl;
        }
    }

    // -------------------- MEMENTO KA CORE #1: SAVE --------------------
    // Current state ka snapshot banao aur return karo. Records ki COPY
    // memento me jaati hai — iske baad database kuch bhi kare, snapshot
    // par asar nahi.
    // (Return heap pointer hai — caller/Caretaker delete ka zimmedar.
    //  Production me unique_ptr<DatabaseMemento> return karna best!)
    DatabaseMemento *createMemento()
    {
        cout << "Creating database backup..." << endl;
        return new DatabaseMemento(records);
    }

    // -------------------- MEMENTO KA CORE #2: RESTORE --------------------
    // Snapshot se pura state WAPAS laga do — jo bhi changes snapshot ke
    // baad hue the, sab gayab! Ek hi line me poora rollback: records
    // ko memento wale purane data se replace kar do.
    void restoreFromMemento(const DatabaseMemento &memento)
    {
        records = memento.getState();
        cout << "Database restored from backup!" << endl;
    }

    // Current state print karne ka helper — demo me before/after
    // dikhane ke liye
    void displayRecords()
    {
        cout << "\n--- Current Database State ---" << endl;
        if (records.empty())
        {
            cout << "Database is empty" << endl;
        }
        else
        {
            for (const auto &record : records)
            {
                cout << record.first << " = " << record.second << endl;
            }
        }
        cout << "-----------------------------\n"
             << endl;
    }
};

// ============================================================================
//  SECTION 3: CARETAKER — TransactionManager (backup ka rakhwala 🔐)
// ----------------------------------------------------------------------------
//  Caretaker ka kaam: memento ko SAMBHALNA aur transaction ka lifecycle
//  chalana (begin/commit/rollback).
//
//  SABSE IMPORTANT BAAT: Ye class memento ke ANDAR kabhi nahi jhankti!
//  Na getState() call karti hai, na data padhti hai — bas pointer pakad
//  ke rakhti hai aur sahi waqt pe Database ko wapas de deti hai.
//  "Envelope rakho, kholo mat" — yahi Caretaker ka contract hai.
// ============================================================================
class TransactionManager
{
private:
    // Ek waqt me ek hi backup (single-level undo). Undo STACK chahiye
    // to yahan vector/stack<DatabaseMemento*> rakh lo — multi-level ho jayega!
    DatabaseMemento *backup;

public:
    TransactionManager() : backup(nullptr) {}

    // Destructor — agar koi transaction bina commit/rollback ke reh
    // gayi to uska backup yahan clean hoga (memory leak se bachav)
    ~TransactionManager()
    {
        if (backup)
        {
            delete backup;
        }
    }

    // -------------------- BEGIN: snapshot lo --------------------
    // Transaction shuru = current state ka backup le lo. Ab aage jo
    // bhi changes honge, unhe undo karne ka rasta hamare paas hai.
    void beginTransaction(Database &db)
    {
        cout << "=== BEGIN TRANSACTION ===" << endl;
        if (backup)
        {
            delete backup; // purana backup pada tha to pehle usse hatao (leak nahi!)
        }
        backup = db.createMemento(); // Originator se snapshot MANGA —
                                     // khud nahi banaya (encapsulation!)
    }

    // -------------------- COMMIT: backup phenko --------------------
    // Sab changes pakke ho gaye — ab purane state ki zaroorat nahi,
    // backup delete karke memory free kar do.
    void commitTransaction()
    {
        cout << "=== COMMIT TRANSACTION ===" << endl;
        if (backup)
        {
            delete backup;
            backup = nullptr;
        }
        cout << "Transaction committed successfully!" << endl;
    }

    // -------------------- ROLLBACK: backup se wapas jao --------------------
    // Kuch galat ho gaya! Database ko snapshot wapas de do — wo khud
    // apna state restore kar lega. Caretaker ne data ko touch tak
    // nahi kiya — bas envelope wapas kiya.
    void rollbackTransaction(Database &db)
    {
        cout << "=== ROLLBACK TRANSACTION ===" << endl;
        if (backup)
        {
            db.restoreFromMemento(*backup); // restore Originator karta hai, hum nahi!
            delete backup;
            backup = nullptr;
            cout << "Transaction rolled back!" << endl;
        }
        else
        {
            // Bina begin kiye rollback? — gracefully handle karo
            cout << "No backup available for rollback!" << endl;
        }
    }
};

// ============================================================================
//  SECTION 4: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Do scenarios dikhate hain:
//    1. SUCCESS: begin → changes → commit (changes pakke)
//    2. FAILURE: begin → changes → ERROR → rollback (changes gayab!)
// ============================================================================
int main()
{
    Database db;
    TransactionManager txManager;

    // ---------------- SCENARIO 1: SUCCESS ----------------
    // begin → empty database ka snapshot bana
    txManager.beginTransaction(db);
    db.insert("user1", "Hardik");
    db.insert("user2", "Rohit");
    // Sab theek tha → commit — backup discard, changes permanent
    txManager.commitTransaction();

    db.displayRecords(); // user1, user2 dikhenge ✅

    // ---------------- SCENARIO 2: FAILURE + ROLLBACK ----------------
    // begin → is waqt ka snapshot bana (user1 + user2 wala state)
    txManager.beginTransaction(db);
    db.insert("user3", "Saurav");
    db.insert("user4", "Manish");

    db.displayRecords(); // abhi chaaro users dikhenge (user3/4 abhi kachhe hain)

    // Oops — kuch phat gaya! Ab rollback karo:
    cout << "ERROR: Something went wrong during transaction!" << endl;
    txManager.rollbackTransaction(db);
    // → snapshot wala state wapas — user3/user4 aise gayab jaise aaye hi nahi the!

    db.displayRecords(); // wapas sirf user1, user2 ✅

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  === BEGIN TRANSACTION ===
//  Creating database backup...
//  Inserted: user1 = Hardik
//  Inserted: user2 = Rohit
//  === COMMIT TRANSACTION ===
//  Transaction committed successfully!
//
//  --- Current Database State ---
//  user1 = Hardik
//  user2 = Rohit
//  -----------------------------
//
//  === BEGIN TRANSACTION ===
//  Creating database backup...
//  Inserted: user3 = Saurav
//  Inserted: user4 = Manish
//
//  --- Current Database State ---
//  user1 = Hardik
//  user2 = Rohit
//  user3 = Saurav
//  user4 = Manish
//  -----------------------------
//
//  ERROR: Something went wrong during transaction!
//  === ROLLBACK TRANSACTION ===
//  Database restored from backup!
//  Transaction rolled back!
//
//  --- Current Database State ---
//  user1 = Hardik
//  user2 = Rohit
//  -----------------------------
//
//  Dekho — rollback ke baad user3/user4 GAYAB, bilkul snapshot wala
//  state wapas. Aur poore flow me kisi ne bhi Database ka private
//  `records` directly nahi chheda — encapsulation intact! ✅
// ============================================================================
