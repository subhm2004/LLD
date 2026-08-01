// ============================================================================
//  models/Booking.h — RESERVATION RECORD (requirements #4/#6/#8/#9/#10)
// ----------------------------------------------------------------------------
//  Ek booking = poora reservation record. Pehle sirf basic fields the; ab
//  requirements ke hisaab se enhance:
//    #4 Reservation record  -> kaun (customerId), kya (vehicleId), kab (issuedDay)
//    #8 Equipment           -> addOns_ me EQUIPMENT category ke add-ons
//    #9 Services            -> addOns_ me SERVICE category ke add-ons
//    #10 Overdue/fine       -> dueDay_ + fine_ (late return pe)
//    #6 Cancellation        -> status CANCELLED ho sakta
//
//  totalAmount_ = base rental + saare add-ons (Decorator se calculate hoke
//  aata hai). fine_ return ke waqt add hota agar overdue ho.
//  issuedDay_ = booking kab bani (requirement #4 "date it was issued").
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H

#include <bits/stdc++.h>

#include "../enums/BookingStatus.h"
#include "AddOn.h"

using namespace std;

namespace car_rental_lld {

class Booking {
public:
    Booking(string bookingId, string customerId, string vehicleId, string branchId,
            int startDay, int endDay, int issuedDay)
        : bookingId_(std::move(bookingId)),
          customerId_(std::move(customerId)),
          vehicleId_(std::move(vehicleId)),
          branchId_(std::move(branchId)),
          startDay_(startDay),
          endDay_(endDay),
          issuedDay_(issuedDay),
          dueDay_(endDay),          // by default due date = end day
          totalAmount_(0.0),
          fine_(0.0),
          status_(BookingStatus::CREATED) {}

    const string &getId() const { return bookingId_; }
    const string &getCustomerId() const { return customerId_; }
    const string &getVehicleId() const { return vehicleId_; }
    const string &getBranchId() const { return branchId_; }
    int getStartDay() const { return startDay_; }
    int getEndDay() const { return endDay_; }
    int getIssuedDay() const { return issuedDay_; }  // requirement #4
    int getDueDay() const { return dueDay_; }
    double getTotalAmount() const { return totalAmount_; }
    double getFine() const { return fine_; }
    BookingStatus getStatus() const { return status_; }

    void setStatus(BookingStatus status) { status_ = status; }
    void setTotalAmount(double amount) { totalAmount_ = amount; }
    void setFine(double fine) { fine_ = fine; }

    // Add-ons (requirement #8/#9) — equipment ya service
    void addAddOn(const AddOn &addOn) { addOns_.push_back(addOn); }
    const vector<AddOn> &getAddOns() const { return addOns_; }

    // Rental duration (days) — pricing/fine ke liye
    int getDays() const { return endDay_ - startDay_ + 1; }

private:
    string bookingId_;
    string customerId_;
    string vehicleId_;
    string branchId_;
    int startDay_;
    int endDay_;
    int issuedDay_;          // requirement #4 — kab issue hui
    int dueDay_;             // requirement #10 — kab tak return karni thi
    double totalAmount_;     // base + add-ons
    double fine_;            // requirement #10 — overdue fine
    vector<AddOn> addOns_;   // requirement #8/#9 — equipment + services
    BookingStatus status_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H
