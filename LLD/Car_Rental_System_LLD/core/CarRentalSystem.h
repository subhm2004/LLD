// ============================================================================
//  core/CarRentalSystem.h — SYSTEM KA DIL: FACADE (saare 13 requirements) ❤️
// ----------------------------------------------------------------------------
//  Poore car rental system ka orchestrator. Client (main) ko simple API deta;
//  andar branches, users, vehicles, bookings, add-ons, search, overdue, log —
//  sab coordinate karta. Ye implement karta hai SAARE 13 requirements.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  REQUIREMENTS -> ISME KAHAN:                                            │
//  │   #1 Users (customer/receptionist) -> registerCustomer/Receptionist    │
//  │   #2/#3 Vehicle types + subtypes   -> addVehicle (VehicleFactory)       │
//  │   #4 Reservation records           -> createBooking (issuedDay track)   │
//  │   #5 Rental tracking               -> Customer.rentedCount++/--          │
//  │   #6 Cancellation                  -> cancelBooking                      │
//  │   #7 Vehicle log                   -> vehicleLogs_ (har event logged)   │
//  │   #8/#9 Equipment + services       -> addAddOn (Decorator cost)         │
//  │   #10 Overdue + fines              -> returnVehicle + notify observers  │
//  │   #11 Vehicle search               -> searchByType/Model (SearchService)│
//  │   #12 Branch management            -> branches_ (multiple)              │
//  │   #13 Parking facilities           -> Branch stalls                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERNS: Facade (ye class) + Factory (vehicle) + Decorator (add-on cost) +
//  Strategy (pricing) + Observer (overdue notify). Detail: design_patterns_used.md
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H
#define CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H

#include <bits/stdc++.h>

#include "../decorators/AddOnDecorator.h"
#include "../decorators/BaseRental.h"
#include "../decorators/IRentalCost.h"
#include "../enums/BookingStatus.h"
#include "../enums/VehicleStatus.h"
#include "../enums/VehicleType.h"
#include "../factories/VehicleFactory.h"
#include "../models/AddOn.h"
#include "../models/Booking.h"
#include "../models/Branch.h"
#include "../models/User.h"
#include "../models/Vehicle.h"
#include "../models/VehicleLog.h"
#include "../observers/IRentalObserver.h"
#include "../services/PricingService.h"
#include "../services/SearchService.h"

using namespace std;

namespace car_rental_lld {

class CarRentalSystem {
public:
    ~CarRentalSystem() {
        for (Branch *branch : branches_) {
            delete branch;  // branch apne vehicles bhi delete karta
        }
        for (auto &entry : customers_) {
            delete entry.second;
        }
        for (auto &entry : receptionists_) {
            delete entry.second;
        }
        // observers ka owner ye nahi (caller manage kare)
    }

    // -------------------- USERS (requirement #1) --------------------
    void registerCustomer(Customer *customer) {
        customers_[customer->getId()] = customer;
        cout << "Registered customer: " << customer->getName() << "\n";
    }

    void registerReceptionist(Receptionist *receptionist) {
        receptionists_[receptionist->getId()] = receptionist;
        cout << "Registered receptionist: " << receptionist->getName() << "\n";
    }

    // -------------------- BRANCHES + VEHICLES (req #2/#3/#12/#13) --------------------
    void addBranch(Branch *branch) {
        branches_.push_back(branch);
        cout << "Added branch: " << branch->getName() << " (" << branch->getLocation()
             << ") with " << branch->getTotalStalls() << " parking stalls\n";
    }

    // Vehicle add karo — FACTORY se banega (type+subtype). Branch me park hoga.
    void addVehicle(const string &branchId, VehicleType type, const string &subtype,
                    const string &vehicleId, const string &model, double dailyRate) {
        Branch *branch = getBranchOrThrow(branchId);
        Vehicle *vehicle = VehicleFactory::createVehicle(type, subtype, vehicleId, model, dailyRate);
        branch->addVehicle(vehicle);
        vehicleLogs_[vehicleId].addEntry(0, "Added to branch " + branch->getName());  // req #7
        cout << "Added vehicle: " << vehicle->describe() << " @ " << dailyRate << "/day to " << branch->getName()
             << "\n";
    }

    // -------------------- OBSERVER (requirement #10) --------------------
    void addObserver(IRentalObserver *observer) { observers_.push_back(observer); }

    // -------------------- BOOKING (requirement #4/#8/#9) --------------------
    // Reservation banao. addOns = equipment + services (requirement #8/#9).
    string createBooking(const string &customerId, const string &vehicleId, int startDay, int endDay,
                         int issuedDay, const vector<AddOn> &addOns = {}) {
        Customer *customer = getCustomerOrThrow(customerId);
        Vehicle *vehicle = findVehicleOrThrow(vehicleId);

        if (vehicle->getStatus() != VehicleStatus::AVAILABLE) {
            throw runtime_error("Vehicle not available: " + vehicleId);
        }

        const string branchId = findBranchIdForVehicle(vehicleId);
        const string bookingId = "BOOK_" + to_string(++bookingCounter_);
        Booking booking(bookingId, customerId, vehicleId, branchId, startDay, endDay, issuedDay);

        // Add-ons attach karo (requirement #8/#9)
        for (const AddOn &addOn : addOns) {
            booking.addAddOn(addOn);
        }

        // >>> DECORATOR + STRATEGY se total cost <<<
        // 1. Base amount PricingStrategy se (PricingService context)
        const int days = booking.getDays();
        const double baseAmount = pricingService_.calculateRentAmount(vehicle->getDailyRate(), startDay, endDay);
        // 2. BaseRental component, phir har add-on ko DECORATOR se wrap
        shared_ptr<IRentalCost> cost = make_shared<BaseRental>(baseAmount, days);
        for (const AddOn &addOn : addOns) {
            cost = make_shared<AddOnDecorator>(cost, addOn, days);
        }
        booking.setTotalAmount(cost->getCost());

        booking.setStatus(BookingStatus::CONFIRMED);
        vehicle->setStatus(VehicleStatus::RESERVED);
        customer->incrementRentedCount();  // requirement #5 — rental tracking
        vehicleLogs_[vehicleId].addEntry(issuedDay, "Reserved by " + customer->getName() + " (" + bookingId + ")");  // req #7
        bookings_.insert_or_assign(bookingId, booking);

        cout << "\n[Booking " << bookingId << "] Customer: " << customer->getName() << " | Vehicle: "
             << vehicle->describe() << "\n";
        cout << "  Cost breakdown: " << cost->getDescription() << "\n";
        cout << "  Total amount: Rs " << booking.getTotalAmount() << "\n";
        return bookingId;
    }

    // -------------------- CANCELLATION (requirement #6) --------------------
    void cancelBooking(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CONFIRMED) {
            throw runtime_error("Only confirmed bookings can be cancelled");
        }
        booking->setStatus(BookingStatus::CANCELLED);
        Vehicle *vehicle = findVehicleOrThrow(booking->getVehicleId());
        vehicle->setStatus(VehicleStatus::AVAILABLE);  // vehicle wapas free
        Customer *customer = getCustomerOrThrow(booking->getCustomerId());
        customer->decrementRentedCount();  // rental count wapas
        vehicleLogs_[vehicle->getId()].addEntry(booking->getIssuedDay(), "Booking " + bookingId + " CANCELLED");  // req #7
        cout << "\n[Cancel] Booking " << bookingId << " cancelled. Vehicle available again.\n";
    }

    // -------------------- PICKUP (rental chalu) --------------------
    void pickupVehicle(const string &bookingId, int pickupDay) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CONFIRMED) {
            throw runtime_error("Booking not ready for pickup");
        }
        booking->setStatus(BookingStatus::ACTIVE);
        Vehicle *vehicle = findVehicleOrThrow(booking->getVehicleId());
        vehicle->setStatus(VehicleStatus::RENTED);
        vehicleLogs_[vehicle->getId()].addEntry(pickupDay, "Picked up (" + bookingId + ")");  // req #7
        cout << "\n[Pickup] Booking " << bookingId << " active. Vehicle rented.\n";
    }

    // -------------------- RETURN + OVERDUE/FINE (requirement #10) --------------------
    void returnVehicle(const string &bookingId, int actualReturnDay) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::ACTIVE) {
            throw runtime_error("Booking not active");
        }
        Vehicle *vehicle = findVehicleOrThrow(booking->getVehicleId());

        // REQUIREMENT #10 — overdue check + fine
        const double fine = pricingService_.calculateFine(vehicle->getDailyRate(), booking->getDueDay(), actualReturnDay);
        if (fine > 0.0) {
            booking->setFine(fine);
            const int overdueDays = actualReturnDay - booking->getDueDay();
            Customer *customer = getCustomerOrThrow(booking->getCustomerId());
            // OBSERVER — saare observers ko overdue notify karo
            notifyOverdue(bookingId, customer->getName(), overdueDays, fine);
            vehicleLogs_[vehicle->getId()].addEntry(actualReturnDay, "Returned LATE, fine Rs " + to_string((int)fine));  // req #7
        } else {
            vehicleLogs_[vehicle->getId()].addEntry(actualReturnDay, "Returned on time (" + bookingId + ")");  // req #7
        }

        booking->setStatus(BookingStatus::COMPLETED);
        vehicle->setStatus(VehicleStatus::AVAILABLE);
        Customer *customer = getCustomerOrThrow(booking->getCustomerId());
        customer->decrementRentedCount();  // requirement #5

        cout << "\n[Return] Booking " << bookingId << " completed. ";
        if (fine > 0.0) {
            cout << "Overdue fine: Rs " << fine << ". Total payable: Rs " << (booking->getTotalAmount() + fine) << "\n";
        } else {
            cout << "On time. No fine.\n";
        }
    }

    // -------------------- SEARCH (requirement #11) --------------------
    vector<Vehicle *> searchByType(VehicleType type) const {
        return SearchService::searchByType(branches_, type);
    }
    vector<Vehicle *> searchByModel(const string &query) const {
        return SearchService::searchByModel(branches_, query);
    }

    // -------------------- REPORTS --------------------
    void printInventory() const {
        cout << "\n=== Vehicle Inventory (all branches) ===\n";
        for (Branch *branch : branches_) {
            cout << "Branch: " << branch->getName() << " (free stalls: " << branch->getFreeStalls() << "/"
                 << branch->getTotalStalls() << ")\n";
            for (Vehicle *vehicle : branch->getVehicles()) {
                cout << "  " << vehicle->getId() << " | " << vehicle->describe() << " | "
                     << vehicleStatusToString(vehicle->getStatus()) << "\n";
            }
        }
    }

    void printVehicleLog(const string &vehicleId) const {
        auto it = vehicleLogs_.find(vehicleId);
        if (it != vehicleLogs_.end()) {
            it->second.print(vehicleId);
        }
    }

    int getCustomerRentedCount(const string &customerId) {
        return getCustomerOrThrow(customerId)->getRentedCount();
    }

private:
    unordered_map<string, Customer *> customers_;
    unordered_map<string, Receptionist *> receptionists_;
    vector<Branch *> branches_;
    unordered_map<string, Booking> bookings_;
    unordered_map<string, VehicleLog> vehicleLogs_;  // requirement #7
    vector<IRentalObserver *> observers_;            // requirement #10
    PricingService pricingService_;
    int bookingCounter_ = 0;

    void notifyOverdue(const string &bookingId, const string &customerName, int overdueDays, double fine) {
        for (IRentalObserver *observer : observers_) {
            observer->onOverdue(bookingId, customerName, overdueDays, fine);
        }
    }

    Customer *getCustomerOrThrow(const string &customerId) {
        auto it = customers_.find(customerId);
        if (it == customers_.end()) {
            throw runtime_error("Customer not found: " + customerId);
        }
        return it->second;
    }

    Branch *getBranchOrThrow(const string &branchId) {
        for (Branch *branch : branches_) {
            if (branch->getId() == branchId) {
                return branch;
            }
        }
        throw runtime_error("Branch not found: " + branchId);
    }

    Vehicle *findVehicleOrThrow(const string &vehicleId) {
        for (Branch *branch : branches_) {
            Vehicle *vehicle = branch->findVehicleById(vehicleId);
            if (vehicle) {
                return vehicle;
            }
        }
        throw runtime_error("Vehicle not found: " + vehicleId);
    }

    string findBranchIdForVehicle(const string &vehicleId) {
        for (Branch *branch : branches_) {
            if (branch->findVehicleById(vehicleId)) {
                return branch->getId();
            }
        }
        return "";
    }

    Booking *getBookingOrThrow(const string &bookingId) {
        auto it = bookings_.find(bookingId);
        if (it == bookings_.end()) {
            throw runtime_error("Booking not found: " + bookingId);
        }
        return &it->second;
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H
