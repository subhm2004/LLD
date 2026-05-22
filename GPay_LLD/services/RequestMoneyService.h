#ifndef GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H
#define GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/MoneyRequest.h"
#include "../models/User.h"

namespace gpay_lld {

class RequestMoneyService {
public:
    std::string createRequest(const User& requester, const User& payer, double amount,
                              const std::string& note, int& requestCounter) {
        if (requester.getUserId() == payer.getUserId()) {
            throw std::runtime_error("cannot request money from self");
        }
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }
        const std::string requestId = "REQ_" + std::to_string(++requestCounter);
        requests_.emplace(requestId,
                          MoneyRequest(requestId, requester.getUserId(), payer.getUserId(),
                                       amount, note));
        return requestId;
    }

    MoneyRequest& getRequestOrThrow(const std::string& requestId) {
        auto it = requests_.find(requestId);
        if (it == requests_.end()) {
            throw std::runtime_error("money request not found");
        }
        return it->second;
    }

    void markFulfilled(const std::string& requestId) {
        MoneyRequest& request = getRequestOrThrow(requestId);
        if (!request.isPending()) {
            throw std::runtime_error("request already processed");
        }
        request.markFulfilled();
    }

private:
    std::unordered_map<std::string, MoneyRequest> requests_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H
