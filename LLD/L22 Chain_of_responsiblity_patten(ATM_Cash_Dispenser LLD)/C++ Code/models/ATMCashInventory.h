// ============================================================================
//  models/ATMCashInventory.h — ATM ka POORA cash stock (cassettes ka collection)
// ----------------------------------------------------------------------------
//  Har denomination ka ek cassette (drawer) — inventory unka map rakhti hai
//  (denomination -> CashCassette). addCassette() se stock set, getNumNotes()
//  se kisi denomination ke available notes.
//
//  DETAIL: map ki key `enum class NoteDenomination` hai — par unordered_map
//  ko key HASH karna aata nahi (enum class ke liye default hash nahi hota),
//  isliye custom NoteDenominationHash struct banaya hai jo enum ko int me
//  badal ke hash karta hai. Ye C++ ka ek common gotcha hai (enum class as
//  unordered_map key)! Manager isse har handler ka note-count leta hai.
// ============================================================================
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
