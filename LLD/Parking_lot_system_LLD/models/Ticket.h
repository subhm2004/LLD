// ============================================================================
//  models/Ticket.h — Parking ticket (entry pe milta, exit pe use hota) [req #8]
// ----------------------------------------------------------------------------
//  Requirement #8: customer ko entry pe ticket milta hai, jo exit pe fee
//  calculate + pay karne ke liye use hota hai.
//
//  Ticket store karta hai: ticketId + vehicle number + kaunse spot pe khada +
//  entryTime (fee = duration × rate, isliye entry time zaroori) + entryGateId
//  (kaunse gate se aaya — multiple gates ke liye, requirement #3).
//
//  Immutable — banne ke baad ticket ka data badalta nahi. entryTime hi exit
//  pe duration nikaalne ka aadhaar (exitTime - entryTime = kitni der khada tha).
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

class Ticket {
public:
    Ticket(string ticketId, string vehicleNumber, int spotId, long long entryTime, int entryGateId)
        : ticketId_(std::move(ticketId)),
          vehicleNumber_(std::move(vehicleNumber)),
          spotId_(spotId),
          entryTime_(entryTime),
          entryGateId_(entryGateId) {}

    const string &getTicketId() const { return ticketId_; }
    const string &getVehicleNumber() const { return vehicleNumber_; }
    int getSpotId() const { return spotId_; }
    long long getEntryTime() const { return entryTime_; }
    int getEntryGateId() const { return entryGateId_; }

private:
    string ticketId_;
    string vehicleNumber_;
    int spotId_;
    long long entryTime_;   // fee calculation ka base
    int entryGateId_;       // kaunse entrance se aaya (multi-gate tracking)
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H
