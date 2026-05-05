#ifndef CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H
#define CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H

#include <bits/stdc++.h>

#include "../enums/BookingStatus.h"
#include "../enums/VehicleStatus.h"
#include "../models/Booking.h"
#include "../models/Customer.h"
#include "../models/Vehicle.h"
#include "../services/PricingService.h"

using namespace std;

namespace car_rental_lld {

class CarRentalSystem {
public:
    void registerCustomer(const Customer &customer) {
        customers_.insert_or_assign(customer.getId(), customer);
    }

    void addVehicle(const Vehicle &vehicle) {
        vehicles_.insert_or_assign(vehicle.getId(), vehicle);
    }

    string createBooking(const string &customerId, const string &vehicleId, int startDay, int endDay) {
        Customer *customer = getCustomerOrThrow(customerId);
        Vehicle *vehicle = getVehicleOrThrow(vehicleId);

        if (vehicle->getStatus() != VehicleStatus::AVAILABLE) {
            throw runtime_error("Vehicle not available");
        }

        const double amount = pricingService_.calculateRentAmount(vehicle->getDailyRate(), startDay, endDay);
        const string bookingId = "BOOK_" + to_string(++bookingCounter_);

        Booking booking(bookingId, customer->getId(), vehicle->getId(), startDay, endDay, amount);
        booking.setStatus(BookingStatus::CONFIRMED);
        bookings_.insert_or_assign(bookingId, booking);

        vehicle->setStatus(VehicleStatus::RESERVED);
        cout << "Booking confirmed: " << bookingId << " | Customer: " << customer->getName() << " | Vehicle: "
             << vehicle->getModelName() << " | Amount: " << amount << "\n";
        return bookingId;
    }

    void pickupVehicle(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        Vehicle *vehicle = getVehicleOrThrow(booking->getVehicleId());

        if (booking->getStatus() != BookingStatus::CONFIRMED) {
            throw runtime_error("Booking is not ready for pickup");
        }

        booking->setStatus(BookingStatus::ACTIVE);
        vehicle->setStatus(VehicleStatus::RENTED);
        cout << "Pickup done for booking: " << bookingId << "\n";
    }

    void returnVehicle(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        Vehicle *vehicle = getVehicleOrThrow(booking->getVehicleId());

        if (booking->getStatus() != BookingStatus::ACTIVE) {
            throw runtime_error("Booking is not active");
        }

        booking->setStatus(BookingStatus::COMPLETED);
        vehicle->setStatus(VehicleStatus::AVAILABLE);
        cout << "Return completed for booking: " << bookingId << "\n";
    }

    void printVehicleInventory() const {
        cout << "\n=== Vehicle Inventory ===\n";
        for (const auto &entry : vehicles_) {
            const Vehicle &vehicle = entry.second;
            cout << vehicle.getId() << " | " << vehicle.getModelName() << " | Rate/day: " << vehicle.getDailyRate()
                 << " | Status: " << statusToString(vehicle.getStatus()) << "\n";
        }
    }

private:
    unordered_map<string, Customer> customers_;
    unordered_map<string, Vehicle> vehicles_;
    unordered_map<string, Booking> bookings_;
    PricingService pricingService_;
    int bookingCounter_ = 0;

    static string statusToString(VehicleStatus status) {
        if (status == VehicleStatus::AVAILABLE) {
            return "AVAILABLE";
        }
        if (status == VehicleStatus::RESERVED) {
            return "RESERVED";
        }
        if (status == VehicleStatus::RENTED) {
            return "RENTED";
        }
        return "MAINTENANCE";
    }

    Customer *getCustomerOrThrow(const string &customerId) {
        auto it = customers_.find(customerId);
        if (it == customers_.end()) {
            throw runtime_error("Customer not found");
        }
        return &it->second;
    }

    Vehicle *getVehicleOrThrow(const string &vehicleId) {
        auto it = vehicles_.find(vehicleId);
        if (it == vehicles_.end()) {
            throw runtime_error("Vehicle not found");
        }
        return &it->second;
    }

    Booking *getBookingOrThrow(const string &bookingId) {
        auto it = bookings_.find(bookingId);
        if (it == bookings_.end()) {
            throw runtime_error("Booking not found");
        }
        return &it->second;
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_CORE_CARRENTALSYSTEM_H
