// ============================================================================
//  DIP_followed_new_pointer_style.cpp  —  DIP fix, modern smart-pointer style
// ----------------------------------------------------------------------------
//  Wahi DIP fix (UserService -> Database abstraction), par raw pointer ki jagah
//  `unique_ptr` se ownership clear aur memory management automatic. Concrete DB
//  inject hota hai aur UserService usko own karta hai (RAII). Yeh production
//  jaisa modern C++ idiom hai.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <memory> // unique_ptr ke liye zaroori hai

using namespace std;

// --- 1. Abstraction (Interface) ---
class Database
{
public:
    virtual void save(string data) = 0; // Pure virtual function

    // Virtual destructor: Taaki child classes sahi se saaf hon
    virtual ~Database()
    {
        cout << "Database Interface: Memory Cleaned." << endl;
    }
};

// --- 2. Low-level Modules (Implementations) ---

class MySQLDatabase : public Database
{
public:
    void save(string data) override
    {
        cout << "Executing SQL Query: INSERT INTO users VALUES('" << data << "');" << endl;
    }
    ~MySQLDatabase()
    {
        cout << "MySQL Specific Resources Released." << endl;
    }
};

class MongoDBDatabase : public Database
{
public:
    void save(string data) override
    {
        cout << "Executing MongoDB Function: db.users.insert({name: '" << data << "'})" << endl;
    }
    ~MongoDBDatabase()
    {
        cout << "MongoDB Specific Resources Released." << endl;
    }
};

// --- 3. High-level Module ---

class UserService
{
private:
    unique_ptr<Database> db; // Smart pointer ownership handle karega

public:
    // Constructor: Ownership transfer style
    UserService(unique_ptr<Database> database)
    {
        // std::move haq (ownership) transfer karta hai
        this->db = std::move(database);
    }

    // Method to switch DB at runtime
    void setDatabase(unique_ptr<Database> newDb)
    {
        this->db = std::move(newDb);
    }

    void storeUser(string user)
    {
        if (db)
        {
            db->save(user);
        }
        else
        {
            cout << "Error: No Database connected!" << endl;
        }
    }

    ~UserService()
    {
        cout << "UserService destroyed." << endl;
    }
};

// --- 4. Main Function ---

int main()
{
    cout << "--- Starting Application ---" << endl;

    // make_unique use karein (new likhne ki zaroorat nahi)
    auto mysql = make_unique<MySQLDatabase>();

    // MySQL ke saath service start karein
    // dhyaan dein: mysql pointer ko move karna padega
    UserService service(std::move(mysql));
    service.storeUser("Aditya");

    cout << "\n--- Switching Database ---" << endl;

    // Ab MongoDB mein switch karte hain
    auto mongodb = make_unique<MongoDBDatabase>();
    service.setDatabase(std::move(mongodb));
    service.storeUser("Rohit");

    cout << "\n--- Exiting Application ---" << endl;

    // NOTE: Yahan koi bhi delete likhne ki zaroorat nahi hai.
    // Sab kuch apne aap delete ho jayega jaise hi main khatam hoga.

    return 0;
}