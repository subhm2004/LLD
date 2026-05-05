#include <bits/stdc++.h>

#include "core/LinkedinSystem.h"

using namespace std;
using namespace linkedin_lld;

string statusToString(ConnectionStatus status) {
    if (status == ConnectionStatus::PENDING) {
        return "PENDING";
    }
    if (status == ConnectionStatus::ACCEPTED) {
        return "ACCEPTED";
    }
    return "REJECTED";
}

int main() {
    LinkedinSystem system;

    system.registerUser(UserProfile("U1", "Shubham", "Backend Engineer"));
    system.registerUser(UserProfile("U2", "Ananya", "SDE-II"));
    system.registerUser(UserProfile("U3", "Ravi", "Product Designer"));

    const string req1 = system.sendConnectionRequest("U1", "U2");
    const string req2 = system.sendConnectionRequest("U1", "U3");
    const string req3 = system.sendConnectionRequest("U2", "U3");
    system.acceptConnectionRequest(req1);
    system.acceptConnectionRequest(req2);
    system.rejectConnectionRequest(req3);

    cout << "Request " << req1 << " status: " << statusToString(system.getConnectionRequestStatus(req1)) << "\n";
    cout << "Request " << req3 << " status: " << statusToString(system.getConnectionRequestStatus(req3)) << "\n";

    const string post1 = system.createPost("U2", "Built a scalable booking engine in C++.");
    const string post2 = system.createPost("U3", "Design tips for better onboarding flows.");
    const string post3 = system.createPost("U1", "Practicing LLD every day.");

    const bool firstLike = system.likePost("U1", post1);
    const bool duplicateLike = system.likePost("U1", post1);
    system.likePost("U1", post2);
    system.likePost("U2", post3);

    cout << "First like on " << post1 << ": " << (firstLike ? "SUCCESS" : "IGNORED") << "\n";
    cout << "Duplicate like on " << post1 << ": " << (duplicateLike ? "SUCCESS" : "IGNORED") << "\n";

    vector<Post *> feed = system.getFeed("U1", 10);
    cout << "=== Feed for U1 ===\n";
    for (Post *post : feed) {
        cout << post->getPostId() << " | Author: " << post->getAuthorId() << " | Likes: " << post->getLikeCount()
             << " | " << post->getContent() << "\n";
    }

    return 0;
}
