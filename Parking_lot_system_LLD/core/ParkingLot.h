#ifndef PARKING_LOT_SYSTEM_LLD_CORE_PARKINGLOT_H
#define PARKING_LOT_SYSTEM_LLD_CORE_PARKINGLOT_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"
#include "../models/ParkingSpot.h"
#include "../models/Ticket.h"
#include "../models/Vehicle.h"
#include "../strategies/PricingStrategy.h"

using namespace std;

namespace parking_lot_lld {

class ParkingLot {
public:
    explicit ParkingLot(PricingStrategy *pricingStrategy) : pricingStrategy_(pricingStrategy) {}

    ~ParkingLot() {
        for (ParkingSpot *spot : spots_) {
            delete spot;
        }
        for (auto &entry : activeTickets_) {
            delete entry.second;
        }
        delete pricingStrategy_;
    }

    void addSpot(int spotId, VehicleType type) {
        spots_.push_back(new ParkingSpot(spotId, type));
    }

    string parkVehicle(const Vehicle &vehicle) {
        ParkingSpot *spot = findAvailableSpot(vehicle);
        if (!spot) {
            throw runtime_error("No spot available for vehicle type: " + vehicleTypeToString(vehicle.getType()));
        }

        spot->parkVehicle(vehicle.getNumber());
        const string ticketId = "TICKET_" + to_string(++ticketCounter_);
        auto *ticket = new Ticket(ticketId, vehicle.getNumber(), spot->getSpotId(), time(nullptr));
        activeTickets_[ticketId] = ticket;
        vehicleTypeByTicket_[ticketId] = vehicle.getType();

        cout << "Parked " << vehicle.getNumber() << " at spot " << spot->getSpotId() << " | Ticket: " << ticketId
             << "\n";
        return ticketId;
    }

    double unparkVehicle(const string &ticketId) {
        auto ticketIt = activeTickets_.find(ticketId);
        if (ticketIt == activeTickets_.end()) {
            throw runtime_error("Invalid ticket ID: " + ticketId);
        }

        Ticket *ticket = ticketIt->second;
        ParkingSpot *spot = getSpotById(ticket->getSpotId());
        if (!spot) {
            throw runtime_error("Parking spot not found for ticket");
        }

        const long long exitTime = time(nullptr);
        const VehicleType type = vehicleTypeByTicket_[ticketId];
        const double fee = pricingStrategy_->calculateFee(type, ticket->getEntryTime(), exitTime);
        spot->removeVehicle();

        cout << "Unparked " << ticket->getVehicleNumber() << " from spot " << spot->getSpotId() << " | Fee: " << fee
             << "\n";

        delete ticket;
        activeTickets_.erase(ticketId);
        vehicleTypeByTicket_.erase(ticketId);
        return fee;
    }

    void printAvailability() const {
        int bike = 0;
        int car = 0;
        int truck = 0;

        for (const auto &spot : spots_) {
            if (!spot->isOccupied()) {
                if (spot->getSupportedType() == VehicleType::BIKE) {
                    ++bike;
                } else if (spot->getSupportedType() == VehicleType::CAR) {
                    ++car;
                } else {
                    ++truck;
                }
            }
        }

        cout << "\n=== Available Spots ===\n";
        cout << "BIKE: " << bike << "\n";
        cout << "CAR: " << car << "\n";
        cout << "TRUCK: " << truck << "\n";
    }

private:
    vector<ParkingSpot *> spots_;
    unordered_map<string, Ticket *> activeTickets_;
    unordered_map<string, VehicleType> vehicleTypeByTicket_;
    PricingStrategy *pricingStrategy_;
    int ticketCounter_ = 0;

    ParkingSpot *findAvailableSpot(const Vehicle &vehicle) {
        for (ParkingSpot *spot : spots_) {
            if (spot->canFitVehicle(vehicle)) {
                return spot;
            }
        }
        return nullptr;
    }

    ParkingSpot *getSpotById(int spotId) {
        for (ParkingSpot *spot : spots_) {
            if (spot->getSpotId() == spotId) {
                return spot;
            }
        }
        return nullptr;
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_CORE_PARKINGLOT_H
