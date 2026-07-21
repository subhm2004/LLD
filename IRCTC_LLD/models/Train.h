// ============================================================================
//  models/Train.h  —  Ek train run (ek din ki ek trip) + uska route
// ----------------------------------------------------------------------------
//  Ek train ka number, naam, chalne ki date, aur ROUTE (stations ki ordered list).
//  Route ordered hai — yahi segment-based seat allocation ka poora aadhaar hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STATIONS ORDERED HAIN -> har station ka ek INDEX hota hai            │
//  │                                                                          │
//  │     stations = [BOM, ST, BRC, RTM, KOTA, NDLS]                            │
//  │     index      =  0    1    2    3     4     5                            │
//  │                                                                          │
//  │  Ek journey = do index ke beech ka SEGMENT: BOM->BRC = [0, 2).           │
//  │  Isi index se hi "do bookings overlap karti hain ya nahi" decide hota     │
//  │  hai (SeatAllocationService + SegmentUtils). Isi liye station -> index    │
//  │  conversion (`stationIndex`) is model ka core helper hai.                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `runKey()` = trainNumber + "|" + runDate — ek train ki ek DIN ki trip ki
//     unique pehchaan. Kyun? Kyunki same train alag-alag dino chalti hai, aur
//     har din ki apni seats/bookings hoti hain. "12951|2026-05-25" ek run.
//     Saare maps (inventory, ledger, mutex) isi runKey se keyed hain.
//
//  📌 `servesSegment` — ye train diye source->destination ko cover karti hai kya
//     (aur sahi DIRECTION me: to > from)? Reverse journey alag train number se
//     hoti hai (problem statement assumption), isliye backward segment invalid.
// ============================================================================
#ifndef IRCTC_LLD_MODELS_TRAIN_H
#define IRCTC_LLD_MODELS_TRAIN_H

#include <string>
#include <vector>

namespace irctc_lld {

struct Train {
    std::string trainNumber;            // "12951"
    std::string name;                   // "Mumbai Rajdhani"
    std::string runDate;                // "2026-05-25"
    std::vector<std::string> stations;  // ORDERED route: [BOM, ST, BRC, ...]

    // Station code -> uska index (position) route me. Nahi mila -> -1.
    int stationIndex(const std::string& stationCode) const {
        for (size_t i = 0; i < stations.size(); ++i) {
            if (stations[i] == stationCode) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // Ye train source->destination cover karti hai? (aur forward direction me?)
    // to > from -> destination source ke BAAD aana chahiye (aage ki taraf).
    bool servesSegment(const std::string& source, const std::string& destination) const {
        const int from = stationIndex(source);
        const int to = stationIndex(destination);
        return from >= 0 && to > from;
    }

    // Ek din ki ek trip ki unique key (saare per-run data isi se keyed hai).
    std::string runKey() const { return trainNumber + "|" + runDate; }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_TRAIN_H
