#include <bits/stdc++.h>

#include "core/ParkingLot.h"
#include "models/Vehicle.h"
#include "strategies/HourlyPricingStrategy.h"

using namespace std;
using namespace parking_lot_lld;

int main() {
    ParkingLot *lot = new ParkingLot(new HourlyPricingStrategy());
    lot->addSpot(1, VehicleType::BIKE);
    lot->addSpot(2, VehicleType::BIKE);
    lot->addSpot(3, VehicleType::CAR);
    lot->addSpot(4, VehicleType::CAR);
    lot->addSpot(5, VehicleType::TRUCK);

    Vehicle bike1("BIKE-101", VehicleType::BIKE);
    Vehicle car1("CAR-301", VehicleType::CAR);
    Vehicle truck1("TRUCK-801", VehicleType::TRUCK);

    lot->printAvailability();

    string bikeTicket = lot->parkVehicle(bike1);
    string carTicket = lot->parkVehicle(car1);
    string truckTicket = lot->parkVehicle(truck1);

    lot->printAvailability();

    lot->unparkVehicle(carTicket);
    lot->unparkVehicle(bikeTicket);
    lot->unparkVehicle(truckTicket);

    lot->printAvailability();
    delete lot;
    return 0;
}
