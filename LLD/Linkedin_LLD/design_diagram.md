# LinkedIn LLD — Design Diagrams

> Codebase padh ke banaya. Yeh is repo ka **sabse bada** LLD hai — 16 models + 6 services.
> Isliye ek hi giant diagram ke bajaye **subsystem-wise** diagrams diye hain: connections,
> feed, notifications, aur ek high-level class map.

---

## 1. High-level Class Map — Facade + Services

```mermaid
classDiagram
    direction TB

    class LinkedinSystem {
        <<Facade>>
        -Map users_
        -Map connectionRequests_
        -Map connections_
        -Map following_
        -Map postsByUser_
        -Map companyPages_
        -Map groups_
        -FeedService feedService_
        -SearchService searchService_
        -AnalyticsService analyticsService_
        -NotificationService notificationService_
        -MessagingService messagingService_
        -RecommendationService recommendationService_
        +sendConnectionRequest(from, to)
        +acceptConnectionRequest(id)
        +followUser(follower, followee)
        +createPost(author, content)
        +reactToPost(user, post, type)
        +getFeed(viewer, limit)
        +sendMessage / applyToJob / joinGroup ...
    }

    class FeedService {
        +generateFeed(viewer, connections, postsByUser, limit) Post[]
    }
    class SearchService {
        <<static>>
        +search(keyword, users, pages, groups)$ SearchResult
    }
    class AnalyticsService {
        +getAnalytics(userId, ...) UserAnalytics
    }
    class NotificationService {
        -INotificationObserver[] observers_
        -Map inbox_
        +registerObserver(obs)
        +notify(recipient, type, text)
    }
    class MessagingService {
        -Message[] messages_
        +sendMessage(from, to, content)
        +getInbox(user) Message[]
        +getConversation(a, b) Message[]
    }
    class RecommendationService {
        -Map recommendationsByTarget_
        -PendingRequest[] pendingRequests_
        +requestRecommendation(requester, author)
        +giveRecommendation(...)
    }

    class INotificationObserver {
        <<interface>>
        +onNotification(Notification)*
    }
    class ConsoleNotificationObserver

    INotificationObserver <|.. ConsoleNotificationObserver
    NotificationService o-- INotificationObserver

    LinkedinSystem *-- FeedService
    LinkedinSystem *-- SearchService
    LinkedinSystem *-- AnalyticsService
    LinkedinSystem *-- NotificationService
    LinkedinSystem *-- MessagingService
    LinkedinSystem *-- RecommendationService
```

---

## 2. Domain Models — subsystem-wise

```mermaid
classDiagram
    direction LR

    class UserProfile {
        -string userId_
        -string headline_
        -Education[] educations_
        -Experience[] experiences_
        -string[] skills_
        -int profileViews_
        -int searchAppearances_
        +incrementProfileViews()
    }
    class Education
    class Experience
    class Recommendation {
        -string authorId_
        -string targetId_
        -string text_
    }

    class Post {
        -string postId_
        -string authorId_
        -long long createdAt_
        -Map reactions_
        -Comment[] comments_
        -Set sharedBy_
        +incrementImpressions()
    }
    class Comment
    class ConnectionRequest {
        -string fromUserId_
        -string toUserId_
        -ConnectionStatus status_
    }
    class Message {
        -string fromUserId_
        -string toUserId_
        -string content_
    }
    class Notification {
        -NotificationType type_
        -string text_
        -bool read_
    }
    class CompanyPage {
        -string adminId_
        -Set followers_
        -JobListing[] jobListings_
    }
    class JobListing
    class Group {
        -string creatorId_
        -Set members_
    }

    UserProfile *-- Education
    UserProfile *-- Experience
    Post *-- Comment
    CompanyPage *-- JobListing
```

---

## 3. ⭐ Connections vs Following — do alag rishte

LinkedIn me do tarah ke rishte hain, aur inka data structure alag hai:

```mermaid
flowchart LR
    subgraph mutual["Connection: MUTUAL (do-tarfa)"]
    A1((Alice)) <--> B1((Bob))
    end
    subgraph oneway["Following: ONE-WAY (ek-tarfa)"]
    A2((Alice)) --> C2((Influencer))
    end
```

| | Connection | Following |
|---|---|---|
| Data | `connections_[A].insert(B)` **aur** `connections_[B].insert(A)` | sirf `following_[A].insert(B)` |
| Nature | **do-tarfa** (dono ki set me) | **ek-tarfa** |
| Kaise bane | request → accept (dono ki marzi) | seedha follow (Bob ki marzi nahi chahiye) |
| Feed pe asar | connection ki posts feed me | (follow bhi feed me aa sakta) |

---

## 4. ⭐ Connection Request — State machine

```mermaid
stateDiagram-v2
    [*] --> PENDING : sendConnectionRequest()
    PENDING --> ACCEPTED : accept()<br/>(dono ki connections_ me add)
    PENDING --> REJECTED : reject() (receiver)
    PENDING --> CANCELLED : cancel() (sender ne wapas liya)
    PENDING --> IGNORED : ignore() (receiver)
    ACCEPTED --> [*]
    REJECTED --> [*]
    CANCELLED --> [*]
    IGNORED --> [*]
```

> **Guards (`sendConnectionRequest`):** khud ko request nahi (`from == to`), pehle se connected
> nahi, aur beech me koi pending request nahi honi chahiye. Ye teen checks duplicate/spam
> requests rokte hain.

---

## 5. ⭐ Feed generation flow

```mermaid
flowchart TD
    A[getFeed viewer, limit] --> B[viewer ke saare connections nikaalo]
    B --> C[har connection ki posts collect karo]
    C --> D[viewer ki apni posts bhi add karo]
    D --> E[sort by createdAt DESC - newest pehle]
    E --> F[top 'limit' posts rakho]
    F --> G[har dikhayi gayi post ki<br/>impressions++ - analytics]
    G --> H[feed return]
```

> **Impressions:** feed me dikhne wali har post ka `incrementImpressions()` hota hai — yahi
> data baad me `AnalyticsService` (post impressions) me dikhta hai. View = signal.

---

## 6. Sequence — reactToPost (Observer notification)

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Sys as LinkedinSystem
    participant Post as Post
    participant NS as NotificationService
    participant Obs as ConsoleNotificationObserver

    User->>Sys: reactToPost(userId, postId, LIKE)
    Sys->>Post: reactions_[userId] = LIKE
    Note over Post: Map hai -> ek user ka<br/>ek hi reaction (dobara react = update)
    Sys->>NS: notify(post.author, POST_INTERACTION, text)
    NS->>NS: inbox_[author].push_back(notification)
    loop har registered observer
        NS->>Obs: onNotification(notification)
        Obs-->>User: [console] "X ne aapki post like ki"
    end
    Sys-->>User: true
```

> ⭐ **`reactions_` ek Map hai** (`userId → ReactionType`), set/list nahi — isliye ek user ka
> ek hi reaction rehta hai. Dobara react karne pe purana **update** hota hai (LIKE → LOVE),
> duplicate nahi banta. Yahi baat `sharedBy_` (Set) pe bhi lagti hai — ek user ek hi baar share.

---

## 7. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `LinkedinSystem` | ek darwaza; 6 services + saara data andar |
| **Observer** | `INotificationObserver` → `ConsoleNotificationObserver` | notify pe broadcast; email/push observer add ho sakte |
| **Service layer / SRP** | Feed, Search, Analytics, Notification, Messaging, Recommendation | ek class ek feature |
| **Strategy-ready** | `FeedService` (feed ranking) | abhi chronological; ML ranking se replace ho sakta |
| **State machine** | `ConnectionRequest` (5 states) | request lifecycle clean |
| **Repository-lite** | facade ke `unordered_map`s | in-memory store, DB se replace ho sakta |

> ⚠ **Honest notes (interview me bolna):**
> - `MessagingService` saare messages ek flat `vector` me rakhta hai → inbox/conversation
>   O(N) scan. Real me `Map<conversationKey, Message[]>` chahiye.
> - `postsByUser_` `Post*` (raw pointers) rakhta hai → ownership/cleanup ka dhyan.
> - Feed abhi sirf **connections** ki posts leta hai; ranking pure chronological hai
>   (koi relevance/engagement score nahi) — production me yahi sabse bada difference hai.
