#ifndef AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERSTATION_H
#define AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERSTATION_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/CompartmentStatus.h"
#include "../enums/LockerSize.h"
#include "LockerCompartment.h"

namespace amazon_locker_lld {

class LockerStation {
public:
    LockerStation(std::string stationId, std::string address)
        : stationId_(std::move(stationId)), address_(std::move(address)) {}

    const std::string& stationId() const { return stationId_; }
    const std::string& address() const { return address_; }

    void addCompartment(const LockerCompartment& compartment) {
        compartments_.push_back(compartment);
        compartmentIndex_[compartment.compartmentId()] = compartments_.size() - 1;
    }

    LockerCompartment& getCompartment(const std::string& compartmentId) {
        auto it = compartmentIndex_.find(compartmentId);
        if (it == compartmentIndex_.end()) {
            throw std::invalid_argument("Unknown compartment: " + compartmentId);
        }
        return compartments_.at(it->second);
    }

    const LockerCompartment& getCompartment(const std::string& compartmentId) const {
        auto it = compartmentIndex_.find(compartmentId);
        if (it == compartmentIndex_.end()) {
            throw std::invalid_argument("Unknown compartment: " + compartmentId);
        }
        return compartments_.at(it->second);
    }

    std::vector<LockerCompartment>& compartments() { return compartments_; }
    const std::vector<LockerCompartment>& compartments() const { return compartments_; }

    void printAvailability() const {
        std::cout << "Station " << stationId_ << " @ " << address_ << "\n";
        for (const auto& c : compartments_) {
            std::cout << "  " << c.compartmentId() << " | " << lockerSizeToString(c.size())
                      << " | " << compartmentStatusToString(c.status());
            if (!c.packageId().empty()) {
                std::cout << " | pkg=" << c.packageId();
            }
            std::cout << "\n";
        }
    }

private:
    std::string stationId_;
    std::string address_;
    std::vector<LockerCompartment> compartments_;
    std::unordered_map<std::string, size_t> compartmentIndex_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERSTATION_H
