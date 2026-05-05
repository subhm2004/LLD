#ifndef TINDER_LLD_MODELS_USERPROFILE_H
#define TINDER_LLD_MODELS_USERPROFILE_H

#include <bits/stdc++.h>

#include "../enums/Gender.h"
#include "Location.h"

using namespace std;

namespace tinder_lld {

class UserProfile {
public:
    UserProfile(string name, int age, Gender gender, Location location)
        : name_(std::move(name)), age_(age), gender_(gender), location_(location) {}

    const string &getName() const { return name_; }
    int getAge() const { return age_; }
    Gender getGender() const { return gender_; }
    Location getLocation() const { return location_; }

private:
    string name_;
    int age_;
    Gender gender_;
    Location location_;
};

} // namespace tinder_lld

#endif // TINDER_LLD_MODELS_USERPROFILE_H
