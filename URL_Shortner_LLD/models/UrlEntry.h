#ifndef URL_SHORTNER_LLD_MODELS_URLENTRY_H
#define URL_SHORTNER_LLD_MODELS_URLENTRY_H

#include <bits/stdc++.h>

using namespace std;

namespace url_shortner_lld {

class UrlEntry {
public:
    UrlEntry(string shortCode, string longUrl)
        : shortCode_(std::move(shortCode)),
          longUrl_(std::move(longUrl)),
          createdAt_(time(nullptr)),
          clickCount_(0) {}

    const string &getShortCode() const { return shortCode_; }
    const string &getLongUrl() const { return longUrl_; }
    long long getCreatedAt() const { return createdAt_; }
    int getClickCount() const { return clickCount_; }
    void incrementClickCount() { ++clickCount_; }

private:
    string shortCode_;
    string longUrl_;
    long long createdAt_;
    int clickCount_;
};

} // namespace url_shortner_lld

#endif // URL_SHORTNER_LLD_MODELS_URLENTRY_H
