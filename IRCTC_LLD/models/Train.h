#ifndef IRCTC_LLD_MODELS_TRAIN_H
#define IRCTC_LLD_MODELS_TRAIN_H

#include <stdexcept>
#include <string>
#include <vector>

namespace irctc_lld {

struct Train {
    std::string trainNumber;
    std::string name;
    std::string runDate;
    std::vector<std::string> stations;

    int stationIndex(const std::string& stationCode) const {
        for (size_t i = 0; i < stations.size(); ++i) {
            if (stations[i] == stationCode) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool servesSegment(const std::string& source, const std::string& destination) const {
        const int from = stationIndex(source);
        const int to = stationIndex(destination);
        return from >= 0 && to > from;
    }

    std::string runKey() const { return trainNumber + "|" + runDate; }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_TRAIN_H
