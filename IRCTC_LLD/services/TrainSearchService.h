#ifndef IRCTC_LLD_SERVICES_TRAINSEARCHSERVICE_H
#define IRCTC_LLD_SERVICES_TRAINSEARCHSERVICE_H

#include <vector>

#include "../models/Train.h"
#include "TrainCatalogService.h"

namespace irctc_lld {

class TrainSearchService {
public:
    static std::vector<const Train*> searchByRoute(const TrainCatalogService& catalog,
                                                   const std::string& source,
                                                   const std::string& destination,
                                                   const std::string& date) {
        std::vector<const Train*> result;
        for (const Train* train : catalog.listRunsOnDate(date)) {
            if (train->servesSegment(source, destination)) {
                result.push_back(train);
            }
        }
        return result;
    }

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
