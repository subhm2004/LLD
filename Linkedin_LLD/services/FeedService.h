#ifndef LINKEDIN_LLD_SERVICES_FEEDSERVICE_H
#define LINKEDIN_LLD_SERVICES_FEEDSERVICE_H

#include <bits/stdc++.h>

#include "../models/Post.h"

using namespace std;

namespace linkedin_lld {

class FeedService {
public:
    vector<Post *> buildFeed(const string &viewerId,
                             const unordered_map<string, vector<Post *>> &postsByUser,
                             const unordered_map<string, unordered_set<string>> &connections,
                             size_t limit = 10) const {
        vector<Post *> feed;

        auto connIt = connections.find(viewerId);
        if (connIt != connections.end()) {
            for (const string &connectionId : connIt->second) {
                auto postIt = postsByUser.find(connectionId);
                if (postIt != postsByUser.end()) {
                    for (Post *post : postIt->second) {
                        feed.push_back(post);
                    }
                }
            }
        }

        auto ownPostIt = postsByUser.find(viewerId);
        if (ownPostIt != postsByUser.end()) {
            for (Post *post : ownPostIt->second) {
                feed.push_back(post);
            }
        }

        sort(feed.begin(), feed.end(), [](const Post *a, const Post *b) {
            if (a->getLikeCount() == b->getLikeCount()) {
                return a->getCreatedAt() > b->getCreatedAt();
            }
            return a->getLikeCount() > b->getLikeCount();
        });

        if (feed.size() > limit) {
            feed.resize(limit);
        }
        return feed;
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_FEEDSERVICE_H
