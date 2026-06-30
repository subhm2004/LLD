// models/ATMCashInventory.h — ATM me har denomination ke kitne notes hain, iska
// hisaab (cassettes ka collection). Handlers yahin se notes nikalte/ghatate hain.
#ifndef COR_ATM_MODELS_ATM_CASH_INVENTORY_H
#define COR_ATM_MODELS_ATM_CASH_INVENTORY_H

#include <functional>
#include <unordered_map>

#include "CashCassette.h"

namespace cor_atm {

struct NoteDenominationHash {
    std::size_t operator()(NoteDenomination denomination) const noexcept {
        return std::hash<int>{}(denominationValue(denomination));
    }
};

class ATMCashInventory {
public:
    void addCassette(NoteDenomination denomination, int numNotes) {
        cassettes_.insert_or_assign(denomination, CashCassette(denomination, numNotes));
    }

    int getNumNotes(NoteDenomination denomination) const {
        auto it = cassettes_.find(denomination);
        if (it == cassettes_.end()) {
            return 0;
        }
        return it->second.getNumNotes();
    }

private:
    std::unordered_map<NoteDenomination, CashCassette, NoteDenominationHash> cassettes_;
};

} // namespace cor_atm

#endif // COR_ATM_MODELS_ATM_CASH_INVENTORY_H
