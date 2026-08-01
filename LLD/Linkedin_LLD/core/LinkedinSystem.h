// ============================================================================
//  core/LinkedinSystem.h  —  Poore system ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client (main.cpp) ke liye "LinkedIn" hai. Andar 6 services aur
//  ~10 models ka poora tamasha chalta hai, par bahar se client ko sirf saaf-
//  suthre methods dikhte hain: registerUser, sendConnectionRequest, createPost,
//  react, sendMessage, search, getFeed, getAnalytics, ...
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — client ko subsystem ki complexity se bachao         │
//  │                                                                          │
//  │  Client ko ye NAHI pata:                                                 │
//  │    - feed kaise ban-ta hai       (FeedService)                           │
//  │    - notifications kaise bhejte  (NotificationService — Observer)        │
//  │    - search kaise hoti hai       (SearchService)                         │
//  │    - analytics kaise judte hain  (AnalyticsService)                      │
//  │  Wo bas facade ke methods bulata hai. Har method andar sahi service ko   │
//  │  sahi data ke saath delegate kar deta hai. Facade = orchestrator.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE khud data ka MAALIK hai (users, connections, posts, pages, groups)
//     aur SERVICES ko wo data pass karta hai. Isi liye services zyadatar
//     STATELESS hain (FeedService/SearchService/AnalyticsService) — single
//     source of truth ek jagah (yahan) rehta hai. Jinki apni state cohesive hai
//     (messaging, notifications, recommendations) wo apna data khud rakhti hain.
//
//  ⚠ MEMORY: Posts `new` se bante hain (Post*), isliye destructor unhe delete
//     karta hai. Copy band ki hai (delete) — do systems same Post* share karte
//     to double-free hota. (Rule of Three.)
//
//  📌 Har public method ke upar likha hai wo kaunsi requirement (R1..R13) poori
//     karta hai — code padho aur requirement turant map ho jaaye.
// ============================================================================
#ifndef LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H
#define LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../enums/ConnectionStatus.h"
#include "../enums/NotificationType.h"
#include "../enums/ReactionType.h"
#include "../models/CompanyPage.h"
#include "../models/ConnectionRequest.h"
#include "../models/Group.h"
#include "../models/Notification.h"
#include "../models/Post.h"
#include "../models/Recommendation.h"
#include "../models/UserProfile.h"
#include "../services/AnalyticsService.h"
#include "../services/FeedService.h"
#include "../services/MessagingService.h"
#include "../services/NotificationService.h"
#include "../services/RecommendationService.h"
#include "../services/SearchService.h"

namespace linkedin_lld {

class LinkedinSystem {
public:
    // ---- Posts heap pe hain -> destructor unhe saaf karta hai --------------
    ~LinkedinSystem() {
        for (auto &entry : postsByUser_) {
            for (Post *post : entry.second) {
                delete post;
            }
        }
    }

    // Copy band — do systems same Post* share karte to double-free hota (Rule of Three)
    LinkedinSystem() = default;
    LinkedinSystem(const LinkedinSystem &) = delete;
    LinkedinSystem &operator=(const LinkedinSystem &) = delete;

    // ========================================================================
    //  R1 — PROFILE: register + rich profile view
    // ========================================================================
    // Profile bahar se fluent API se bharke aata hai (education/experience/skills),
    // yahan bas store hota hai. (UserProfile.h me builder-style setters dekho.)
    void registerUser(const UserProfile &profile) { users_.insert_or_assign(profile.getUserId(), profile); }

    // R2 tie-in: kisi ka profile "dekhna" — target ke profile views badhte hain (R5)
    const UserProfile &viewProfile(const std::string &viewerId, const std::string &targetId) {
        validateUserExists(viewerId);
        validateUserExists(targetId);
        if (viewerId != targetId) {
            users_.at(targetId).incrementProfileViews(); // apna profile dekhne se view nahi badhta
        }
        return users_.at(targetId);
    }

    const UserProfile &getUserProfile(const std::string &userId) const {
        validateUserExists(userId);
        return users_.at(userId);
    }

    // ========================================================================
    //  R2 — SEARCH: users + company pages + groups
    // ========================================================================
    SearchService::SearchResult search(const std::string &keyword) {
        return searchService_.search(keyword, users_, companyPages_, groups_);
    }

    // ========================================================================
    //  R3 — CONNECTION MANAGEMENT: send / accept / reject / cancel / ignore
    // ========================================================================
    std::string sendConnectionRequest(const std::string &fromUserId, const std::string &toUserId) {
        validateUserExists(fromUserId);
        validateUserExists(toUserId);
        if (fromUserId == toUserId) {
            throw std::runtime_error("Cannot connect with self");
        }
        if (areConnected(fromUserId, toUserId)) {
            throw std::runtime_error("Users already connected");
        }
        if (hasPendingRequestBetween(fromUserId, toUserId)) {
            throw std::runtime_error("Pending connection request already exists between users");
        }

        const std::string requestId = "REQ_" + std::to_string(++requestCounter_);
        connectionRequests_.insert_or_assign(requestId, ConnectionRequest(requestId, fromUserId, toUserId));

        // R10: receiver ko notify karo ki connection request aayi
        notificationService_.notify(toUserId, NotificationType::CONNECTION_REQUEST,
                                    displayName(fromUserId) + " ne aapko connection request bheji");
        return requestId;
    }

    void acceptConnectionRequest(const std::string &requestId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        requirePending(request); // sirf PENDING se transition (ConnectionRequest.h state machine)

        request->setStatus(ConnectionStatus::ACCEPTED);
        // Mutual connection — dono taraf jodo
        connections_[request->getFromUserId()].insert(request->getToUserId());
        connections_[request->getToUserId()].insert(request->getFromUserId());

        // R10: bhejne wale ko batao ki accept ho gaya
        notificationService_.notify(request->getFromUserId(), NotificationType::CONNECTION_REQUEST,
                                    displayName(request->getToUserId()) + " ne aapki connection request accept ki");
    }

    void rejectConnectionRequest(const std::string &requestId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        requirePending(request);
        request->setStatus(ConnectionStatus::REJECTED);
    }

    // R3 "cancel" — sirf BHEJNE WALA (from) apni request wapas le sakta hai
    void cancelConnectionRequest(const std::string &requestId, const std::string &requesterId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        requirePending(request);
        if (request->getFromUserId() != requesterId) {
            throw std::runtime_error("Sirf request bhejne wala hi use cancel kar sakta hai");
        }
        request->setStatus(ConnectionStatus::CANCELLED);
    }

    // R3 "ignore" — sirf PAANE WALA (to) request ignore kar sakta hai
    void ignoreConnectionRequest(const std::string &requestId, const std::string &receiverId) {
        ConnectionRequest *request = getRequestOrThrow(requestId);
        requirePending(request);
        if (request->getToUserId() != receiverId) {
            throw std::runtime_error("Sirf request paane wala hi use ignore kar sakta hai");
        }
        request->setStatus(ConnectionStatus::IGNORED);
    }

    ConnectionStatus getConnectionRequestStatus(const std::string &requestId) {
        return getRequestOrThrow(requestId)->getStatus();
    }

    // ========================================================================
    //  R4 — FOLLOW: bina connect kiye follow karo (one-directional)
    // ========================================================================
    //  ⭐ Connection MUTUAL hai (dono taraf), par FOLLOW ONE-WAY hai — main
    //  aapko follow kar sakta hoon bina aapke follow kiye. Isi liye alag graph.
    void followUser(const std::string &followerId, const std::string &followeeId) {
        validateUserExists(followerId);
        validateUserExists(followeeId);
        if (followerId == followeeId) {
            throw std::runtime_error("Cannot follow self");
        }
        following_[followerId].insert(followeeId);
    }

    void unfollowUser(const std::string &followerId, const std::string &followeeId) {
        auto it = following_.find(followerId);
        if (it != following_.end()) {
            it->second.erase(followeeId);
        }
    }

    bool isFollowing(const std::string &followerId, const std::string &followeeId) const {
        auto it = following_.find(followerId);
        return it != following_.end() && it->second.count(followeeId) != 0;
    }

    std::unordered_set<std::string> getFollowing(const std::string &userId) const {
        auto it = following_.find(userId);
        return it == following_.end() ? std::unordered_set<std::string>{} : it->second;
    }

    // ========================================================================
    //  R5 — ANALYTICS: connections, profile views, impressions, search appearances
    // ========================================================================
    AnalyticsService::UserAnalytics getAnalytics(const std::string &userId) const {
        validateUserExists(userId);
        return analyticsService_.computeFor(userId, users_, connections_, postsByUser_);
    }

    // ========================================================================
    //  R6 — RECOMMENDATIONS: request + provide
    // ========================================================================
    void requestRecommendation(const std::string &requesterId, const std::string &authorId) {
        validateUserExists(requesterId);
        validateUserExists(authorId);
        recommendationService_.requestRecommendation(requesterId, authorId);
    }

    std::string provideRecommendation(const std::string &authorId, const std::string &targetId,
                                      const std::string &text) {
        validateUserExists(authorId);
        validateUserExists(targetId);
        if (text.empty()) {
            throw std::runtime_error("Recommendation text khaali nahi ho sakta");
        }
        return recommendationService_.provideRecommendation(authorId, targetId, text);
    }

    const std::vector<Recommendation> &getRecommendationsFor(const std::string &userId) const {
        return recommendationService_.getRecommendationsFor(userId);
    }

    // ========================================================================
    //  R7 — CONTENT CREATION: post banao
    // ========================================================================
    std::string createPost(const std::string &authorId, const std::string &content) {
        validateUserExists(authorId);
        if (content.empty()) {
            throw std::runtime_error("Post content cannot be empty");
        }
        const std::string postId = "POST_" + std::to_string(++postCounter_);
        Post *post = new Post(postId, authorId, content);
        postsByUser_[authorId].push_back(post);
        return postId;
    }

    // ========================================================================
    //  R8 — ENGAGEMENT: react / like / comment / react-to-comment / share
    // ========================================================================
    // React (6 types). true -> naya/badla, false -> pehle se wahi tha.
    bool reactToPost(const std::string &userId, const std::string &postId, ReactionType type) {
        validateUserExists(userId);
        Post *post = getPostOrThrow(postId);
        const bool changed = post->react(userId, type);
        if (changed) {
            notifyPostAuthor(post, userId, displayName(userId) + " ne aapke post pe " +
                                              reactionTypeToString(type) + " kiya");
        }
        return changed;
    }

    // Backward-compatible like (andar se LIKE reaction)
    bool likePost(const std::string &userId, const std::string &postId) {
        return reactToPost(userId, postId, ReactionType::LIKE);
    }

    // Comment karo -> commentId lauta do
    std::string commentOnPost(const std::string &userId, const std::string &postId,
                              const std::string &content) {
        validateUserExists(userId);
        if (content.empty()) {
            throw std::runtime_error("Comment khaali nahi ho sakta");
        }
        Post *post = getPostOrThrow(postId);
        const std::string commentId = "CMT_" + std::to_string(++commentCounter_);
        post->addComment(Comment(commentId, userId, content));
        notifyPostAuthor(post, userId, displayName(userId) + " ne aapke post pe comment kiya");
        return commentId;
    }

    // "Engage with comments" — comment pe react karo
    bool reactToComment(const std::string &userId, const std::string &postId,
                        const std::string &commentId, ReactionType type) {
        validateUserExists(userId);
        Post *post = getPostOrThrow(postId);
        Comment *comment = post->findComment(commentId);
        if (comment == nullptr) {
            throw std::runtime_error("Comment not found: " + commentId);
        }
        const bool changed = comment->react(userId, type);
        if (changed && comment->getAuthorId() != userId) {
            // R10: comment likhne wale ko batao
            notificationService_.notify(comment->getAuthorId(), NotificationType::POST_INTERACTION,
                                        displayName(userId) + " ne aapke comment pe react kiya");
        }
        return changed;
    }

    // Share karo (duplicate share safe)
    bool sharePost(const std::string &userId, const std::string &postId) {
        validateUserExists(userId);
        Post *post = getPostOrThrow(postId);
        const bool shared = post->addShare(userId);
        if (shared) {
            notifyPostAuthor(post, userId, displayName(userId) + " ne aapka post share kiya");
        }
        return shared;
    }

    // ========================================================================
    //  R6/FR6 — FEED: apne + connections ke posts, ranked
    // ========================================================================
    std::vector<Post *> getFeed(const std::string &viewerId, size_t limit = 10) const {
        validateUserExists(viewerId);
        return feedService_.buildFeed(viewerId, postsByUser_, connections_, limit);
    }

    // Post ko id se padhne ke liye (demo printing me kaam aata hai)
    const Post *getPost(const std::string &postId) const { return getPostOrThrow(postId); }

    // ========================================================================
    //  R9 — MESSAGING: send / inbox / conversation
    // ========================================================================
    std::string sendMessage(const std::string &fromUserId, const std::string &toUserId,
                           const std::string &content) {
        validateUserExists(fromUserId);
        validateUserExists(toUserId);
        if (content.empty()) {
            throw std::runtime_error("Message khaali nahi ho sakta");
        }
        const std::string messageId = messagingService_.sendMessage(fromUserId, toUserId, content);
        // R10: receiver ko naye message ki notification
        notificationService_.notify(toUserId, NotificationType::MESSAGE,
                                    displayName(fromUserId) + " ne aapko message bheja");
        return messageId;
    }

    std::vector<Message> getInbox(const std::string &userId) const {
        validateUserExists(userId);
        return messagingService_.getInbox(userId);
    }

    std::vector<Message> getConversation(const std::string &userA, const std::string &userB) const {
        return messagingService_.getConversation(userA, userB);
    }

    // ========================================================================
    //  R10 — NOTIFICATIONS: observer register + inbox
    // ========================================================================
    void registerNotificationObserver(const std::shared_ptr<INotificationObserver> &observer) {
        notificationService_.registerObserver(observer);
    }

    const std::vector<Notification> &getNotifications(const std::string &userId) const {
        return notificationService_.getInbox(userId);
    }

    int getUnreadNotificationCount(const std::string &userId) const {
        return notificationService_.getUnreadCount(userId);
    }

    // ========================================================================
    //  R11 — COMPANY PAGES: create + follow
    // ========================================================================
    std::string createCompanyPage(const std::string &adminId, const std::string &name) {
        validateUserExists(adminId);
        const std::string pageId = "PAGE_" + std::to_string(++pageCounter_);
        companyPages_.emplace(pageId, CompanyPage(pageId, name, adminId));
        return pageId;
    }

    void followCompanyPage(const std::string &userId, const std::string &pageId) {
        validateUserExists(userId);
        getPageOrThrow(pageId).addFollower(userId);
    }

    const CompanyPage &getCompanyPage(const std::string &pageId) const {
        auto it = companyPages_.find(pageId);
        if (it == companyPages_.end()) {
            throw std::runtime_error("Company page not found: " + pageId);
        }
        return it->second;
    }

    // ========================================================================
    //  R12 — JOB LISTINGS: page pe job daalo + apply karo
    // ========================================================================
    std::string postJob(const std::string &pageId, const std::string &title,
                        const std::string &description) {
        CompanyPage &page = getPageOrThrow(pageId);
        const std::string jobId = "JOB_" + std::to_string(++jobCounter_);
        page.addJobListing(JobListing(jobId, pageId, title, description));
        return jobId;
    }

    bool applyToJob(const std::string &userId, const std::string &pageId, const std::string &jobId) {
        validateUserExists(userId);
        CompanyPage &page = getPageOrThrow(pageId);
        JobListing *job = page.findJob(jobId);
        if (job == nullptr) {
            throw std::runtime_error("Job not found: " + jobId);
        }
        return job->apply(userId);
    }

    // ========================================================================
    //  R13 — GROUPS: create + join
    // ========================================================================
    std::string createGroup(const std::string &creatorId, const std::string &name) {
        validateUserExists(creatorId);
        const std::string groupId = "GRP_" + std::to_string(++groupCounter_);
        groups_.emplace(groupId, Group(groupId, name, creatorId));
        return groupId;
    }

    void joinGroup(const std::string &userId, const std::string &groupId) {
        validateUserExists(userId);
        getGroupOrThrow(groupId).join(userId);
    }

    const Group &getGroup(const std::string &groupId) const {
        auto it = groups_.find(groupId);
        if (it == groups_.end()) {
            throw std::runtime_error("Group not found: " + groupId);
        }
        return it->second;
    }

private:
    // ---- Data (facade single source of truth) ------------------------------
    std::unordered_map<std::string, UserProfile> users_;
    std::unordered_map<std::string, ConnectionRequest> connectionRequests_;
    std::unordered_map<std::string, std::unordered_set<std::string>> connections_; // mutual (R3)
    std::unordered_map<std::string, std::unordered_set<std::string>> following_;    // one-way (R4)
    std::unordered_map<std::string, std::vector<Post *>> postsByUser_;              // R7 (heap posts)
    std::unordered_map<std::string, CompanyPage> companyPages_;                     // R11/R12
    std::unordered_map<std::string, Group> groups_;                                 // R13

    // ---- Services ----------------------------------------------------------
    FeedService feedService_;                   // FR6 (stateless)
    SearchService searchService_;               // R2  (stateless)
    AnalyticsService analyticsService_;         // R5  (stateless)
    NotificationService notificationService_;   // R10 (Observer subject; owns inbox)
    MessagingService messagingService_;         // R9  (owns messages)
    RecommendationService recommendationService_; // R6 (owns recommendations)

    // ---- Counters (unique ID generation) -----------------------------------
    int requestCounter_ = 0;
    int postCounter_ = 0;
    int commentCounter_ = 0;
    int pageCounter_ = 0;
    int jobCounter_ = 0;
    int groupCounter_ = 0;

    // ---- Validation / helper functions -------------------------------------
    void validateUserExists(const std::string &userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("User not found: " + userId);
        }
    }

    // Notification me userId ki jagah naam dikhe (thoda friendly)
    std::string displayName(const std::string &userId) const {
        auto it = users_.find(userId);
        return it == users_.end() ? userId : it->second.getName();
    }

    // Post author ko interaction ki notification (self-interaction pe nahi)
    void notifyPostAuthor(const Post *post, const std::string &actorId, const std::string &text) {
        if (post->getAuthorId() != actorId) {
            notificationService_.notify(post->getAuthorId(), NotificationType::POST_INTERACTION, text);
        }
    }

    bool areConnected(const std::string &a, const std::string &b) const {
        auto it = connections_.find(a);
        return it != connections_.end() && it->second.count(b) != 0;
    }

    bool hasPendingRequestBetween(const std::string &userA, const std::string &userB) const {
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

    // Ek jagah "PENDING hai?" guard — accept/reject/cancel/ignore sab yahi bulaate hain
    static void requirePending(const ConnectionRequest *request) {
        if (request->getStatus() != ConnectionStatus::PENDING) {
            throw std::runtime_error("Request already processed");
        }
    }

    ConnectionRequest *getRequestOrThrow(const std::string &requestId) {
        auto it = connectionRequests_.find(requestId);
        if (it == connectionRequests_.end()) {
            throw std::runtime_error("Connection request not found");
        }
        return &it->second;
    }

    Post *getPostOrThrow(const std::string &postId) const {
        for (const auto &entry : postsByUser_) {
            for (Post *post : entry.second) {
                if (post->getPostId() == postId) {
                    return post;
                }
            }
        }
        throw std::runtime_error("Post not found: " + postId);
    }

    CompanyPage &getPageOrThrow(const std::string &pageId) {
        auto it = companyPages_.find(pageId);
        if (it == companyPages_.end()) {
            throw std::runtime_error("Company page not found: " + pageId);
        }
        return it->second;
    }

    Group &getGroupOrThrow(const std::string &groupId) {
        auto it = groups_.find(groupId);
        if (it == groups_.end()) {
            throw std::runtime_error("Group not found: " + groupId);
        }
        return it->second;
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_CORE_LINKEDINSYSTEM_H
