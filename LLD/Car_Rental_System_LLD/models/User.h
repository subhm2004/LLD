// ============================================================================
//  models/User.h — USER hierarchy (requirement #1) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #1: 2 tarah ke users — Customer (rent karta) aur Receptionist
//  (rental process manage karta). Dono User base ko extend karte hain —
//  INHERITANCE se common cheezein (id, name) share, alag cheezein subclass me.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │              User (base — id, name, role)                               │
//  │                  △                                                       │
//  │       ┌──────────┴──────────┐                                          │
//  │   Customer                Receptionist                                  │
//  │   (license +              (employeeId +                                 │
//  │    rentedCount ← req #5)   manage kar sakta)                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ RENTAL TRACKING (requirement #5): Customer ke paas rentedCount_ hai —
//  usne kitne vehicles rent kiye. System booking pe ye badhata/ghatata hai.
//  "Ek customer ne kitni gaadiyan li" ka jawab yahi field deta.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_USER_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_USER_H

#include <bits/stdc++.h>

#include "../enums/UserRole.h"

using namespace std;

namespace car_rental_lld {

// -------------------- Base User --------------------
class User {
public:
    User(string id, string name, UserRole role)
        : id_(std::move(id)), name_(std::move(name)), role_(role) {}
    virtual ~User() = default;

    const string &getId() const { return id_; }
    const string &getName() const { return name_; }
    UserRole getRole() const { return role_; }

protected:
    string id_;
    string name_;
    UserRole role_;
};

// -------------------- Customer (rent karta) --------------------
class Customer : public User {
public:
    Customer(string id, string name, string licenseNumber)
        : User(std::move(id), std::move(name), UserRole::CUSTOMER),
          licenseNumber_(std::move(licenseNumber)),
          rentedCount_(0) {}

    const string &getLicenseNumber() const { return licenseNumber_; }

    // REQUIREMENT #5 — rental tracking: kitne vehicles rent kiye
    int getRentedCount() const { return rentedCount_; }
    void incrementRentedCount() { ++rentedCount_; }
    void decrementRentedCount() {
        if (rentedCount_ > 0) --rentedCount_;
    }

private:
    string licenseNumber_;
    int rentedCount_;  // active rentals ka count
};

// -------------------- Receptionist (staff — process manage karta) --------------------
class Receptionist : public User {
public:
    Receptionist(string id, string name, string employeeId)
        : User(std::move(id), std::move(name), UserRole::RECEPTIONIST),
          employeeId_(std::move(employeeId)) {}

    const string &getEmployeeId() const { return employeeId_; }

private:
    string employeeId_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_USER_H
