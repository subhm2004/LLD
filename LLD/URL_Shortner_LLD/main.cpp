// ============================================================================
//  main.cpp  —  URL Shortener ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye demo core flow dikhata hai:
//    1. Do long URLs ko shorten karo -> short URLs milte hain
//    2. DEDUP: wahi URL dobara shorten karo -> WAHI short URL (naya nahi)
//    3. Resolve: short URL se wapas long URL (aur click count badhta hai)
//    4. Stats: har short URL ke analytics (clicks, createdAt)
//
//  ⭐ Client ko sirf `UrlShortnerService` dikhta hai (Facade). Base62Encoder,
//     index maps, UrlEntry, validation — kuch nahi. Bas shorten/resolve/stats.
// ============================================================================
#include <iostream>
#include <string>

#include "services/UrlShortnerService.h"

using namespace std;
using namespace url_shortner_lld;

int main() {
    UrlShortnerService shortner;

    const string original1 = "https://www.example.com/articles/low-level-design";
    const string original2 = "https://www.example.com/system-design/url-shortner";

    // ---- Shorten (R1) + dedup (R2) -----------------------------------------
    const string short1 = shortner.shortenUrl(original1);
    const string short2 = shortner.shortenUrl(original2);
    const string short1Again = shortner.shortenUrl(original1); // ⭐ same URL -> same short

    cout << "Short URL 1: " << short1 << "\n";
    cout << "Short URL 2: " << short2 << "\n";
    cout << "Short URL 1 (dedup): " << short1Again << "\n"; // short1 ke barabar hona chahiye

    // ---- Resolve (R3) + click tracking (R4) --------------------------------
    // short1 do baar resolve -> uska click count 2 ho jaayega
    cout << "\nResolved 1: " << shortner.resolveShortUrl(short1) << "\n";
    cout << "Resolved 2: " << shortner.resolveShortUrl(short2) << "\n";
    cout << "Resolved 1 again: " << shortner.resolveShortUrl(short1) << "\n";

    // ---- Stats (R5) — short1 ke 2 clicks, short2 ke 1 click ----------------
    shortner.printStats(short1);
    shortner.printStats(short2);
    return 0;
    // ⭐ Koi delete nahi — shortner stack pe hai, scope khatam hote hi uska
    //    destructor saari UrlEntry saaf kar dega.
}
