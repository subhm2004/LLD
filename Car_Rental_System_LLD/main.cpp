#include <bits/stdc++.h>

#include "core/CarRentalSystem.h"

using namespace std;
using namespace car_rental_lld;

int main() {
  CarRentalSystem *system = new CarRentalSystem()   ;

  system->registerCustomer(Customer("C1", "Shubham", "DL123456"));
  system->registerCustomer(Customer("C2", "Ananya", "DL789012"));

  system->addVehicle(Vehicle("V1", "Hyundai i20", 1800.0));
  system->addVehicle(Vehicle("V2", "Honda City", 2500.0));
  system->addVehicle(Vehicle("V3", "Mahindra XUV700", 4200.0));

  system->printVehicleInventory();

  string bookingId = system->createBooking("C1", "V2", 1, 3);
  system->pickupVehicle(bookingId);
  system->printVehicleInventory();

  system->returnVehicle(bookingId);
  system->printVehicleInventory();

  delete system;
  return 0;
}
