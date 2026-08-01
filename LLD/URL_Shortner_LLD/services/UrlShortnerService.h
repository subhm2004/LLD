// ============================================================================
//  services/UrlShortnerService.h  —  Poore system ka FACADE (asli dimaag)
// ----------------------------------------------------------------------------
//  Ye class hi client ke liye "URL shortener" hai. Andar 3 kaam manage karti hai:
//    1. shorten  -> long URL se ek chhota short code banao (aur yaad rakho)
//    2. resolve  -> short URL se wapas long URL nikaalo (+ click count badhao)
//    3. stats    -> ek short URL ke analytics dikhao (clicks, createdAt)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE — client ko subsystem ki complexity se bachao                 │
//  │                                                                          │
//  │  Client ko ye NAHI pata: base62 encoding, unique-code generation,        │
//  │  dedup index, click counters, URL validation. Wo bas 3 method bulata hai. │
//  │  Andar Base62Encoder (delegation) + do index maps + UrlEntry sab chhupe.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO INDEX MAPS — dono direction ki O(1) lookup                        │
//  │                                                                          │
//  │  shortToEntry_ : shortCode -> UrlEntry*   (resolve/stats ke liye)         │
//  │  longToShort_  : longUrl   -> shortCode   (DEDUP ke liye — R2)            │
//  │                                                                          │
//  │  Kyun do? Kyunki dono taraf ki query chahiye: "is short code ka long URL?"│
//  │  aur "is long URL ka pehle se code hai?" (taaki dobara na banaye). Ek hi  │
//  │  map se ek direction milti; do maps se dono O(1). Ye "space for speed"    │
//  │  wala classic trade-off hai (thodi extra memory, par fast dono-taraf).   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ MEMORY: UrlEntry `new` se bante hain (raw UrlEntry*), destructor unhe  │
//  │     delete karta hai. ISI LIYE COPY/MOVE EXPLICITLY DELETE KIYE HAIN:     │
//  │                                                                          │
//  │  🐛 Ye ek ASLI latent bug thi: is class me koi unique_ptr member nahi     │
//  │     (LoadBalancer ke ulat jise unique_ptr bacha leta tha). Iske saare     │
//  │     members (2 maps, counter, string) copyable hain — to compiler ek      │
//  │     IMPLICIT copy constructor banata jo pointers ko SHALLOW copy karta.   │
//  │     Do copies ban jaatin same UrlEntry* pakde hue -> dono ke destructor   │
//  │     usi pointer ko delete karte -> DOUBLE FREE 💥 (crash).                │
//  │                                                                          │
//  │  ✅ Fix: copy/move ko `= delete` kar diya — ab galti se copy ho hi nahi   │
//  │     sakti (compile error milega, silent crash nahi). Rule of Three/Five.  │
//  │  💡 Aur behtar: `unordered_map<string, UrlEntry>` (by value) rakhte to    │
//  │     manual new/delete ki zaroorat hi na padti. Yahan raw+delete           │
//  │     "interview-simple" rakha, par = delete se safe bana diya.            │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H
#define URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/UrlEntry.h"
#include "../utils/Base62Encoder.h"

using namespace std;

namespace url_shortner_lld {

class UrlShortnerService {
public:
    // Counter 1000 se shuru — chhote/ajeeb codes (jaise "0","1") se bachne ke liye
    // aur seedha decent-length base62 code milta hai.
    UrlShortnerService() : idCounter_(1000) {}

    // Saari UrlEntry heap pe hain -> destructor unhe saaf karta hai (koi leak na rahe).
    ~UrlShortnerService() {
        for (auto &kv : shortToEntry_) {
            delete kv.second;
        }
    }

    // ⭐ Copy/move BAND — warna raw pointers shallow-copy hoke DOUBLE FREE (upar note).
    UrlShortnerService(const UrlShortnerService &) = delete;
    UrlShortnerService &operator=(const UrlShortnerService &) = delete;
    UrlShortnerService(UrlShortnerService &&) = delete;
    UrlShortnerService &operator=(UrlShortnerService &&) = delete;

    // ========================================================================
    //  SHORTEN — long URL se short URL (dedup ke saath) — R1, R2, R6
    // ========================================================================
    string shortenUrl(const string &longUrl) {
        validateLongUrl(longUrl); // R6: galat URL reject

        // ⭐ R2 DEDUP: agar ye long URL pehle se shorten ho chuki hai, to WAHI
        //    purana short code lauta do (naya nahi banao — same URL = same short).
        auto existing = longToShort_.find(longUrl);
        if (existing != longToShort_.end()) {
            return baseDomain_ + "/" + existing->second;
        }

        // Nayi URL -> naya unique short code banao aur dono index me daalo.
        const string shortCode = generateUniqueShortCode();
        UrlEntry *entry = new UrlEntry(shortCode, longUrl);

        shortToEntry_[shortCode] = entry;   // resolve/stats ke liye
        longToShort_[longUrl] = shortCode;   // future dedup ke liye
        return baseDomain_ + "/" + shortCode;
    }

    // ========================================================================
    //  RESOLVE — short URL se wapas long URL (+ click count) — R3, R4
    // ========================================================================
    string resolveShortUrl(const string &shortUrl) {
        const string shortCode = extractShortCode(shortUrl); // URL me se code nikaalo
        auto it = shortToEntry_.find(shortCode);
        if (it == shortToEntry_.end()) {
            throw runtime_error("Short URL not found"); // aisa code hai hi nahi
        }

        it->second->incrementClickCount(); // R4: har resolve = ek click
        return it->second->getLongUrl();
    }

    // ========================================================================
    //  STATS — ek short URL ke analytics — R5
    // ========================================================================
    void printStats(const string &shortUrl) const {
        const string shortCode = extractShortCode(shortUrl);
        auto it = shortToEntry_.find(shortCode);
        if (it == shortToEntry_.end()) {
            throw runtime_error("Short URL not found");
        }

        const UrlEntry *entry = it->second;
        cout << "\n=== URL Stats ===\n";
        cout << "Short URL: " << baseDomain_ << "/" << entry->getShortCode() << "\n";
        cout << "Long URL: " << entry->getLongUrl() << "\n";
        cout << "Clicks: " << entry->getClickCount() << "\n";
        cout << "CreatedAt(epoch): " << entry->getCreatedAt() << "\n";
    }

private:
    unordered_map<string, UrlEntry *> shortToEntry_; // shortCode -> entry (OWNER — dtor deletes)
    unordered_map<string, string> longToShort_;      // longUrl -> shortCode (dedup index)
    long long idCounter_;                            // agla unique id (base62 ka input)
    const string baseDomain_ = "https://sho.rt";     // short URL ka prefix

    // ---- R6: URL valid hai? (khaali nahi, http/https se shuru) --------------
    void validateLongUrl(const string &longUrl) const {
        if (longUrl.empty()) {
            throw runtime_error("Long URL cannot be empty");
        }
        // `rfind(prefix, 0) == 0` = "string prefix se SHURU hoti hai kya" (starts_with,
        // jo C++20 me aaya; C++17 me yahi idiom use karte hain).
        if (longUrl.rfind("http://", 0) != 0 && longUrl.rfind("https://", 0) != 0) {
            throw runtime_error("Long URL must start with http:// or https://");
        }
    }

    // ---- Ek naya, kabhi na use hua short code banao -------------------------
    //
    // ⭐ Counter + base62 = COLLISION-FREE by design. Har counter value ka
    // base62 code UNIQUE hota hai (base62 ek bijection hai). Isi liye niche wala
    // `count == 0` check practically HAMESHA pehli baar pass ho jaata hai.
    // Ye while-loop ek DEFENSIVE safety net hai — agar kabhi code-generation
    // random/hash-based ho jaaye (jahan collision mumkin hai) to ye kaam aata.
    string generateUniqueShortCode() {
        while (true) {
            const string shortCode = Base62Encoder::encode(idCounter_++);
            if (shortToEntry_.count(shortCode) == 0) {
                return shortCode;
            }
            // (counter-based me yahan kabhi nahi aayega — bijection unique deti hai)
        }
    }

    // ---- Poore short URL me se sirf code nikaalo ("...sho.rt/g8" -> "g8") ----
    string extractShortCode(const string &shortUrl) const {
        const size_t pos = shortUrl.rfind('/'); // aakhri '/' ke baad wala hissa = code
        if (pos == string::npos || pos == shortUrl.size() - 1) {
            throw runtime_error("Invalid short URL format"); // koi '/' nahi ya trailing '/'
        }
        return shortUrl.substr(pos + 1);
    }
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H
