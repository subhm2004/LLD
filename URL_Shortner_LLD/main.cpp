#include <bits/stdc++.h>

#include "services/UrlShortnerService.h"

using namespace std;
using namespace url_shortner_lld;

int main() {
    UrlShortnerService shortner;

    const string original1 = "https://www.example.com/articles/low-level-design";
    const string original2 = "https://www.example.com/system-design/url-shortner";

    const string short1 = shortner.shortenUrl(original1);
    const string short2 = shortner.shortenUrl(original2);
    const string short1Again = shortner.shortenUrl(original1);

    cout << "Short URL 1: " << short1 << "\n";
    cout << "Short URL 2: " << short2 << "\n";
    cout << "Short URL 1 (dedup): " << short1Again << "\n";

    cout << "\nResolved 1: " << shortner.resolveShortUrl(short1) << "\n";
    cout << "Resolved 2: " << shortner.resolveShortUrl(short2) << "\n";
    cout << "Resolved 1 again: " << shortner.resolveShortUrl(short1) << "\n";

    shortner.printStats(short1);
    shortner.printStats(short2);
    return 0;
}
