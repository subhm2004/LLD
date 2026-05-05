#ifndef LINKEDIN_LLD_MODELS_POST_H
#define LINKEDIN_LLD_MODELS_POST_H

#include <bits/stdc++.h>

using namespace std;

namespace linkedin_lld {

class Post {
public:
    Post(string postId, string authorId, string content)
        : postId_(std::move(postId)),
          authorId_(std::move(authorId)),
          content_(std::move(content)),
          createdAt_(time(nullptr)),
          likeCount_(0) {}

    const string &getPostId() const { return postId_; }
    const string &getAuthorId() const { return authorId_; }
    const string &getContent() const { return content_; }
    long long getCreatedAt() const { return createdAt_; }
    int getLikeCount() const { return likeCount_; }

    bool addLike(const string &userId) {
        if (likedBy_.count(userId) != 0) {
            return false;
        }
        likedBy_.insert(userId);
        ++likeCount_;
        return true;
    }

private:
    string postId_;
    string authorId_;
    string content_;
    long long createdAt_;
    int likeCount_;
    unordered_set<string> likedBy_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_POST_H
