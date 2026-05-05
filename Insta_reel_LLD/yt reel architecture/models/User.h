#ifndef INSTA_REEL_LLD_MODELS_USER_H
#define INSTA_REEL_LLD_MODELS_USER_H

#include <bits/stdc++.h>

using namespace std;

namespace reel_platform_lld {

class User {
public:
    User(string id, string name) : id_(std::move(id)), name_(std::move(name)) {}

    const string &getId() const { return id_; }
    const string &getName() const { return name_; }

    void follow(const string &userId) {
        if (userId != id_) {
            following_.insert(userId);
        }
    }

    bool isFollowing(const string &userId) const { return following_.count(userId) != 0; }

private:
    string id_;
    string name_;
    unordered_set<string> following_;
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_MODELS_USER_H
