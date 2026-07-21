// ============================================================================
//  models/Beneficiary.h  —  Saved contact ("Bob" -> bob@okhdfc)
// ----------------------------------------------------------------------------
//  GPay me jo saved contacts dikhte hain, wo yahi hai. Chhoti si class hai par
//  ek achhi design baat sikhati hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Beneficiary me `upiId` hai, `userId` NAHI — jaan-boojh ke              │
//  │                                                                          │
//  │  Kyunki jise tum save kar rahe ho wo GPay pe REGISTERED ho, ye zaroori    │
//  │  nahi! UPI ek OPEN network hai — kisi bhi app (PhonePe, Paytm, bank app)  │
//  │  ka VPA save kiya ja sakta hai. Agar `userId` store karte to sirf apne    │
//  │  hi app ke users save kar paate — poora UPI ka point hi khatam.          │
//  │                                                                          │
//  │  📌 Rule: EXTERNAL identifier (jo doosre system ka hai) store karo,       │
//  │     apna INTERNAL id tab jab wo cheez sach me tumhare system ki ho.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `ownerUserId_` — beneficiary KISKI list me hai. Alice ka "Bob" aur Charlie
//     ka "Bob" alag entries hain (nickname bhi alag ho sakta hai). Isi liye
//     GPaySystem me storage `unordered_map<userId, vector<Beneficiary>>` hai.
//
//  📌 `vector` (set nahi) — yaani duplicate beneficiary add ho sakti hai. Chhoti
//     si gap hai; real app me (ownerUserId + upiId) pe uniqueness check hota.
// ============================================================================
#ifndef GPAY_LLD_MODELS_BENEFICIARY_H
#define GPAY_LLD_MODELS_BENEFICIARY_H

#include <string>
#include <utility>

namespace gpay_lld {

class Beneficiary {
public:
    Beneficiary() = default;

    Beneficiary(std::string ownerUserId, std::string nickname, std::string upiId)
        : ownerUserId_(std::move(ownerUserId)), // kiski contact list
          nickname_(std::move(nickname)),       // "Bhai", "Doodhwala"
          upiId_(std::move(upiId)) {}           // ⭐ VPA (userId nahi — upar note)

    const std::string& getOwnerUserId() const { return ownerUserId_; }
    const std::string& getNickname() const { return nickname_; }
    const std::string& getUpiId() const { return upiId_; }

private:
    std::string ownerUserId_;
    std::string nickname_;
    std::string upiId_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_BENEFICIARY_H
