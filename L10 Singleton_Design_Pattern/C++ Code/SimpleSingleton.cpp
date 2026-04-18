// Singleton ka mltb ek esi class banana jisk hum bas ek hi object bana paye dusra object banana hi possible na ho

#include <bits/stdc++.h>

using namespace std;

class Singleton
{
private:
    static Singleton *instance;

    Singleton()
    {
        cout << "Singleton Constructor called" << endl;
    }

public:
    static Singleton *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new Singleton();
        }
        return instance;
    }
};

// Initialize static  (class ke bahar declare karna padta hai)
Singleton *Singleton::instance = nullptr;

int main()
{
    Singleton *s1 = Singleton::getInstance();
    Singleton *s2 = Singleton::getInstance();

    cout << (s1 == s2) << endl;
}