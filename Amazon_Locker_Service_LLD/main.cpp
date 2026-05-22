#include <bits/stdc++.h>

#include "core/AmazonLockerService.h"
#include "models/LockerCompartment.h"
#include "models/LockerStation.h"

using namespace std;
using namespace amazon_locker_lld;

static LockerStation buildDemoStation() {
    LockerStation station("STN_BLR_01", "Koramangala 4th Block, Bangalore");
    station.addCompartment(LockerCompartment("C-S1", LockerSize::SMALL));
    station.addCompartment(LockerCompartment("C-S2", LockerSize::SMALL));
    station.addCompartment(LockerCompartment("C-M1", LockerSize::MEDIUM));
    station.addCompartment(LockerCompartment("C-M2", LockerSize::MEDIUM));
    station.addCompartment(LockerCompartment("C-L1", LockerSize::LARGE));
    return station;
}

int main() {
    AmazonLockerService service;
    service.registerStation(buildDemoStation());

    const string stationId = "STN_BLR_01";

    cout << "=== Amazon Locker Service LLD Demo ===\n\n";

    cout << "--- Initial availability ---\n";
    service.printStationAvailability(stationId);

    cout << "\n--- Courier deposits package (MEDIUM) ---\n";
    const string pkg1 =
        service.depositPackage(stationId, "ORD-9001", "CUST_ALICE", LockerSize::MEDIUM);
    const string code1 = service.getPickupCodeForDemo(pkg1);

    cout << "\n--- Availability after deposit ---\n";
    service.printStationAvailability(stationId);

    cout << "\n--- Wrong pickup code ---\n";
    try {
        service.pickupPackage(stationId, "000000");
    } catch (const exception& ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Successful pickup ---\n";
    service.pickupPackage(stationId, code1);
    cout << "Package status: "
         << packageStatusToString(service.getPackage(pkg1).status()) << "\n";

    cout << "\n--- Re-use same code (should fail) ---\n";
    try {
        service.pickupPackage(stationId, code1);
    } catch (const exception& ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Availability after pickup ---\n";
    service.printStationAvailability(stationId);

    cout << "\n--- Deposit LARGE then SMALL (fit rules) ---\n";
    const string pkgLarge =
        service.depositPackage(stationId, "ORD-9002", "CUST_BOB", LockerSize::LARGE);
    const string codeLarge = service.getPickupCodeForDemo(pkgLarge);

    const string pkgSmall =
        service.depositPackage(stationId, "ORD-9003", "CUST_CARA", LockerSize::SMALL);
    const string codeSmall = service.getPickupCodeForDemo(pkgSmall);

    service.printStationAvailability(stationId);

    cout << "\n--- Fill remaining slots until full ---\n";
    service.depositPackage(stationId, "ORD-9004", "CUST_DAN", LockerSize::SMALL);
    service.depositPackage(stationId, "ORD-9005", "CUST_EVE", LockerSize::MEDIUM);
    service.depositPackage(stationId, "ORD-9006", "CUST_FIN", LockerSize::MEDIUM);
    service.printStationAvailability(stationId);

    cout << "\n--- No compartment left (all 5 slots full) ---\n";
    try {
        service.depositPackage(stationId, "ORD-9007", "CUST_GIA", LockerSize::SMALL);
    } catch (const exception& ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Pickup remaining packages ---\n";
    service.pickupPackage(stationId, codeLarge);
    service.pickupPackage(stationId, codeSmall);
    service.pickupPackage(stationId, service.getPickupCodeForDemo("PKG_4"));
    service.pickupPackage(stationId, service.getPickupCodeForDemo("PKG_5"));
    service.pickupPackage(stationId, service.getPickupCodeForDemo("PKG_6"));
    service.printStationAvailability(stationId);

    cout << "\nAmazon_Locker_Service_LLD demo completed.\n";
    return 0;
}
