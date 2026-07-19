// ============================================================================
//  observers/IRentalObserver.h — OBSERVER interface (requirement #10) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #10: vehicle time pe return na ho to customer ko NOTIFY karo
//  (+ fine). Notification bhejne ka kaam Observer pattern (L12) se — system
//  event fire karta ("booking overdue"), observers react karte.
//
//  onOverdue() — jab koi booking overdue detect ho, system saare observers
//  ko notify karta. CustomerNotifier is interface ko implement karta
//  (customer ko message deta). Naya notification channel (SMS/email/push)
//  add karna ho to bas naya observer — system untouched.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_OBSERVERS_IRENTALOBSERVER_H
#define CAR_RENTAL_SYSTEM_LLD_OBSERVERS_IRENTALOBSERVER_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

class IRentalObserver {
public:
    virtual ~IRentalObserver() = default;

    // Booking overdue hui — customer ko notify karo (fine ke saath)
    virtual void onOverdue(const string &bookingId, const string &customerName, int overdueDays, double fine) = 0;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_OBSERVERS_IRENTALOBSERVER_H
