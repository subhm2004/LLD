// Double Checked Locking Pattern (DCLP)
// DCLP ek design pattern hai jo thread-safe singleton implementation ke liye use hota hai.
// Is pattern ka main idea hai ki hum singleton instance ko lazy initialization ke saath create karte hain, aur performance ko optimize karne ke liye double-checking mechanism use karte hain
// C++14 mein DCLP ko sahi tarike se implement karna thoda tricky ho sakta hai, kyunki C++11 ke memory model ke baad hi is pattern ko safely implement kiya ja sakta hai. C++14 mein bhi aapko mutex aur memory barriers ka sahi use karna hoga taaki thread safety ensure ho sake.
// Yahan ek example diya gaya hai ki kaise aap DCLP ko C++14 mein implement kar sakte hain:
#include <iostream>
#include <mutex>

using namespace std;

class DatabaseService
{
private:
    // 1. Static instance pointer
    static DatabaseService *instance;
    static mutex mtx;

    // Private constructor taaki bahar se object na ban sake
    DatabaseService()
    {
        std::cout << "Database Service Initialized (Heavy Task)\n";
    }

    // Singleton integrity rules:
    // In 4 lines ka purpose hai ki existing singleton object ka duplicate
    // copy/move se kabhi create na ho sake.

    // 1) Copy constructor block:
    //    DatabaseService b(a); ya DatabaseService b = a; allow nahi hoga.
    DatabaseService(const DatabaseService &) = delete;

    // 2) Copy assignment block:
    //    b = a; type assignment disallow.
    DatabaseService &operator=(const DatabaseService &) = delete;

    // 3) Move constructor block:
    //    DatabaseService b(std::move(a)); disallow.
    DatabaseService(DatabaseService &&) = delete;

    // 4) Move assignment block:
    //    b = std::move(a); disallow.
    DatabaseService &operator=(DatabaseService &&) = delete;

public:

    static DatabaseService *getInstance()
    {
        // --- STEP 1: Pehla Check (Optimization) ---
        // Agar instance pehle se bana hai, toh lock lene ki zaroorat hi nahi hai.
        // Isse performance fast rehti hai kyunki lock lena ek "expensive" operation hai.
        if (instance == nullptr)
        {

            // --- STEP 2: Lock Lagana ---
            // Agar instance null hai, toh hi lock lo taaki sirf EK thread andar jaye.
            lock_guard<mutex> lock(mtx);

            // --- STEP 3: Doosra Check (Actual Safety) ---
            // Maan lo Thread A aur Thread B dono ne Step 1 pass kiya.
            // Thread A ne lock liya aur object bana diya.
            // Jab Thread A lock chhodegi, Thread B andar aayegi.
            // Agar hum ye second check nahi lagayenge, toh Thread B fir se naya object bana degi.
            if (instance == nullptr)
            {
                instance = new DatabaseService();
            }
        }
        return instance;
    }

    void query(std::string sql)
    {
        cout << "Executing: " << sql << endl;
    }
};

// Static variables ko initialize karna
DatabaseService *DatabaseService::instance = nullptr;
mutex DatabaseService::mtx;

int main()
{
    // Multiple calls, lekin object sirf ek hi baar banega
    DatabaseService *db1 = DatabaseService::getInstance();
    DatabaseService *db2 = DatabaseService::getInstance();

    db1->query("SELECT * FROM users");

    return 0;
}