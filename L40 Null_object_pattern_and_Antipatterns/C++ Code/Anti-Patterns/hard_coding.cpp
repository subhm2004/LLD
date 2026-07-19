/*
=====================================================================
HARD CODING THINGS ANTI-PATTERN
=====================================================================
THEORY:
Hard coding matlab values (jaise strings, numbers, config settings)
ko seedha code ke andar likh dena, instead of unhe configurable
(jaise config file, database, environment variable) banane ke.

PROBLEM:
- Value change karni ho to code recompile karna padega
- Different environments (dev/test/prod) ke liye alag builds banane
  padte hain
- Reusability kam ho jati hai
- Security issue bhi ho sakta hai (jaise API keys hardcode karna)

SOLUTION APPROACH:
- Config files (JSON/YAML) use karo
- Environment variables use karo
- Constants ko ek centralized config class/file me rakho
- Dependency Injection use karo taaki values bahar se pass ho
=====================================================================
*/

#include <iostream>
#include <string>
#include <map>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: values seedhe function ke andar hardcode ki
// ---------------------------------------------------------------
class Greeter
{
public:
    void sayHello()
    {
        string message = "Hello"; // Hardcoded - configurable hona chahiye
        string user = "Admin";    // Hardcoded - ye bhi dynamic hona chahiye
        cout << message << " " << user << endl;
    }
    // Problem: agar message ya default user change karna ho to
    // code recompile karna padega
};

// ---------------------------------------------------------------
// FIXED VERSION: values ko ek config map se le rahe hain
// ---------------------------------------------------------------
class ConfigManager
{
    map<string, string> settings;

public:
    ConfigManager()
    {
        // Real project me ye values JSON/YAML file ya env variable
        // se load hoti, abhi demo ke liye map me daal rahe hain
        settings["greeting_message"] = "Hello";
        settings["default_user"] = "Admin";
    }

    string get(string key)
    {
        return settings[key];
    }
};

class GreeterFixed
{
    ConfigManager *config;

public:
    GreeterFixed(ConfigManager *cfg) : config(cfg) {}

    void sayHello()
    {
        string message = config->get("greeting_message");
        string user = config->get("default_user");
        cout << message << " " << user << endl;
    }
};

int main()
{
    cout << "--- Hard Coding (Anti-Pattern) Demo ---" << endl;
    Greeter greeter;
    greeter.sayHello();

    cout << "\n--- Fixed Version (Config Driven) Demo ---" << endl;
    ConfigManager config;
    GreeterFixed greeterFixed(&config);
    greeterFixed.sayHello();

    return 0;
}