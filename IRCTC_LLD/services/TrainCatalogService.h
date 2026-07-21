// ============================================================================
//  services/TrainCatalogService.h  —  Saari train runs ka registry (catalog)
// ----------------------------------------------------------------------------
//  Ye service train runs ko store aur lookup karti hai. Do index rakhti hai
//  taaki dono tarah ki query fast (O(1)) ho:
//     trainsByRunKey_ : "12951|2026-05-25" -> Train   (number+date se lookup)
//     runsByDate_     : "2026-05-25" -> [runKeys...]   (date pe saari trains)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO INDEX kyun? — do alag queries, dono O(1)                          │
//  │                                                                          │
//  │  FR-2 "search by train number + date" -> trainsByRunKey_ se seedha        │
//  │  FR-1 "search by route + date"         -> pehle runsByDate_ se us din ki  │
//  │                                           saari trains, phir route filter  │
//  │                                           (TrainSearchService karta hai)  │
//  │                                                                          │
//  │  Ek hi map se ek query fast hoti, doosri O(N) scan. Do index = dono fast  │
//  │  (thodi extra memory — classic space-for-speed trade-off).               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `listRunsOnDate` `const Train*` (pointers) return karta hai — trains ki
//     COPY nahi banti, sirf catalog ke andar wale asli Train objects ke pate.
//     `.at(key)` use kiya (not []) taaki const method me insert na ho.
//
//  📌 Duplicate run reject: same trainNumber+date dobara add -> error (ek din
//     ek hi trip — problem statement assumption).
// ============================================================================
#ifndef IRCTC_LLD_SERVICES_TRAINCATALOGSERVICE_H
#define IRCTC_LLD_SERVICES_TRAINCATALOGSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Train.h"

namespace irctc_lld {

class TrainCatalogService {
public:
    // Naya train run add karo (dono index update). Duplicate run -> error.
    void addTrain(const Train& train) {
        const std::string key = train.runKey();
        if (trainsByRunKey_.count(key) != 0) {
            throw std::runtime_error("train run already exists: " + key);
        }
        trainsByRunKey_[key] = train;                  // number+date index
        runsByDate_[train.runDate].push_back(key);     // date index
    }

    // Train number + date se ek run. Nahi mila -> error.
    const Train& getTrainRun(const std::string& trainNumber, const std::string& date) const {
        const std::string key = trainNumber + "|" + date;
        auto it = trainsByRunKey_.find(key);
        if (it == trainsByRunKey_.end()) {
            throw std::runtime_error("train run not found: " + key);
        }
        return it->second;
    }

    bool hasTrainRun(const std::string& trainNumber, const std::string& date) const {
        return trainsByRunKey_.count(trainNumber + "|" + date) != 0;
    }

    // Ek date par chalne wali saari trains (route-search isko filter karta hai).
    std::vector<const Train*> listRunsOnDate(const std::string& date) const {
        std::vector<const Train*> result;
        auto it = runsByDate_.find(date);
        if (it == runsByDate_.end()) {
            return result; // us din koi train nahi
        }
        for (const std::string& key : it->second) {
            result.push_back(&trainsByRunKey_.at(key)); // asli Train ka pata (copy nahi)
        }
        return result;
    }

private:
    std::unordered_map<std::string, Train> trainsByRunKey_;              // runKey -> Train
    std::unordered_map<std::string, std::vector<std::string>> runsByDate_; // date -> runKeys
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_TRAINCATALOGSERVICE_H
