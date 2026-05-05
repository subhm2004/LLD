#ifndef PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H
#define PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

class Ticket {
public:
    Ticket(string ticketId, string vehicleNumber, int spotId, long long entryTime)
        : ticketId_(std::move(ticketId)),
          vehicleNumber_(std::move(vehicleNumber)),
          spotId_(spotId),
          entryTime_(entryTime) {}

    const string &getTicketId() const { return ticketId_; }
    const string &getVehicleNumber() const { return vehicleNumber_; }
    int getSpotId() const { return spotId_; }
    long long getEntryTime() const { return entryTime_; }

private:
    string ticketId_;
    string vehicleNumber_;
    int spotId_;
    long long entryTime_;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_MODELS_TICKET_H
