#ifndef IRCTC_LLD_CORE_IRCTCSYSTEM_H
#define IRCTC_LLD_CORE_IRCTCSYSTEM_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/TicketBooking.h"
#include "../models/Train.h"
#include "../models/User.h"
#include "../services/BookingService.h"
#include "../services/SeatAllocationService.h"
#include "../services/SeatInventoryService.h"
#include "../services/TrainCatalogService.h"
#include "../services/TrainSearchService.h"

namespace irctc_lld {

class IRCTCSystem {
public:
    std::string registerUser(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("name required");
        }
        const std::string userId = "USR_" + std::to_string(++userCounter_);
        users_[userId] = User{userId, name};
        return userId;
    }

    void addTrainRun(const Train& train, int coachSeatCount) {
        catalog_.addTrain(train);
        inventory_.initializeCoach(train.runKey(), coachSeatCount);
    }

    std::vector<const Train*> searchByRoute(const std::string& source, const std::string& destination,
                                            const std::string& date) const {
        return TrainSearchService::searchByRoute(catalog_, source, destination, date);
    }

    const Train* searchByTrainNumber(const std::string& trainNumber, const std::string& date) const {
        return TrainSearchService::searchByTrainNumber(catalog_, trainNumber, date);
    }

    int getAvailableSeatCount(const std::string& trainNumber, const std::string& date,
                              const std::string& source, const std::string& destination) const {
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        const std::string runKey = train.runKey();
        return SeatAllocationService::countAvailableSeats(
            train, inventory_.getSeats(runKey), bookingService_.getLedgerForRun(runKey), source,
            destination);
    }

    std::vector<std::string> listAvailableSeats(const std::string& trainNumber,
                                                const std::string& date, const std::string& source,
                                                const std::string& destination) const {
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        const std::string runKey = train.runKey();
        return SeatAllocationService::listAvailableSeatIds(
            train, inventory_.getSeats(runKey), bookingService_.getLedgerForRun(runKey), source,
            destination);
    }

    TicketBooking bookTicket(const std::string& userId, const std::string& trainNumber,
                             const std::string& date, const std::string& source,
                             const std::string& destination,
                             const std::string& preferredSeatId = "") {
        validateUser(userId);
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        return bookingService_.bookTicket(train, userId, source, destination, preferredSeatId,
                                          bookingCounter_);
    }

    void cancelTicket(const std::string& bookingId, const std::string& userId) {
        validateUser(userId);
        bookingService_.cancelTicket(bookingId, userId);
    }

    const TicketBooking& getBooking(const std::string& bookingId) const {
        return bookingService_.getBooking(bookingId);
    }

private:
    TrainCatalogService catalog_;
    SeatInventoryService inventory_;
    BookingService bookingService_{&inventory_};

    std::unordered_map<std::string, User> users_;
    int userCounter_{0};
    int bookingCounter_{0};

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_CORE_IRCTCSYSTEM_H
