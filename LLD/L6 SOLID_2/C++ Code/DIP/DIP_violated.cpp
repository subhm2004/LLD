// ============================================================================
//  DIP_violated.cpp  —  Dependency Inversion Principle ka VIOLATION
// ----------------------------------------------------------------------------
//  DIP: high-level module (policy) ko low-level module (detail) par DIRECT
//  depend nahi karna chahiye — dono ko abstraction par depend karna chahiye.
//  Yahan high-level UserService seedha concrete MySQLDatabase ko banata/use
//  karta hai. Mongo pe switch karna ho? UserService edit karni padegi; test ke
//  liye real DB chahiye. Fix: DIP_followed.cpp (Database interface + injection).
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

class MySQLDatabase
{ // Low-level module
public:
    void saveToSQL(string data)
    {
        cout << "Executing SQL Query: INSERT INTO users VALUES('" << data << "');" << endl;
    }
};

class MongoDBDatabase
{ // Low-level module
public:
    void saveToMongo(string data)
    {
        cout << "Executing MongoDB Function: db.users.insert({name: '" << data << "'})" << endl;
    }
};

class UserService
{ // High-level module (Tightly coupled)
private:
    MySQLDatabase *sqlDb;     // Direct dependency on MySQL
    MongoDBDatabase *mongoDb; // Direct dependency on MongoDB

public:
    void storeUserToSQL(string user)
    {
        // MySQL-specific code
        sqlDb->saveToSQL(user);
    }

    void storeUserToMongo(string user)
    {
        // MongoDB-specific code
        mongoDb->saveToMongo(user);
    }
};

int main()
{
    UserService *service = new UserService();
    service->storeUserToSQL("Aditya");
    service->storeUserToMongo("Rohit");
}
