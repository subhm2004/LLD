#ifndef ATM_LLD_ENUMS_TRANSACTIONSTATUS_H
#define ATM_LLD_ENUMS_TRANSACTIONSTATUS_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

/**
 * @enum TransactionStatus
 * @brief ATM transaction ki states/status represent karta hai.
 * 
 * Jab transaction shuru hoti hai toh wo INITIATED hoti hai,
 * aur completion par success ya fail state me transition hoti hai.
 */
enum class TransactionStatus { 
    INITIATED, // Transaction abhi start hui hai aur process ho rahi hai.
    SUCCESS,   // Transaction bina kisi error ke successfully complete ho gayi hai.
    FAILED     // Exception ya validation crash ke chalte transaction fail ho gayi hai.
};

} // namespace atm_lld

#endif // ATM_LLD_ENUMS_TRANSACTIONSTATUS_H
