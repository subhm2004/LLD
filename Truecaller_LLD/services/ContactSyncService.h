// ============================================================================
//  services/ContactSyncService.h  —  Contacts upload karke ENRICH karna
// ----------------------------------------------------------------------------
//  User apni phonebook bhejta hai; ye service har contact ke saath duniya ka
//  naam jod deti hai (agar directory me mila to).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ENRICH karta hai, REPLACE nahi                                        │
//  │                                                                          │
//  │      entry.setDirectoryName(...)   // NAYI field bharta hai              │
//  │      // localName_ ko HAATH NAHI LAGATA                                  │
//  │                                                                          │
//  │  Yaani tumhara "Mom" hamesha "Mom" hi rahega — saath me app bata dega     │
//  │  ki "waise ye Sunita Sharma hai". User ka apna context kabhi nahi jaata.  │
//  │  📌 Rule: user ka diya hua data kabhi silently mat badlo; system ka       │
//  │     enrichment ALAG field me rakho. (ContactEntry.h ka poora note.)      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `for (ContactEntry entry : uploaded)` — dhyan do, ye by-VALUE hai
//     (`const&` nahi). Jaan-boojh ke: hume har entry ki apni COPY chahiye taaki
//     usme `directoryName` bhar sakein. Input `uploaded` `const&` hai — usko
//     modify karne ka koi haq nahi. Copy banao, usme likho, aage bhejo.
//     📌 Ye "pure function" wali soch hai: input untouched, naya output.
//
//  ⭐ `enriched.reserve(uploaded.size())` — hume exact size pehle se pata hai
//     (har uploaded contact ek result deta hai), to ek hi baar memory allocate
//     karo. Bina reserve ke vector baar-baar grow hoke reallocate karta hai
//     (aur saara data copy karta hai). 1000 contacts pe ye asli farak hai.
//
//  ⭐ Match hone ke liye do shartein: profile mila HO **aur** uska
//     `displayName` khaali na ho. Doosri shart kyun? Kyunki spam-report se bane
//     profiles ka naam "Reported Number" jaisa placeholder hota hai ya khaali —
//     usse contact ko "enrich" karna ulta confusing hoga.
//
//  ⭐ Poora method `const` aur stateless — directory PARAMETER se aati hai.
//     Isse test karna trivial hai: apni banayi hui directory pass karo, output
//     check karo. Koi setup/teardown nahi.
//
//  ⚠ PRIVACY (interview me zaroor bolna): asli duniya me poori phonebook upload
//     karna ek bada privacy issue hai — tumhare contacts ne to consent diya hi
//     nahi. GDPR/DPDP me ye seedha regulated hai. Isi wajah se Truecaller pe
//     kai desh me case chale hain. Design me hashed/consented sync ka option
//     hona chahiye. Ye LLD me out-of-scope hai par jaanna zaroori hai.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H
#define TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../models/ContactEntry.h"
#include "../models/PhoneProfile.h"

namespace truecaller_lld {

class ContactSyncService {
public:
    std::vector<ContactEntry> syncContacts(
        const std::vector<ContactEntry>& uploaded,
        const std::unordered_map<std::string, PhoneProfile>& directory) const {
        std::vector<ContactEntry> enriched;
        enriched.reserve(uploaded.size()); // ⭐ ek hi allocation

        for (ContactEntry entry : uploaded) { // ⭐ by-value COPY (input untouched)
            // Phone dono taraf normalized hai -> seedha find chalta hai.
            auto it = directory.find(entry.getPhone());
            // ⭐ Placeholder/khaali naam se enrich mat karo (upar note).
            if (it != directory.end() && !it->second.getDisplayName().empty()) {
                entry.setDirectoryName(it->second.getDisplayName()); // ⭐ ADD, replace nahi
            }
            enriched.push_back(std::move(entry));
        }
        return enriched;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H
