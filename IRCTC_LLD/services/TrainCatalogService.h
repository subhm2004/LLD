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
    void addTrain(const Train& train) {
        const std::string key = train.runKey();
        if (trainsByRunKey_.count(key) != 0) {
            throw std::runtime_error("train run already exists: " + key);
        }
        trainsByRunKey_[key] = train;
        runsByDate_[train.runDate].push_back(key);
    }

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

    std::vector<const Train*> listRunsOnDate(const std::string& date) const {
        std::vector<const Train*> result;
        auto it = runsByDate_.find(date);
        if (it == runsByDate_.end()) {
            return result;
        }
        for (const std::string& key : it->second) {
            result.push_back(&trainsByRunKey_.at(key));
        }
        return result;
    }

private:
    std::unordered_map<std::string, Train> trainsByRunKey_;
    std::unordered_map<std::string, std::vector<std::string>> runsByDate_;
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_TRAINCATALOGSERVICE_H
