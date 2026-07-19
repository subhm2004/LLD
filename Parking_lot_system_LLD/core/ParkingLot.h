// ============================================================================
//  core/ParkingLot.h — SYSTEM KA DIL: FACADE + OBSERVABLE + saare rules ❤️
// ----------------------------------------------------------------------------
//  Poore parking lot ka orchestrator. Client (main) ko sirf simple API deta —
//  andar spots, tickets, pricing, payment, gates, display board sab coordinate
//  karta hai. Ye implement karta hai LAGBHAG SAARE requirements:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  REQUIREMENTS -> ISME KAHAN:                                            │
//  │   #1 Capacity 40,000     -> maxCapacity_ (constructor me set)           │
//  │   #2 4 spot types        -> addSpot(id, SpotType)                       │
//  │   #3 Multiple gates      -> entryGates_ / exitGates_                    │
//  │   #4 4 vehicle types     -> parkVehicle(Vehicle)                        │
//  │   #5 Display board        -> DisplayBoard observer (notify on change)   │
//  │   #6 Max capacity rule    -> isFull() check park se pehle              │
//  │   #7 Full notification    -> observers ko isFull=true notify           │
//  │   #8 Ticket               -> parkVehicle ticket deta                    │
//  │   #9 Payment machine/agent-> unparkVehicle PaymentChannel leta          │
//  │   #10 Hourly rates        -> pricingStrategy_ (Strategy)                │
//  │   #11 Payment methods     -> unparkVehicle PaymentMethod leta           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERNS ISME:
//    - FACADE     : client ko simple API, andar sab manage
//    - OBSERVABLE : DisplayBoard ko notify karta (Observer subject)
//    - STRATEGY   : pricingStrategy_ (fee) + payment factory se processor
//    - FACTORY    : PaymentProcessorFactory se payment channel banwata
//
//  ⭐ OBSERVER FLOW: har park/unpark ke baad notifyObservers() -> DisplayBoard
//  automatically update. Board ko manually refresh nahi karna padta.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_CORE_PARKINGLOT_H
#define PARKING_LOT_SYSTEM_LLD_CORE_PARKINGLOT_H

#include <bits/stdc++.h>

#include "../enums/PaymentChannel.h"
#include "../enums/PaymentMethod.h"
#include "../enums/SpotType.h"
#include "../enums/VehicleType.h"
#include "../models/DisplayBoard.h"
#include "../models/Gate.h"
#include "../models/ParkingSpot.h"
#include "../models/Ticket.h"
#include "../models/Vehicle.h"
#include "../observers/IParkingObserver.h"
#include "../payment/PaymentProcessor.h"
#include "../payment/PaymentProcessorFactory.h"
#include "../strategies/PricingStrategy.h"

using namespace std;

namespace parking_lot_lld {

class ParkingLot {
public:
    // maxCapacity default 40,000 (requirement #1). Pricing strategy inject.
    explicit ParkingLot(PricingStrategy *pricingStrategy, int maxCapacity = 40000)
        : pricingStrategy_(pricingStrategy), maxCapacity_(maxCapacity) {}

    ~ParkingLot() {
        for (ParkingSpot *spot : spots_) {
            delete spot;
        }
        for (auto &entry : activeTickets_) {
            delete entry.second;
        }
        for (Gate *gate : entryGates_) {
            delete gate;
        }
        for (Gate *gate : exitGates_) {
            delete gate;
        }
        delete pricingStrategy_;
        // NOTE: observers (DisplayBoard) is lot ke owner nahi — caller manage kare
    }

    // -------------------- SETUP --------------------

    // Spot add karo (requirement #2). Capacity guard (requirement #1/#6):
    // total spots maxCapacity se zyada nahi ho sakte.
    void addSpot(int spotId, SpotType type) {
        if (static_cast<int>(spots_.size()) >= maxCapacity_) {
            throw runtime_error("Cannot add spot: parking lot capacity reached");
        }
        spots_.push_back(new ParkingSpot(spotId, type));
    }

    // Multiple entry/exit gates (requirement #3)
    void addEntryGate(int gateId) { entryGates_.push_back(new Gate(gateId, GateType::ENTRY)); }
    void addExitGate(int gateId) { exitGates_.push_back(new Gate(gateId, GateType::EXIT)); }

    // OBSERVER register — DisplayBoard (ya koi bhi observer) ko subscribe karo
    void addObserver(IParkingObserver *observer) {
        observers_.push_back(observer);
        // register hote hi ek initial update do (taaki board current state pakde)
        notifyObservers();
    }

    // -------------------- ENTRY (park) --------------------

    // Vehicle park karo — requirement #4/#6/#8. entryGateId = kaunse gate se aaya.
    // Return: ticket id.
    string parkVehicle(const Vehicle &vehicle, int entryGateId) {
        // REQUIREMENT #6 — MAX CAPACITY RULE: full ho to admit mat karo
        if (isFull()) {
            cout << "[Entry-Gate-" << entryGateId << "] Parking is FULL. " << vehicle.getNumber()
                 << " cannot be admitted.\n";
            notifyObservers();  // board pe full notification (requirement #7)
            throw runtime_error("Parking lot is full");
        }

        ParkingSpot *spot = findAvailableSpot(vehicle);
        if (!spot) {
            // Lot full nahi, par IS vehicle ke liye koi compatible spot khaali nahi
            throw runtime_error("No compatible spot available for vehicle type: " +
                                vehicleTypeToString(vehicle.getType()));
        }

        spot->parkVehicle(vehicle.getNumber());
        // REQUIREMENT #8 — TICKET generate karo
        const string ticketId = "TICKET_" + to_string(++ticketCounter_);
        auto *ticket = new Ticket(ticketId, vehicle.getNumber(), spot->getSpotId(), time(nullptr), entryGateId);
        activeTickets_[ticketId] = ticket;
        vehicleTypeByTicket_[ticketId] = vehicle.getType();

        cout << "[Entry-Gate-" << entryGateId << "] Parked " << vehicle.getNumber()
             << " at spot " << spot->getSpotId() << " (" << spotTypeToString(spot->getSpotType())
             << ") | Ticket: " << ticketId << "\n";

        notifyObservers();  // REQUIREMENT #5/#7 — display board update
        return ticketId;
    }

    // -------------------- EXIT (unpark + pay) --------------------

    // Vehicle exit — requirement #8/#9/#10/#11. Ticket validate -> fee (Strategy)
    // -> payment (channel + method) -> spot free. Return: fee paid.
    double unparkVehicle(const string &ticketId, int exitGateId, PaymentChannel channel, PaymentMethod method) {
        auto ticketIt = activeTickets_.find(ticketId);
        if (ticketIt == activeTickets_.end()) {
            throw runtime_error("Invalid ticket ID: " + ticketId);
        }

        Ticket *ticket = ticketIt->second;
        ParkingSpot *spot = getSpotById(ticket->getSpotId());
        if (!spot) {
            throw runtime_error("Parking spot not found for ticket");
        }

        // REQUIREMENT #10 — fee = duration × rate (Strategy)
        const long long exitTime = time(nullptr);
        const VehicleType type = vehicleTypeByTicket_[ticketId];
        const double fee = pricingStrategy_->calculateFee(type, ticket->getEntryTime(), exitTime);

        cout << "[Exit-Gate-" << exitGateId << "] Vehicle " << ticket->getVehicleNumber()
             << " | Fee due: Rs " << fee << "\n";

        // REQUIREMENT #9/#11 — payment channel (machine/agent) + method (card/cash/upi).
        // FACTORY se sahi processor banwao, phir usse pay karwao (Strategy).
        PaymentProcessor *processor = PaymentProcessorFactory::create(channel);
        const bool paid = processor->processPayment(fee, method);
        delete processor;  // ek-baar use, delete

        if (!paid) {
            throw runtime_error("Payment failed for ticket: " + ticketId);
        }

        // Payment ho gaya -> spot free karo, ticket close karo
        spot->removeVehicle();
        cout << "[Exit-Gate-" << exitGateId << "] Unparked from spot " << spot->getSpotId() << ". Thank you!\n";

        delete ticket;
        activeTickets_.erase(ticketId);
        vehicleTypeByTicket_.erase(ticketId);

        notifyObservers();  // spot free hua -> board update
        return fee;
    }

    // -------------------- STATUS --------------------

    // REQUIREMENT #6 — lot full hai? (occupied count == total spots)
    bool isFull() const {
        if (spots_.empty()) {
            return false;
        }
        return occupiedCount() >= static_cast<int>(spots_.size());
    }

    // Per-spot-type available counts (DisplayBoard ke liye + report)
    map<SpotType, int> getAvailability() const {
        map<SpotType, int> availability;
        // Saare spot types 0 se initialize (taaki 0-available bhi dikhe)
        availability[SpotType::HANDICAPPED] = 0;
        availability[SpotType::COMPACT] = 0;
        availability[SpotType::LARGE] = 0;
        availability[SpotType::MOTORCYCLE] = 0;
        for (const ParkingSpot *spot : spots_) {
            if (!spot->isOccupied()) {
                availability[spot->getSpotType()]++;
            }
        }
        return availability;
    }

private:
    vector<ParkingSpot *> spots_;
    unordered_map<string, Ticket *> activeTickets_;
    unordered_map<string, VehicleType> vehicleTypeByTicket_;
    vector<Gate *> entryGates_;
    vector<Gate *> exitGates_;
    vector<IParkingObserver *> observers_;  // DisplayBoard etc.
    PricingStrategy *pricingStrategy_;
    int maxCapacity_;
    int ticketCounter_ = 0;

    int occupiedCount() const {
        int count = 0;
        for (const ParkingSpot *spot : spots_) {
            if (spot->isOccupied()) {
                ++count;
            }
        }
        return count;
    }

    // OBSERVER subject ka core: saare observers ko current state bhejo
    void notifyObservers() {
        const map<SpotType, int> availability = getAvailability();
        const bool full = isFull();
        for (IParkingObserver *observer : observers_) {
            observer->onParkingUpdate(availability, full);
        }
    }

    // >>> BEST-FIT ALLOCATION <<< (naive first-fit se behtar design)
    // Vehicle ko "tightest" (sabse fitting) spot pehle do — jaise motorcycle
    // ko MOTORCYCLE spot pehle, COMPACT baad me. Warna motorcycle COMPACT spot
    // le leti aur car ke liye jagah waste ho jaati (real bug!). Preference
    // order se ye avoid hota — chhoti vehicle chhote spot me, bade spot bache
    // rahein bade vehicles ke liye. Ye ek classic parking-lot optimization hai.
    ParkingSpot *findAvailableSpot(const Vehicle &vehicle) {
        for (SpotType preferredType : getSpotPreference(vehicle)) {
            for (ParkingSpot *spot : spots_) {
                if (spot->getSpotType() == preferredType && spot->canFitVehicle(vehicle)) {
                    return spot;
                }
            }
        }
        return nullptr;
    }

    // Vehicle ke liye spot types ka preference order (tightest -> loosest).
    // Handicapped permit ho to HANDICAPPED sabse pehle (entitled to convenient spot).
    vector<SpotType> getSpotPreference(const Vehicle &vehicle) const {
        vector<SpotType> preference;
        if (vehicle.hasHandicappedPermit()) {
            preference.push_back(SpotType::HANDICAPPED);
        }
        switch (vehicle.getType()) {
            case VehicleType::MOTORCYCLE:
                preference.push_back(SpotType::MOTORCYCLE);  // pehla choice
                preference.push_back(SpotType::COMPACT);     // warna compact
                preference.push_back(SpotType::LARGE);       // warna large
                break;
            case VehicleType::CAR:
                preference.push_back(SpotType::COMPACT);     // car ko compact pehle
                preference.push_back(SpotType::LARGE);       // warna large
                break;
            case VehicleType::VAN:
            case VehicleType::TRUCK:
                preference.push_back(SpotType::LARGE);       // bade vehicles sirf large
                break;
        }
        return preference;
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
