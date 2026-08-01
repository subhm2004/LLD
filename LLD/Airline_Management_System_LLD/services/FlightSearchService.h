// ============================================================================
//  services/FlightSearchService.h  —  Flights dhoondho (Requirement #1)
// ----------------------------------------------------------------------------
//  "Search flights by source, destination, and date."
//
//  Ye service ka ek hi kaam: diye gaye source + destination + day pe matching
//  SCHEDULED flights laut do.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Sirf `SCHEDULED` flights hi search me aati hain                       │
//  │                                                                          │
//  │  DELAYED / CANCELLED / DEPARTED flights user ko dikhani nahi (unpe naya   │
//  │  booking nahi khul sakta). Isi liye filter me `status == SCHEDULED` bhi   │
//  │  hai. User ko sirf wahi flights dikhti hain jo book ho sakti hain.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SAB METHODS `static` — ye ek STATELESS service hai
//     Is class ka koi member/state nahi hai. Ye bas diya gaya data (flights map)
//     leti hai aur natija deti hai — ek "pure function" jaisi. Isi liye object
//     banane ki zaroorat nahi (`FlightSearchService::search(...)` seedha). Poore
//     services folder me yahi consistent design hai (SRP + testable).
//
//  💡 `const Flight*` (pointer) return karte hain — matched flights ki COPY
//     nahi banti, sirf unka pata (address). `const` isliye ki caller search
//     result se flight ko galti se badal na de (read-only view).
//
//  ⚠ Ye linear scan (O(N) over all flights) hai. Real system me source/dest/day
//     pe index (ya DB query) hota. Demo scale (kuch flights) pe bilkul theek.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_FLIGHTSEARCHSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_FLIGHTSEARCHSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/FlightStatus.h"
#include "../models/Flight.h"

namespace airline_mgmt {

class FlightSearchService {
public:
    static std::vector<const Flight *> search(const std::unordered_map<std::string, Flight> &flights,
                                              const std::string &source, const std::string &destination,
                                              int departureDay) {
        std::vector<const Flight *> result;
        for (const auto &entry : flights) {
            const Flight &f = entry.second;
            // Chaaron condition match honi chahiye: source, destination, day, aur bookable status
            if (f.sourceAirport == source && f.destinationAirport == destination && f.departureDay == departureDay &&
                f.status == FlightStatus::SCHEDULED) {
                result.push_back(&entry.second); // asli flight ka pata daalo (copy nahi)
            }
        }
        return result;
    }
};

} // namespace airline_mgmt

#endif
