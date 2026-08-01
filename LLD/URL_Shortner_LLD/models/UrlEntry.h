// ============================================================================
//  models/UrlEntry.h  —  Ek shortened URL ka record
// ----------------------------------------------------------------------------
//  Jab bhi koi long URL shorten hoti hai, ek UrlEntry banta hai. Isme:
//     shortCode  -> chhota code (jaise "g8") — long URL ka "naam"
//     longUrl    -> asli poori URL
//     createdAt  -> kab bani (epoch seconds)
//     clickCount -> kitni baar is short URL ko access/resolve kiya gaya
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `clickCount_` — analytics ka dil (Requirement #4, #5)                 │
//  │                                                                          │
//  │  Har baar jab short URL resolve hoti hai (koi us link pe click karta),    │
//  │  service `incrementClickCount()` bulati hai. Isse "ye link kitna popular  │
//  │  hai" ka data milta hai (printStats me dikhta). Ye counter UrlEntry ke    │
//  │  andar hai kyunki ye is entry ki apni state hai (encapsulation).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `createdAt_` = `time(nullptr)` (epoch seconds since 1970). Isse entries ko
//     time se sort/filter kiya ja sakta hai, ya expiry policy add ki ja sakti
//     (jo abhi out of scope hai — requirements Assumption).
//
//  ⭐ Zyadatar getters hi hain (sirf clickCount badalta hai) — baaki fields
//     ek baar set hone ke baad immutable hain (shortCode/longUrl kabhi nahi
//     badalte). Ye jaan-boojh ke hai: ek short URL ka mapping fixed rehna chahiye.
// ============================================================================
#ifndef URL_SHORTNER_LLD_MODELS_URLENTRY_H
#define URL_SHORTNER_LLD_MODELS_URLENTRY_H

#include <ctime>
#include <string>
#include <utility>

using namespace std;

namespace url_shortner_lld {

class UrlEntry {
public:
    // Naya entry — click count 0 se, createdAt abhi ka time.
    UrlEntry(string shortCode, string longUrl)
        : shortCode_(std::move(shortCode)),
          longUrl_(std::move(longUrl)),
          createdAt_(time(nullptr)),
          clickCount_(0) {}

    const string &getShortCode() const { return shortCode_; }
    const string &getLongUrl() const { return longUrl_; }
    long long getCreatedAt() const { return createdAt_; }
    int getClickCount() const { return clickCount_; }

    // Resolve hone par service ise bulati hai (analytics).
    void incrementClickCount() { ++clickCount_; }

private:
    string shortCode_;   // "g8" — chhota code (immutable)
    string longUrl_;     // asli poori URL (immutable)
    long long createdAt_; // kab bani (epoch seconds)
    int clickCount_;      // kitni baar access hui (badalta rehta)
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_MODELS_URLENTRY_H
