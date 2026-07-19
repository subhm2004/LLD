# Requirements: LinkedIn-Like System

## Platform Requirements (R1–R13) — image/problem se

Ye woh 13 core LinkedIn requirements hain jo requirements-overview image me diye gaye
the. Har requirement ke saamne uska **code location** mapped hai — matlab har point
actually implement ho chuka hai, sirf theory nahi.

| ID | Requirement | Kahan implement hua (code) |
|----|-------------|-----------------------------|
| **R1** | Profile Customization (education, work experience, achievements, skills) | `UserProfile` (fluent `addEducation`/`addExperience`/`addSkill`/`addAchievement`) + `Education`, `Experience` |
| **R2** | Search & Exploration (users, pages, groups dekh/dhoondh sakein) | `SearchService::search()` + `LinkedinSystem::search()` / `viewProfile()` |
| **R3** | Connection Management (send, cancel, accept, ignore requests) | `ConnectionRequest` + `ConnectionStatus` (PENDING/ACCEPTED/REJECTED/**CANCELLED**/**IGNORED**) + facade `send/accept/reject/cancel/ignoreConnectionRequest` |
| **R4** | Following Users (bina connect kiye follow) | `LinkedinSystem::followUser()` — alag one-way `following_` graph |
| **R5** | Insights & Analytics (connections, profile views, post impressions, search appearances) | `AnalyticsService::computeFor()` + `UserProfile` counters + `Post::impressions_` |
| **R6** | Recommendations (request + provide) | `RecommendationService` + `Recommendation` model |
| **R7** | Content Creation (posts banao/share) | `LinkedinSystem::createPost()` + `Post` |
| **R8** | Engagement (reactions, shares, comments, comments pe engage) | `Post` (6-type `ReactionType`, `Comment` with own reactions, `sharedBy_`) |
| **R9** | Messaging (send/receive) | `MessagingService` + `Message` |
| **R10** | Notifications (messages, connection requests, content interactions) | **Observer** `NotificationService` + `INotificationObserver` + `Notification` |
| **R11** | Company Pages (create + follow) | `CompanyPage` + facade `createCompanyPage`/`followCompanyPage` |
| **R12** | Job Listings (page pe jobs + apply) | `JobListing` (inside `CompanyPage`) + `postJob`/`applyToJob` |
| **R13** | Groups (create + join) | `Group` + `createGroup`/`joinGroup` |

> Design patterns discussion ke liye dekho [`design_patterns_used.md`](./design_patterns_used.md).

---

## Functional Requirements (base — existing, still valid)

1. User can register with profile details.
2. User can send connection request to another user.
3. User can accept connection request.
4. Connected users can create posts.
5. Users can like posts.
6. System should provide personalized feed containing own and connections' posts.

## Non-Functional Requirements

1. Design should be modular and extensible.
2. Feed retrieval should be efficient for in-memory usage.
3. Core operations should include validation for invalid entities.
4. Separation of concerns between domain models and feed logic.

## Assumptions

1. Authentication and authorization are out of scope (koi login/password nahi — user IDs se kaam).
2. Persistence/database is out of scope — sab in-memory hai (system band = data gaya).
3. Feed ranking simplified: total engagement (reactions) first, phir recency. ML ranking / ads out of scope.
4. Search linear scan hai (inverted index / search engine out of scope) — demo scale ke liye theek.
5. Messaging storage ek flat list hai (per-user index / DB out of scope).
6. Single-threaded; concurrency / thread-safety out of scope.

> Note: purane version me comments, shares aur notifications "out of scope" the — ab **implement**
> ho chuke hain (R8, R10). Isi tarah search, follow, analytics, recommendations, company pages,
> jobs aur groups bhi ab live hain.
