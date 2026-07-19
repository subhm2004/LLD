// ============================================================================
//  observers/CustomerNotifier.h — Concrete OBSERVER (customer ko notify) [NAYA]
// ----------------------------------------------------------------------------
//  IRentalObserver ka implementation — overdue hone pe customer ko notification
//  message deta (requirement #10). Real me ye SMS/email/push bhejta; demo me
//  print. Naye channels ke liye naye observers add ho sakte, system untouched.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_OBSERVERS_CUSTOMERNOTIFIER_H
#define CAR_RENTAL_SYSTEM_LLD_OBSERVERS_CUSTOMERNOTIFIER_H

#include <bits/stdc++.h>

#include "IRentalObserver.h"

using namespace std;

namespace car_rental_lld {

class CustomerNotifier : public IRentalObserver {
public:
    void onOverdue(const string &bookingId, const string &customerName, int overdueDays, double fine) override {
        cout << "  [Notification -> " << customerName << "] Your rental (" << bookingId
             << ") is OVERDUE by " << overdueDays << " day(s). A fine of Rs " << fine
             << " has been applied. Please return the vehicle ASAP.\n";
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_OBSERVERS_CUSTOMERNOTIFIER_H
