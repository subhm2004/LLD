#ifndef ATM_LLD_ENUMS_TRANSACTIONTYPE_H
#define ATM_LLD_ENUMS_TRANSACTIONTYPE_H

#include <bits/stdc++.h>

using namespace std;

namespace atm_lld {

/**
 * @enum TransactionType
 * @brief ATM system me hone wale alag-alag transactions ke types ko represent karta hai.
 */
enum class TransactionType { 
    BALANCE_INQUIRY, // Account balance check karne ki query transaction.
    CASH_WITHDRAWAL  // Account se cash nikalne ki withdrawal transaction.
};

} // namespace atm_lld

#endif // ATM_LLD_ENUMS_TRANSACTIONTYPE_H
