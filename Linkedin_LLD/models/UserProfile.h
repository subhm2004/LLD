#ifndef LINKEDIN_LLD_MODELS_USERPROFILE_H
#define LINKEDIN_LLD_MODELS_USERPROFILE_H

#include <bits/stdc++.h>

using namespace std;

namespace linkedin_lld {

class UserProfile {
public:
    UserProfile(string userId, string name, string headline)
        : userId_(std::move(userId)), name_(std::move(name)), headline_(std::move(headline)) {}

    const string &getUserId() const { return userId_; }
    const string &getName() const { return name_; }
    const string &getHeadline() const { return headline_; }

private:
    string userId_;
    string name_;
    string headline_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_USERPROFILE_H
