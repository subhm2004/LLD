#ifndef LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H
#define LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H

#include <bits/stdc++.h>

#include "../enums/ConnectionStatus.h"
#include "../models/ConnectionRequest.h"
#include "../models/Post.h"
#include "../models/UserProfile.h"
#include "../services/FeedService.h"

using namespace std;

namespace linkedin_lld {

class LinkedinSystem {
public:
    ~LinkedinSystem() {
        for (auto &entry : postsByUser_) {
            for (Post *post : entry.second) {
                delete post;
            }
        }
    }

    void registerUser(const UserProfile &profile) { users_.insert_or_assign(profile.getUserId(), profile); }

    string sendConnectionRequest(const string &fromUserId, const string &toUserId) {
        validateUserExists(fromUserId);
        validateUserExists(toUserId);
        if (fromUserId == toUserId) {
            throw runtime_error("Cannot connect with self");
        }
        if (areConnected(fromUserId, toUserId)) {
            throw runtime_error("Users already connected");
        }
        if (hasPendingRequestBetween(fromUserId, toUserId)) {
            throw runtime_error("Pending connection request already exists between users");
        }

        const string requestId = "REQ_" + to_string(++requestCounter_);
        connectionRequests_.insert_or_assign(requestId, ConnectionRequest(requestId, fromUserId, toUserId));
        return requestId;
    }

    void acceptConnectionRequest(const string &requestId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        if (request->getStatus() != ConnectionStatus::PENDING) {
            throw runtime_error("Request already processed");
        }

        request->setStatus(ConnectionStatus::ACCEPTED);
        connections_[request->getFromUserId()].insert(request->getToUserId());
        connections_[request->getToUserId()].insert(request->getFromUserId());
    }

    void rejectConnectionRequest(const string &requestId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        if (request->getStatus() != ConnectionStatus::PENDING) {
            throw runtime_error("Request already processed");
        }
        request->setStatus(ConnectionStatus::REJECTED);
    }

    ConnectionStatus getConnectionRequestStatus(const string &requestId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        return request->getStatus();
    }

    string createPost(const string &authorId, const string &content) {
        validateUserExists(authorId);
        if (content.empty()) {
            throw runtime_error("Post content cannot be empty");
        }
        const string postId = "POST_" + to_string(++postCounter_);
        Post *post = new Post(postId, authorId, content);
        postsByUser_[authorId].push_back(post);
        return postId;
    }

    bool likePost(const string &userId, const string &postId) {
        validateUserExists(userId);
        Post *post = getPostOrThrow(postId);
        return post->addLike(userId);
    }

    vector<Post *> getFeed(const string &viewerId, size_t limit = 10) const {
        validateUserExists(viewerId);
        return feedService_.buildFeed(viewerId, postsByUser_, connections_, limit);
    }

private:
    unordered_map<string, UserProfile> users_;
    unordered_map<string, ConnectionRequest> connectionRequests_;
    unordered_map<string, unordered_set<string>> connections_;
    unordered_map<string, vector<Post *>> postsByUser_;
    FeedService feedService_;
    int requestCounter_ = 0;
    int postCounter_ = 0;

    void validateUserExists(const string &userId) const {
        if (users_.find(userId) == users_.end()) {
            throw runtime_error("User not found: " + userId);
        }
    }

    bool areConnected(const string &a, const string &b) const {
        auto it = connections_.find(a);
        if (it == connections_.end()) {
            return false;
        }
        return it->second.count(b) != 0;
    }

    bool hasPendingRequestBetween(const string &userA, const string &userB) const {
        for (const auto &entry : connectionRequests_) {
            const ConnectionRequest &request = entry.second;
            if (request.getStatus() != ConnectionStatus::PENDING) {
                continue;
            }

            const bool forward = request.getFromUserId() == userA && request.getToUserId() == userB;
            const bool reverse = request.getFromUserId() == userB && request.getToUserId() == userA;
            if (forward || reverse) {
                return true;
            }
        }
        return false;
    }

    ConnectionRequest *getRequestOrThrow(const string &requestId) {
        auto it = connectionRequests_.find(requestId);
        if (it == connectionRequests_.end()) {
            throw runtime_error("Connection request not found");
        }
        return &it->second;
    }

    Post *getPostOrThrow(const string &postId) const {
        for (const auto &entry : postsByUser_) {
            for (Post *post : entry.second) {
                if (post->getPostId() == postId) {
                    return post;
                }
            }
        }
        throw runtime_error("Post not found: " + postId);
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H
