// ============================================================================
//  main.cpp — CAR RENTAL SYSTEM demo (saare 13 requirements exercise)
// ----------------------------------------------------------------------------
//  Ye demo image wale SAARE 13 requirements dikhata hai:
//    users (customer+receptionist) -> branches + parking stalls ->
//    vehicles (types + subtypes via factory) -> search (type/model) ->
//    booking with add-ons (equipment+services, decorator cost) -> cancel ->
//    pickup -> return (on-time + overdue with fine + notification) ->
//    rental tracking + vehicle log.
//
//  Date integer "day number" se represent hoti (assumption).
// ============================================================================
#include <bits/stdc++.h>

#include "core/CarRentalSystem.h"
#include "observers/CustomerNotifier.h"

using namespace std;
using namespace car_rental_lld;

int main() {
    CarRentalSystem system;

    // ---- REQUIREMENT #1: Users (customer + receptionist) ----
    cout << "===== Users =====\n";
    system.registerCustomer(new Customer("C1", "Shubham", "DL123456"));
    system.registerCustomer(new Customer("C2", "Ananya", "DL789012"));
    system.registerReceptionist(new Receptionist("R1", "Priya", "EMP-01"));

    // ---- REQUIREMENT #10: Observer (overdue notifier) ----
    CustomerNotifier notifier;
    system.addObserver(&notifier);

    // ---- REQUIREMENT #12/#13: Branches with parking stalls ----
    cout << "\n===== Branches =====\n";
    system.addBranch(new Branch("B1", "Delhi Central", "New Delhi", 10));
    system.addBranch(new Branch("B2", "Mumbai West", "Mumbai", 8));

    // ---- REQUIREMENT #2/#3: Vehicles (types + subtypes via FACTORY) ----
    cout << "\n===== Vehicles (Factory) =====\n";
    system.addVehicle("B1", VehicleType::CAR, "LUXURY", "V1", "Mercedes E-Class", 5000.0);
    system.addVehicle("B1", VehicleType::CAR, "ECONOMY", "V2", "Hyundai i20", 1800.0);
    system.addVehicle("B1", VehicleType::VAN, "CARGO", "V3", "Tata Ace", 2200.0);
    system.addVehicle("B2", VehicleType::MOTORCYCLE, "SPORTS", "V4", "Yamaha R15", 900.0);
    system.addVehicle("B2", VehicleType::TRUCK, "HIGH_DUTY", "V5", "Ashok Leyland", 6000.0);

    system.printInventory();

    // ---- REQUIREMENT #11: Search (type / model) ----
    cout << "\n===== Search =====\n";
    cout << "Cars available:\n";
    for (Vehicle *v : system.searchByType(VehicleType::CAR)) {
        cout << "  - " << v->describe() << "\n";
    }
    cout << "Model search 'yamaha':\n";
    for (Vehicle *v : system.searchByModel("yamaha")) {
        cout << "  - " << v->describe() << "\n";
    }

    // ---- REQUIREMENT #4/#8/#9: Booking with equipment + services (Decorator) ----
    cout << "\n===== Booking with add-ons =====\n";
    vector<AddOn> addOns = {
        AddOn("GPS Navigation", 300.0, AddOnCategory::EQUIPMENT),   // req #8
        AddOn("Child Seat", 150.0, AddOnCategory::EQUIPMENT),       // req #8
        AddOn("Driver", 1500.0, AddOnCategory::SERVICE),           // req #9
    };
    // Mercedes 3 days (day 1-3), issued day 1, with add-ons
    string b1 = system.createBooking("C1", "V1", 1, 3, /*issuedDay=*/1, addOns);

    // ---- REQUIREMENT #6: Cancellation ----
    cout << "\n===== Cancellation =====\n";
    string b2 = system.createBooking("C2", "V2", 2, 4, 2);  // Ananya books i20
    system.cancelBooking(b2);  // phir cancel kar deti

    // ---- Pickup + Return (on-time) ----
    cout << "\n===== Pickup & Return (on-time) =====\n";
    system.pickupVehicle(b1, 1);
    system.returnVehicle(b1, 3);  // due day 3, returned day 3 -> on time

    // ---- REQUIREMENT #10: Overdue return + fine + notification ----
    cout << "\n===== Overdue return (fine + notification) =====\n";
    string b3 = system.createBooking("C2", "V5", 5, 7, 5);  // Truck, due day 7
    system.pickupVehicle(b3, 5);
    system.returnVehicle(b3, 10);  // returned day 10 -> 3 days late -> fine!

    // ---- REQUIREMENT #5: Rental tracking ----
    cout << "\n===== Rental tracking =====\n";
    cout << "Shubham (C1) active rentals: " << system.getCustomerRentedCount("C1") << "\n";
    cout << "Ananya (C2) active rentals: " << system.getCustomerRentedCount("C2") << "\n";

    // ---- REQUIREMENT #7: Vehicle log ----
    cout << "\n===== Vehicle logs =====\n";
    system.printVehicleLog("V1");
    system.printVehicleLog("V5");

    return 0;
}
