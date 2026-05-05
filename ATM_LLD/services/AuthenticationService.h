#ifndef ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
#define ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H

#include <bits/stdc++.h>

#include "../models/Card.h"

using namespace std;

namespace atm_lld {

class AuthenticationService {
public:
    bool authenticate(const Card &card, const string &enteredPin) const { return card.validatePin(enteredPin); }
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_AUTHENTICATIONSERVICE_H
