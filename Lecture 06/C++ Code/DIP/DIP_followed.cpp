#include <bits/stdc++.h>
using namespace std;

// Abstraction (Interface)
class Database
{
public:
    virtual void save(string data) = 0; // Pure virtual function
};

// MySQL implementation (Low-level module)
class MySQLDatabase : public Database
{
public:
    void save(string data) override
    {
        cout << "Executing SQL Query: INSERT INTO users VALUES('" << data << "');" << endl;
    }
};

// MongoDB implementation (Low-level module)
class MongoDBDatabase : public Database
{
public:
    void save(string data) override
    {
        cout << "Executing MongoDB Function: db.users.insert({name: '" << data << "'})" << endl;
    }
};

// High-level module (Now loosely coupled)
class UserService
{
private:
    Database *db; // Dependency Injection

public:
    UserService(Database *db)
    {
        this->db = db;
    }

    void setDatabase(Database *db)
    {

        this->db = db;
    }

    void storeUser(string user)
    {
        db->save(user);
    }
};

int main()
{
    MySQLDatabase *mysql = new MySQLDatabase();
    MongoDBDatabase *mongodb = new MongoDBDatabase();

    UserService *service1 = new UserService(mysql);
    service1->storeUser("Aditya");

    UserService *service2 = new UserService(mongodb);
    service2->storeUser("Rohit");
}
