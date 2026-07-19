#ifndef ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
#define ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H

#include <bits/stdc++.h>

#include "../models/Card.h"

using namespace std;

namespace atm_lld {

/**
 * @class AuthenticationService
 * @brief User aur Card credentials verification karne wali service class.
 * 
 * Single responsibility principle follow karti hai: Iska kaam sirf card PIN check aur match karna hai.
 */
class AuthenticationService {
public:
    // Card pointer aur user ke input PIN code validation check method.
    bool authenticate(const Card &card, const string &enteredPin) const { 
        return card.validatePin(enteredPin); 
    }
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
