// ============================================================================
//  core/TinderSystem.h — FACADE (L17): poore system ka orchestrator ❤️
// ----------------------------------------------------------------------------
//  Ye file sabse important hai — client ko sirf ek clean API deti hai, aur
//  andar sab coordinate karti hai (users, matching, chat rooms, blocks,
//  swipe limits). Har method ke peeche multiple cheezein ho rahi hoti hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  swipe() method ke PEECHE kitna kaam (Facade ka fayda):                 │
//  │                                                                          │
//  │   1. self-swipe check (khud ko swipe nahi)                             │
//  │   2. block check (blocked user ko swipe nahi)                          │
//  │   3. daily limit check (consumeSwipe — 10 swipe/2 super-like per day)   │
//  │   4. swipe record karo (from user ki history me)                       │
//  │   5. MUTUAL like? -> ChatRoom banao (MATCH!)                            │
//  │                                                                          │
//  │  Client ko sirf swipe("user1","user2",RIGHT) dikhta — baaki 5 steps    │
//  │  chhupe hain. Yahi Facade!                                             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  KEY DATA STRUCTURES (interview me batane layak):
//    users_          -> id -> User* (sab users ka registry)
//    chatRooms_      -> "CHAT_a_b" -> ChatRoom (matched pairs ke chat)
//    blockedUsers_   -> id -> set<blocked ids> (block relationships)
//    dailySwipeCount -> id -> count (rate limiting)
//
//  ⭐ CHAT ROOM KEY ka trick: "CHAT_userA_userB" — par order koi bhi ho
//  sakta! isliye getChatRoom() DONO orders check karta (key1 aur key2).
//  Bina iske "user1->user2" aur "user2->user1" alag rooms samajh jaate.
//
//  ⚠️ NOTE: daily limits in-memory counters se hain (dailySwipeCount_) —
//  par ye kabhi RESET nahi hote (koi "naya din" logic nahi). Real system
//  me midnight pe reset ya timestamp-based check chahiye. Demo limitation.
// ============================================================================
#ifndef TINDER_LLD_CORE_TINDERSYSTEM_H
#define TINDER_LLD_CORE_TINDERSYSTEM_H

#include <bits/stdc++.h>

#include "../enums/SwipeAction.h"
#include "../models/ChatRoom.h"
#include "../models/User.h"
#include "../services/MatchingService.h"

using namespace std;

namespace tinder_lld {

class TinderSystem {
public:
    ~TinderSystem() {
        for (auto &entry : users_) {
            delete entry.second;
        }
    }

    void registerUser(const string &userId, const UserProfile &profile) {
        if (users_.count(userId) != 0) {
            throw runtime_error("User already exists");
        }
        users_[userId] = new User(userId, profile);
    }

    vector<User *> discoverProfiles(const string &userId, double maxDistanceKm) {
        User *user = getUserOrThrow(userId);
        vector<User *> raw = matchingService_.findNearbyProfiles(user, users_, maxDistanceKm);
        vector<User *> filtered;
        for (User *candidate : raw) {
            if (isBlockedEitherWay(userId, candidate->getUserId())) {
                continue;
            }
            filtered.push_back(candidate);
        }
        return filtered;
    }

    bool swipe(const string &fromUserId, const string &toUserId, SwipeAction action) {
        if (fromUserId == toUserId) {
            throw runtime_error("Cannot swipe on self");
        }
        if (isBlockedEitherWay(fromUserId, toUserId)) {
            throw runtime_error("Swipe not allowed due to block");
        }
        if (!consumeSwipe(fromUserId, action)) {
            throw runtime_error("Daily swipe/super-like limit reached");
        }

        User *from = getUserOrThrow(fromUserId);
        User *to = getUserOrThrow(toUserId);
        from->swipe(toUserId, action);
        to->touch();

        if ((action == SwipeAction::RIGHT || action == SwipeAction::SUPER_LIKE) && to->liked(fromUserId)) {
            const string roomId = "CHAT_" + fromUserId + "_" + toUserId;
            chatRooms_.emplace(roomId, ChatRoom(roomId, fromUserId, toUserId));
            cout << "Match created between " << fromUserId << " and " << toUserId << "\n";
            return true;
        }
        return false;
    }

    bool undoSwipe(const string &userId, const string &targetUserId) {
        User *user = getUserOrThrow(userId);
        return user->undoSwipe(targetUserId);
    }

    void unmatch(const string &userA, const string &userB) {
        const string key1 = "CHAT_" + userA + "_" + userB;
        const string key2 = "CHAT_" + userB + "_" + userA;
        if (chatRooms_.erase(key1) == 0) {
            chatRooms_.erase(key2);
        }
        cout << "Users unmatched: " << userA << " and " << userB << "\n";
    }

    void blockUser(const string &blockerId, const string &blockedId) {
        getUserOrThrow(blockerId);
        getUserOrThrow(blockedId);
        blockedUsers_[blockerId].insert(blockedId);
        unmatch(blockerId, blockedId);
        cout << blockerId << " blocked " << blockedId << "\n";
    }

    void sendMessage(const string &fromUserId, const string &toUserId, const string &content) {
        ChatRoom *room = getChatRoom(fromUserId, toUserId);
        if (room == nullptr) {
            throw runtime_error("No chat room found for users");
        }
        room->addMessage(fromUserId, content);
        getUserOrThrow(fromUserId)->touch();
        getUserOrThrow(toUserId)->touch();
    }

    void readMessages(const string &readerId, const string &otherUserId) {
        ChatRoom *room = getChatRoom(readerId, otherUserId);
        if (room == nullptr) {
            throw runtime_error("No chat room found for users");
        }
        room->markMessagesReadFor(readerId);
        getUserOrThrow(readerId)->touch();
    }

    void printChat(const string &userA, const string &userB) {
        ChatRoom *room = getChatRoom(userA, userB);
        if (room == nullptr) {
            cout << "No chat found between users.\n";
            return;
        }
        cout << "ChatRoom: " << room->getRoomId() << "\n";
        for (const Message &message : room->getMessages()) {
            cout << message.getSenderId() << ": " << message.getContent()
                 << " [" << (message.isRead() ? "READ" : "UNREAD") << "]\n";
        }
    }

    long long getLastActive(const string &userId) const {
        User *user = getUserOrThrow(userId);
        return user->getLastActive();
    }

private:
    unordered_map<string, User *> users_;
    unordered_map<string, ChatRoom> chatRooms_;
    unordered_map<string, unordered_set<string>> blockedUsers_;
    unordered_map<string, int> dailySwipeCount_;
    unordered_map<string, int> dailySuperLikeCount_;
    int dailySwipeLimit_ = 10;
    int dailySuperLikeLimit_ = 2;
    MatchingService matchingService_;

    User *getUserOrThrow(const string &userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw runtime_error("User not found: " + userId);
        }
        return it->second;
    }

    ChatRoom *getChatRoom(const string &userA, const string &userB) {
        const string key1 = "CHAT_" + userA + "_" + userB;
        const string key2 = "CHAT_" + userB + "_" + userA;
        auto it1 = chatRooms_.find(key1);
        if (it1 != chatRooms_.end()) {
            return &it1->second;
        }
        auto it2 = chatRooms_.find(key2);
        if (it2 != chatRooms_.end()) {
            return &it2->second;
        }
        return nullptr;
    }

    bool isBlockedEitherWay(const string &userA, const string &userB) const {
        auto itA = blockedUsers_.find(userA);
        if (itA != blockedUsers_.end() && itA->second.count(userB) != 0) {
            return true;
        }
        auto itB = blockedUsers_.find(userB);
        if (itB != blockedUsers_.end() && itB->second.count(userA) != 0) {
            return true;
        }
        return false;
    }

    bool consumeSwipe(const string &userId, SwipeAction action) {
        if (action == SwipeAction::LEFT || action == SwipeAction::RIGHT) {
            if (dailySwipeCount_[userId] >= dailySwipeLimit_) {
                return false;
            }
            ++dailySwipeCount_[userId];
            return true;
        }

        if (action == SwipeAction::SUPER_LIKE) {
            if (dailySuperLikeCount_[userId] >= dailySuperLikeLimit_) {
                return false;
            }
            ++dailySuperLikeCount_[userId];
            return true;
        }
        return false;
    }
};

} // namespace tinder_lld

#endif // TINDER_LLD_CORE_TINDERSYSTEM_H
