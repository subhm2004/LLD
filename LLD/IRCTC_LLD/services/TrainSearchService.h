// ============================================================================
//  services/TrainSearchService.h  —  Trains dhoondho (route ya number se)
// ----------------------------------------------------------------------------
//  Do search modes (FR-1, FR-2):
//     searchByRoute(src, dest, date)  -> us din ki wo trains jo ye segment cover
//                                        karti hain (forward direction me)
//     searchByTrainNumber(num, date)  -> ek specific train run (ya nullptr)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye service CATALOG ke UPAR ek patli "query layer" hai (SRP)          │
//  │                                                                          │
//  │  Catalog "storage" hai (trains rakho, id/date se do). Search "logic" hai  │
//  │  (route filter, direction check). Alag rakhne se: search algorithm badle  │
//  │  (jaise fastest-route, connecting trains) to catalog untouched.           │
//  │                                                                          │
//  │  searchByRoute internally: catalog se us din ki saari trains lo, phir      │
//  │  har ek pe `servesSegment(src, dest)` check (Train.h) — jo forward         │
//  │  direction bhi verify karta hai (dest, src ke baad aana chahiye).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `static` methods (stateless) — catalog reference le ke query karti hai,
//     khud kuch store nahi karti.
// ============================================================================
#ifndef IRCTC_LLD_SERVICES_TRAINSEARCHSERVICE_H
#define IRCTC_LLD_SERVICES_TRAINSEARCHSERVICE_H

#include <string>
#include <vector>

#include "../models/Train.h"
#include "TrainCatalogService.h"

namespace irctc_lld {

class TrainSearchService {
public:
    // FR-1: source->destination (forward) cover karne wali trains us date par.
    static std::vector<const Train*> searchByRoute(const TrainCatalogService& catalog,
                                                   const std::string& source,
                                                   const std::string& destination,
                                                   const std::string& date) {
        std::vector<const Train*> result;
        for (const Train* train : catalog.listRunsOnDate(date)) {
            if (train->servesSegment(source, destination)) { // route + direction check
                result.push_back(train);
            }
        }
        return result;
    }

    // FR-2: ek specific train number+date. Nahi mila -> nullptr.
    static const Train* searchByTrainNumber(const TrainCatalogService& catalog,
                                            const std::string& trainNumber, const std::string& date) {
        if (!catalog.hasTrainRun(trainNumber, date)) {
            return nullptr;
        }
        return &catalog.getTrainRun(trainNumber, date);
    }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_TRAINSEARCHSERVICE_H
