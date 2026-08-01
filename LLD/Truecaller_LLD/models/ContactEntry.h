// ============================================================================
//  models/ContactEntry.h  —  User ki phonebook ka ek contact
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ DO NAAM hain — aur yahi is class ka poora point hai                 │
//  │                                                                          │
//  │     localName_     -> jo TUMNE apne phone me save kiya ("Mom", "Bhai")   │
//  │     directoryName_ -> jo DUNIYA jaanti hai ("Sunita Sharma")             │
//  │                                                                          │
//  │  Dono ek doosre ko REPLACE nahi karte, saath rehte hain:                 │
//  │   • Tumhe apna hi diya naam dikhna chahiye ("Mom", na ki "Sunita Sharma") │
//  │   • Par app ye bhi bata sakta hai "waise ye Sunita Sharma hai"           │
//  │  Agar directory naam se local ko OVERWRITE kar dete to user ka apna       │
//  │  personal context chala jaata — bahut kharab UX. 😤                       │
//  │                                                                          │
//  │  📌 Rule: user ka DIYA HUA data kabhi silently mat badlo. System ka       │
//  │     enrichment ALAG field me rakho, saath me dikhao.                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `hasDirectoryMatch()` = "directoryName_ khaali nahi hai".
//     Ye simple check UI ko batata hai ki extra info dikhani hai ya nahi
//     (demo output me "-> directory: Ravi Kumar" isi se aata hai).
//     Alag `bool matched_` flag rakhne se do fields sync me rakhne padte —
//     ek hi field se derive karna zyada safe hai (single source of truth).
//
//  ⭐ `localName_` aur `phone_` ke setters NAHI hain — ye contact ki pehchaan
//     hai, upload ke waqt fix ho jaati hai. Sirf `setDirectoryName` hai, jo
//     system enrichment ke waqt bharta hai. Yaani likhne ka haq bata hi deta
//     hai ki kaun sa data kiska hai.
//
//  📌 Constructor me `normalizePhone` — isi wajah se ContactSyncService seedha
//     `directory.find(entry.getPhone())` kar sakta hai. Dono taraf same format
//     hai to lookup bina kisi extra kaam ke match ho jaata hai.
// ============================================================================
#ifndef TRUECALLER_LLD_MODELS_CONTACTENTRY_H
#define TRUECALLER_LLD_MODELS_CONTACTENTRY_H

#include <string>
#include <utility>

#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

/** User's synced contact: uska apna naam + system ka enrichment. */
class ContactEntry {
public:
    ContactEntry() = default;

    ContactEntry(std::string localName, std::string phone)
        : localName_(std::move(localName)),
          phone_(normalizePhone(std::move(phone))) {} // ⭐ directory ke key se match karega

    const std::string& getLocalName() const { return localName_; }   // ⭐ user ka naam
    const std::string& getPhone() const { return phone_; }

    // System enrichment — sirf yahi likha ja sakta hai.
    void setDirectoryName(const std::string& name) { directoryName_ = name; }
    const std::string& getDirectoryName() const { return directoryName_; } // ⭐ duniya ka naam
    bool hasDirectoryMatch() const { return !directoryName_.empty(); }

private:
    std::string localName_;     // "Mom" (user ne save kiya)
    std::string phone_;
    std::string directoryName_; // "Sunita Sharma" (system ne bhara) — khaali = match nahi
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CONTACTENTRY_H
