#ifndef URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H
#define URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H

#include <bits/stdc++.h>

#include "../models/UrlEntry.h"
#include "../utils/Base62Encoder.h"

using namespace std;

namespace url_shortner_lld {

class UrlShortnerService {
public:
    UrlShortnerService() : idCounter_(1000) {}

    ~UrlShortnerService() {
        for (auto &kv : shortToEntry_) {
            delete kv.second;
        }
    }

    string shortenUrl(const string &longUrl) {
        validateLongUrl(longUrl);

        auto existing = longToShort_.find(longUrl);
        if (existing != longToShort_.end()) {
            return baseDomain_ + "/" + existing->second;
        }

        const string shortCode = generateUniqueShortCode();
        UrlEntry *entry = new UrlEntry(shortCode, longUrl);

        shortToEntry_[shortCode] = entry;
        longToShort_[longUrl] = shortCode;
        return baseDomain_ + "/" + shortCode;
    }

    string resolveShortUrl(const string &shortUrl) {
        const string shortCode = extractShortCode(shortUrl);
        auto it = shortToEntry_.find(shortCode);
        if (it == shortToEntry_.end()) {
            throw runtime_error("Short URL not found");
        }

        it->second->incrementClickCount();
        return it->second->getLongUrl();
    }

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
    unordered_map<string, UrlEntry *> shortToEntry_;
    unordered_map<string, string> longToShort_;
    long long idCounter_;
    const string baseDomain_ = "https://sho.rt";

    void validateLongUrl(const string &longUrl) const {
        if (longUrl.empty()) {
            throw runtime_error("Long URL cannot be empty");
        }
        if (longUrl.rfind("http://", 0) != 0 && longUrl.rfind("https://", 0) != 0) {
            throw runtime_error("Long URL must start with http:// or https://");
        }
    }

    string generateUniqueShortCode() {
        while (true) {
            const string shortCode = Base62Encoder::encode(idCounter_++);
            if (shortToEntry_.count(shortCode) == 0) {
                return shortCode;
            }
        }
    }

    string extractShortCode(const string &shortUrl) const {
        const size_t pos = shortUrl.rfind('/');
        if (pos == string::npos || pos == shortUrl.size() - 1) {
            throw runtime_error("Invalid short URL format");
        }
        return shortUrl.substr(pos + 1);
    }
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_SERVICES_URLSHORTNERSERVICE_H
