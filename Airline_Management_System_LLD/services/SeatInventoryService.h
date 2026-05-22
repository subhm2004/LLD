#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_SEATINVENTORYSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_SEATINVENTORYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/SeatClass.h"
#include "../enums/SeatStatus.h"
#include "../models/Aircraft.h"
#include "../models/Seat.h"

namespace airline_mgmt {

class SeatInventoryService {
public:
    static void clearSeatsForFlight(const std::string &flightId, std::unordered_map<std::string, Seat> &seats) {
        for (auto it = seats.begin(); it != seats.end();) {
            if (it->second.flightId == flightId) {
                it = seats.erase(it);
            } else {
                ++it;
            }
        }
    }

    static void generateSeatsForFlight(const Aircraft &aircraft, const std::string &flightId,
                                       std::unordered_map<std::string, Seat> &seats) {
        clearSeatsForFlight(flightId, seats);
        int row = 1;
        for (int i = 0; i < aircraft.economySeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "E" + std::to_string(seatRow), seatRow, 1, SeatClass::ECONOMY, 3500.0);
        }
        for (int i = 0; i < aircraft.premiumEconomySeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "P" + std::to_string(seatRow), seatRow, 1, SeatClass::PREMIUM_ECONOMY, 5500.0);
        }
        for (int i = 0; i < aircraft.businessSeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "B" + std::to_string(seatRow), seatRow, 1, SeatClass::BUSINESS, 12000.0);
        }
    }

    static std::vector<const Seat *> getAvailableSeats(const std::string &flightId,
                                                       const std::unordered_map<std::string, Seat> &seats) {
        std::vector<const Seat *> result;
        for (const auto &entry : seats) {
            if (entry.second.flightId == flightId && entry.second.status == SeatStatus::AVAILABLE) {
                result.push_back(&entry.second);
            }
        }
        return result;
    }

    static Seat &getSeatOrThrow(const std::string &seatId, std::unordered_map<std::string, Seat> &seats) {
        auto it = seats.find(seatId);
        if (it == seats.end()) {
            throw std::runtime_error("Seat not found: " + seatId);
        }
        return it->second;
    }

private:
    static void addSeat(std::unordered_map<std::string, Seat> &seats, const std::string &flightId,
                        const std::string &suffix, int row, int col, SeatClass seatClass, double basePrice) {
        const std::string seatId = flightId + "-" + suffix;
        if (seats.count(seatId)) {
            throw std::runtime_error("Duplicate seat id: " + seatId);
        }
        seats.emplace(seatId, Seat{seatId, flightId, row, col, seatClass, SeatStatus::AVAILABLE, basePrice});
    }
};

} // namespace airline_mgmt

#endif
