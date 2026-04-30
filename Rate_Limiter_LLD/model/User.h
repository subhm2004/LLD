#ifndef USER_H
#define USER_H

#include <string>
#include "../enums/UserTier.h"

using namespace std;
class User
{
private:
    string userId;
    UserTier tier;

public:
    User(string userId, UserTier tier)
        : userId(std ::move(userId)), tier(tier) {}

    const string &getUserId() const { return userId; }
    UserTier getTier() const { return tier; }
};

#endif // USER_H