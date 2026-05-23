#ifndef IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H
#define IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace irctc_lld {

class SeatInventoryService {
public:
    void initializeCoach(const std::string& runKey, int seatCount) {
        if (seatCount < 1) {
            throw std::invalid_argument("seatCount must be >= 1");
        }
        std::vector<std::string> seats;
        seats.reserve(static_cast<size_t>(seatCount));
        for (int i = 1; i <= seatCount; ++i) {
            seats.push_back("S" + std::to_string(i));
        }
        seatsByRun_[runKey] = std::move(seats);
    }

    const std::vector<std::string>& getSeats(const std::string& runKey) const {
        auto it = seatsByRun_.find(runKey);
        if (it == seatsByRun_.end()) {
            throw std::runtime_error("seat inventory not found for run: " + runKey);
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> seatsByRun_;
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H
