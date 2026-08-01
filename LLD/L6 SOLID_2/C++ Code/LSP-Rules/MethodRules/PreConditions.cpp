// ============================================================================
//  PreConditions.cpp  —  LSP Method Rule: Pre-conditions
// ----------------------------------------------------------------------------
//  Pre-condition = method chalne se PEHLE jo sach hona chahiye (input shart).
//  LSP rule: subtype pre-condition ko WEAKEN kar sakta hai, STRENGTHEN nahi.
//  Matlab override caller se ZYADA demand nahi kar sakta (jaise base 0-100
//  accept karta tha to derived sirf 50-100 maange -> GALAT, LSP break).
//  Yaad rakho: "demand no more" (pre-conditions sirf dheeli ho sakti hain).
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// A Precondition must be statisfied before a method can be executed.
// Sub classes can weaken the precondition but cannot strengthen it.

class User {
public:
    // Precondition: Password must be at least 8 characters long
    virtual void setPassword(string password) {
        if (password.length() < 8) {
            throw invalid_argument("Password must be at least 8 characters long!");
        }
        cout << "Password set successfully" << endl;
    }
};

class AdminUser : public User {
public:
    // Precondition: Password must be at least 6 characters
    void setPassword(string password) override {
        if (password.length() < 6) { 
            throw invalid_argument("Password must be at least 6 characters long!");
        }
        cout << "Password set successfully" << endl;
    }
};

int main() {
    User* user = new AdminUser();
    user->setPassword("Admin1");  // Works fine: AdminUser allows shorter passwords

    return 0;
}
