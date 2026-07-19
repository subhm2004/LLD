// ============================================================================
//  main.cpp  —  LinkedIn LLD ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Har scene image ke ek requirement (R1..R13) ko CHALA ke dikhata hai — sirf
//  likha nahi, asli output me. Dhyaan do client kitna kam jaanta hai (Facade):
//  wo sirf `LinkedinSystem` ke methods bulata hai, andar ki 6 services + 10
//  models ka naam tak nahi aata.
//
//  ============================================================================
//   DEMO PLAN — kaunsa scene kaunsi requirement dikhata hai
//  ----------------------------------------------------------------------------
//     Setup    -> R1   Rich profiles (education, experience, skills)
//     Scene 1  -> R3   Connection: send / accept / reject / cancel / ignore
//     Scene 2  -> R4   Follow (bina connect kiye)
//     Scene 3  -> R7,R8 Post banao; react (6 types), comment, react-comment, share
//     Scene 4  -> FR6  Personalized feed (ranked by engagement)
//     Scene 5  -> R9   Messaging (send + inbox)
//     Scene 6  -> R2   Search (users / pages / groups)
//     Scene 7  -> R6   Recommendations (request + provide)
//     Scene 8  -> R11,R12 Company page + follow + job + apply
//     Scene 9  -> R13  Groups (create + join)
//     Scene 10 -> R5   Analytics (connections, views, impressions, search appears)
//     Scene 11 -> R10  Notifications inbox (real-time pings upar dikhte rahe)
//  ============================================================================
#include <iostream>
#include <string>
#include <vector>

#include "core/LinkedinSystem.h"

using namespace std;
using namespace linkedin_lld;

// Chhota helper — har scene ka heading saaf dikhe.
static void scene(const string &title) {
    cout << "\n\n==================================================================\n";
    cout << "  " << title << "\n";
    cout << "==================================================================\n";
}

int main() {
    LinkedinSystem system;

    // ------------------------------------------------------------------------
    //  R10 — Notification observer sabse pehle register (Observer pattern).
    //  Ab jahan bhi koi event hoga, ye console pe turant 🔔 print kar dega.
    // ------------------------------------------------------------------------
    auto consoleObserver = make_shared<ConsoleNotificationObserver>();
    system.registerNotificationObserver(consoleObserver);

    // ========================================================================
    //  SETUP + R1 — rich profiles (education, work experience, skills, achievements)
    // ========================================================================
    scene("SETUP + R1 — Profile Customization");

    system.registerUser(UserProfile("U1", "Shubham", "Backend Engineer")
                            .addEducation({"IIT Delhi", "B.Tech CSE", "2021"})
                            .addExperience({"SDE-II", "Google", "2022 - Present"})
                            .addSkill("C++")
                            .addSkill("System Design")
                            .addAchievement("Built a scalable booking engine"));

    system.registerUser(UserProfile("U2", "Ananya", "SDE-II @ Amazon")
                            .addEducation({"BITS Pilani", "B.E. CS", "2020"})
                            .addSkill("Distributed Systems")
                            .addSkill("Java"));

    system.registerUser(UserProfile("U3", "Ravi", "Product Designer")
                            .addExperience({"Sr. Designer", "Figma", "2021 - Present"})
                            .addSkill("UX"));

    // R1: ek profile ka "comprehensive summary" print karke dikhao
    const UserProfile &p = system.getUserProfile("U1");
    cout << "Profile: " << p.getName() << " — " << p.getHeadline() << "\n";
    cout << "  Education:\n";
    for (const Education &e : p.getEducations()) {
        cout << "    - " << e.getDegree() << ", " << e.getSchool() << " (" << e.getYear() << ")\n";
    }
    cout << "  Experience:\n";
    for (const Experience &e : p.getExperiences()) {
        cout << "    - " << e.getTitle() << " @ " << e.getCompany() << " (" << e.getDuration() << ")\n";
    }
    cout << "  Skills: ";
    for (const string &s : p.getSkills()) {
        cout << s << "  ";
    }
    cout << "\n  Achievements: ";
    for (const string &a : p.getAchievements()) {
        cout << a;
    }
    cout << "\n";

    // ========================================================================
    //  SCENE 1 — R3 Connection Management: send / accept / reject / cancel / ignore
    // ========================================================================
    scene("SCENE 1 — R3 Connection Management");

    const string req1 = system.sendConnectionRequest("U1", "U2"); // notify U2
    const string req2 = system.sendConnectionRequest("U1", "U3"); // notify U3
    const string req3 = system.sendConnectionRequest("U2", "U3"); // notify U3

    system.acceptConnectionRequest(req1); // U2 accepts -> U1-U2 connected, notify U1
    system.rejectConnectionRequest(req2); // U3 rejects
    system.ignoreConnectionRequest(req3, "U3"); // U3 ignores U2's request

    cout << "  " << req1 << " -> " << connectionStatusToString(system.getConnectionRequestStatus(req1)) << "\n";
    cout << "  " << req2 << " -> " << connectionStatusToString(system.getConnectionRequestStatus(req2)) << "\n";
    cout << "  " << req3 << " -> " << connectionStatusToString(system.getConnectionRequestStatus(req3)) << "\n";

    // Cancel demo: U3 -> U1 bheji, phir U3 khud cancel kar deta hai
    const string req4 = system.sendConnectionRequest("U3", "U1");
    system.cancelConnectionRequest(req4, "U3");
    cout << "  " << req4 << " -> " << connectionStatusToString(system.getConnectionRequestStatus(req4)) << " (sender ne cancel kiya)\n";

    // Guard demo: already-processed request dobara process nahi hoti
    try {
        system.acceptConnectionRequest(req2); // ye REJECTED hai
        cout << "  ERROR: processed request dobara accept ho gayi!\n";
    } catch (const exception &e) {
        cout << "  Expected guard: " << e.what() << "\n";
    }

    // ========================================================================
    //  SCENE 2 — R4 Follow (bina connect kiye)
    // ========================================================================
    scene("SCENE 2 — R4 Following Users");

    system.followUser("U1", "U3"); // U1 follows U3 (U3 ko connect kiye bina)
    system.followUser("U2", "U3");
    cout << "  U1 -> U3 follow? " << (system.isFollowing("U1", "U3") ? "YES" : "NO") << "\n";
    cout << "  U3 ke followers count (U1,U2): " << system.getFollowing("U1").size() << " (U1 kitno ko follow karta hai)\n";

    // ========================================================================
    //  SCENE 3 — R7 Content + R8 Engagement (reactions/comments/shares)
    // ========================================================================
    scene("SCENE 3 — R7 Content Creation + R8 Engagement");

    const string post1 = system.createPost("U2", "Built a scalable booking engine in C++.");
    const string post2 = system.createPost("U3", "Design tips for better onboarding flows.");
    const string post3 = system.createPost("U1", "Practicing LLD every day.");

    // R8: 6 tarah ke reactions (U2 ke post ko sabse zyada engagement dete hain)
    cout << "\n  -- Reactions (R8) --\n";
    system.reactToPost("U1", post1, ReactionType::CELEBRATE); // notify U2
    system.reactToPost("U3", post1, ReactionType::INSIGHTFUL);
    system.likePost("U1", post2); // backward-compatible like = LIKE reaction

    // Duplicate reaction check
    const bool first = system.reactToPost("U3", post2, ReactionType::LOVE);
    const bool dup = system.reactToPost("U3", post2, ReactionType::LOVE);
    cout << "  React on " << post2 << ": first=" << (first ? "OK" : "IGNORED")
         << ", duplicate=" << (dup ? "OK" : "IGNORED") << "\n";

    // R8: comment + "engage with comment" (comment pe react)
    cout << "\n  -- Comments (R8) --\n";
    const string cmt1 = system.commentOnPost("U1", post1, "Zabardast kaam!"); // notify U2
    system.reactToComment("U2", post1, cmt1, ReactionType::LIKE);             // notify U1 (comment author)

    // R8: share
    cout << "\n  -- Shares (R8) --\n";
    system.sharePost("U1", post2); // notify U3

    const Post *pp = system.getPost(post1);
    cout << "\n  " << post1 << " -> reactions=" << pp->getReactionCount() << ", comments="
         << pp->getComments().size() << ", shares=" << pp->getShareCount() << "\n";

    // ========================================================================
    //  SCENE 4 — FR6 Personalized Feed (ranked by engagement)
    // ========================================================================
    scene("SCENE 4 — Personalized Feed for U1 (own + connections' posts)");

    // U1 sirf U2 se connected hai (U3 ke saath reject/ignore hua tha), to feed me
    // U1 ke apne post + U2 ke posts aayenge, engagement ke order me.
    vector<Post *> feed = system.getFeed("U1", 10);
    for (Post *post : feed) {
        cout << "  " << post->getPostId() << " | by " << post->getAuthorId()
             << " | reactions=" << post->getReactionCount() << " | " << post->getContent() << "\n";
    }

    // ========================================================================
    //  SCENE 5 — R9 Messaging
    // ========================================================================
    scene("SCENE 5 — R9 Messaging");

    system.sendMessage("U1", "U2", "Hey Ananya, loved your post!"); // notify U2
    system.sendMessage("U2", "U1", "Thanks Shubham! 😊");           // notify U1
    cout << "  U1 ka inbox:\n";
    for (const Message &m : system.getInbox("U1")) {
        cout << "    from " << m.getFromUserId() << ": " << m.getContent() << "\n";
    }

    // ========================================================================
    //  SCENE 6 — R2 Search (users / pages / groups)
    // ========================================================================
    scene("SCENE 6 — R2 Search and Exploration");

    // Company page + group pehle bana dete hain taaki search me dikhein
    const string gpId = system.createGroup("U1", "C++ Developers India");
    const string pgId = system.createCompanyPage("U2", "Google");

    auto result = system.search("e"); // "e" -> Engineer, Google, Developers... sab match
    cout << "  Search 'e' -> users=" << result.userIds.size() << ", pages="
         << result.pageIds.size() << ", groups=" << result.groupIds.size() << "\n";
    for (const string &uid : result.userIds) {
        cout << "    user: " << system.getUserProfile(uid).getName() << " ("
             << system.getUserProfile(uid).getHeadline() << ")\n";
    }

    // ========================================================================
    //  SCENE 7 — R6 Recommendations (request + provide)
    // ========================================================================
    scene("SCENE 7 — R6 Recommendations");

    system.requestRecommendation("U1", "U2"); // U1 chahta hai U2 use recommend kare
    system.provideRecommendation("U2", "U1", "Shubham ek reliable backend engineer hai — highly recommended!");
    cout << "  U1 ke profile pe recommendations:\n";
    for (const Recommendation &r : system.getRecommendationsFor("U1")) {
        cout << "    from " << r.getAuthorId() << ": \"" << r.getText() << "\"\n";
    }

    // ========================================================================
    //  SCENE 8 — R11 Company Page + R12 Job Listings
    // ========================================================================
    scene("SCENE 8 — R11 Company Pages + R12 Job Listings");

    system.followCompanyPage("U1", pgId);
    system.followCompanyPage("U3", pgId);
    const string jobId = system.postJob(pgId, "Senior C++ Engineer", "5+ yrs, LLD strong");
    const bool applied = system.applyToJob("U1", pgId, jobId);
    const bool dupApply = system.applyToJob("U1", pgId, jobId);

    const CompanyPage &page = system.getCompanyPage(pgId);
    cout << "  Page '" << page.getName() << "' followers=" << page.getFollowerCount() << "\n";
    cout << "  Job apply: first=" << (applied ? "OK" : "IGNORED")
         << ", duplicate=" << (dupApply ? "OK" : "IGNORED")
         << ", total applicants=" << page.getJobListings().front().getApplicantCount() << "\n";

    // ========================================================================
    //  SCENE 9 — R13 Groups
    // ========================================================================
    scene("SCENE 9 — R13 Groups");

    system.joinGroup("U2", gpId);
    system.joinGroup("U3", gpId);
    const Group &g = system.getGroup(gpId);
    cout << "  Group '" << g.getName() << "' members=" << g.getMemberCount() << " (creator + joiners)\n";

    // ========================================================================
    //  SCENE 10 — R5 Analytics
    // ========================================================================
    scene("SCENE 10 — R5 Insights and Analytics");

    // U1 ka profile view badhane ke liye U2 uska profile dekhta hai
    system.viewProfile("U2", "U1");
    system.viewProfile("U3", "U1");

    auto a = system.getAnalytics("U1");
    cout << "  U1 analytics:\n";
    cout << "    connections       : " << a.connections << "\n";
    cout << "    profile views     : " << a.profileViews << "\n";
    cout << "    post impressions  : " << a.postImpressions << "\n";
    cout << "    search appearances: " << a.searchAppearances << "\n";

    // ========================================================================
    //  SCENE 11 — R10 Notifications inbox (pings upar chalte rahe; ab summary)
    // ========================================================================
    scene("SCENE 11 — R10 Notifications inbox");

    cout << "  U2 ke paas " << system.getUnreadNotificationCount("U2") << " unread notifications:\n";
    for (const Notification &n : system.getNotifications("U2")) {
        cout << "    [" << notificationTypeToString(n.getType()) << "] " << n.getText() << "\n";
    }

    cout << "\nDemo complete — saare 13 requirements chal gaye. ✅\n";
    return 0;
}
