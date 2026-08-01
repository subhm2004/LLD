// ============================================================================
//  observers/IParkingObserver.h — OBSERVER interface (requirement #5/#7) [NAYA]
// ----------------------------------------------------------------------------
//  Observer Pattern (L12) ka contract. ParkingLot (subject/observable) jab bhi
//  koi vehicle park/unpark hota hai, to apne saare observers ko NOTIFY karta.
//  DisplayBoard is interface ko implement karta — automatically update ho jaata.
//
//  Fayda: ParkingLot ko ye nahi jaanna ki display kaise update ho — bas
//  "kuch badla" bol deta hai (notify). Naya observer (jaise mobile app,
//  entrance LED) add karna ho to bas is interface ko implement karo,
//  ParkingLot untouched. Yahi Observer ka loose coupling.
//
//  onParkingUpdate() ko do cheezein milti hain:
//    availability -> har SpotType ke available spots ka count (map)
//    isFull       -> lot poora bhar gaya kya (requirement #6/#7 ke liye)
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_OBSERVERS_IPARKINGOBSERVER_H
#define PARKING_LOT_SYSTEM_LLD_OBSERVERS_IPARKINGOBSERVER_H

#include <bits/stdc++.h>

#include "../enums/SpotType.h"

using namespace std;

namespace parking_lot_lld {

class IParkingObserver {
public:
    virtual ~IParkingObserver() = default;

    // ParkingLot koi bhi change pe ye call karta hai
    virtual void onParkingUpdate(const map<SpotType, int> &availability, bool isFull) = 0;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_OBSERVERS_IPARKINGOBSERVER_H
