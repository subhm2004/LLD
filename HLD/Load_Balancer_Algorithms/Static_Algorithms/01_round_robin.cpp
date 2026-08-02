// ============================================================================
//  STATIC #1 — 01_round_robin.cpp
//  ROUND ROBIN — "baari-baari, sabko barabar"
// ----------------------------------------------------------------------------
//  Sabse purana aur sabse simple load balancing algorithm. Requests ko bas
//  baari-baari servers me baant do:
//
//        Req1 -> S1,  Req2 -> S2,  Req3 -> S3,  Req4 -> S4,  Req5 -> S1, ...
//
//  Poora algorithm ek line ka hai:
//
//        chosenServer = nextIndex++ % serverCount
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ YE "STATIC" ALGORITHM KYUN KEHLATA HAI?                               │
//  │                                                                          │
//  │  Kyunki faisla lete waqt ye server ki ABHI KI HAALAT bilkul nahi dekhta.  │
//  │  Server pe 500 requests chal rahi hon ya 0 — Round Robin ko farak nahi    │
//  │  padta, uski baari aayi to request milegi.                               │
//  │                                                                          │
//  │  Sirf ek counter yaad rakhta hai (`nextIndex`) — server ki koi state      │
//  │  nahi. Isliye ye ANDHA (blind) hai, par bahut TEZ aur SASTA bhi.          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ ISKI ASLI KAMZORI — "barabar COUNT" ka matlab "barabar LOAD" nahi     │
//  │                                                                          │
//  │  Round Robin har server ko barabar requests ki GINTI deta hai. Par        │
//  │  requests barabar nahi hoti! Ek request 2ms ki hai, doosri 5 second ki.   │
//  │                                                                          │
//  │  Agar kismat se 3 bhaari requests ek hi server pe chali gayi, to wo       │
//  │  server ragdta rahega jabki baaki khaali baithe honge — aur Round Robin   │
//  │  phir bhi usi ko agli baari pe request dega, kyunki use dikhta hi nahi    │
//  │  ki wo bhara hua hai.                                                    │
//  │                                                                          │
//  │  ⭐ Ye demo wahi naap ke dikhata hai: requests ki ginti lagbhag barabar   │
//  │     hogi, par LATENCY me bada farak — aur peak connections me bhi.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde   : bahut simple, O(1), koi state nahi, koi tracking overhead nahi
//  ❌ Nuksan   : server ki load/capacity dekhta hi nahi
//  📌 Kab use  : jab saare servers EK JAISE hon AUR requests bhi lagbhag ek jaisi
//                (jaise simple stateless API jo har baar utna hi kaam karti hai)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 01_round_robin.cpp -o rr_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " STATIC #1 : ROUND ROBIN\n";
    cout << " 4 identical servers | baari-baari distribution\n";
    cout << "=========================================================\n\n";

    // ---- Part A: algorithm khud dikhao (pehli 12 requests) ---------------
    cout << "[A] Algorithm kaise chalta hai (pehli 12 requests):\n\n";
    {
        vector<string> serverNames = {"S1", "S2", "S3", "S4"};
        size_t nextIndex = 0;
        cout << "    ";
        for (int i = 1; i <= 12; ++i)
        {
            // ⭐ POORA ALGORITHM YAHI HAI — ek counter, ek modulo.
            string chosen = serverNames[nextIndex % serverNames.size()];
            ++nextIndex;
            cout << "Req" << i << "->" << chosen << (i < 12 ? ", " : "\n");
        }
        cout << "\n    Bilkul saaf cycle: S1, S2, S3, S4, phir wapas S1...\n";
        cout << "    Server ki haalat kahin nahi dekhi gayi — bas counter ghooma.\n\n";
    }

    // ---- Part B: asli traffic pe chalao ----------------------------------
    cout << "[B] Ab asli traffic (mixed: 85% halki, 15% BHAARI requests):\n\n";

    vector<Server> servers = makeEqualServers();

    // ⭐ Round Robin ka selector — sirf ek counter, aur kuch nahi.
    //    `mutable` isliye ki lambda apna counter badal sake.
    size_t nextIndex = 0;
    Selector roundRobin = [&nextIndex](const vector<Server> &pool, const string &clientIp) -> size_t
    {
        (void)clientIp; // ⭐ Round Robin ko client ki bhi parwah nahi
        size_t chosen = nextIndex % pool.size();
        ++nextIndex;
        return chosen;
    };

    SimResult result = runSimulation(servers, roundRobin);

    printServerTable(servers);
    cout << "\n";
    printResult(result);

    // ---- Part C: kamzori ko naapo ----------------------------------------
    cout << "\n[C] ⭐ ASLI BAAT — count barabar hai, par LOAD nahi:\n\n";

    long long minRequests = LLONG_MAX, maxRequests = 0;
    double minLatency = 1e9, maxLatency = 0.0;
    int maxPeak = 0, minPeak = INT_MAX;
    for (const Server &server : servers)
    {
        minRequests = min(minRequests, server.assignedRequests);
        maxRequests = max(maxRequests, server.assignedRequests);
        minLatency = min(minLatency, server.averageLatency());
        maxLatency = max(maxLatency, server.averageLatency());
        maxPeak = max(maxPeak, server.peakConnections);
        minPeak = min(minPeak, server.peakConnections);
    }

    cout << "    Requests ki GINTI  : " << minRequests << " se " << maxRequests
         << "  -> farak sirf " << (maxRequests - minRequests) << " ka. Perfect ✅\n";
    cout << "    Average LATENCY    : " << fixed << setprecision(2) << minLatency << " se "
         << maxLatency << "  -> farak " << setprecision(1)
         << (minLatency > 0 ? (maxLatency / minLatency) : 0.0) << "x ❌\n";
    cout << "    Peak connections   : " << minPeak << " se " << maxPeak << "  -> ek server pe "
         << (minPeak > 0 ? (1.0 * maxPeak / minPeak) : 0.0) << "x zyada bojh ❌\n";

    cout << "\n    ⭐ Yahi Round Robin ka poora sach hai:\n";
    cout << "       Ginti ke hisaab se ye PERFECT hai — sabko barabar requests di.\n";
    cout << "       Par kaam ke hisaab se TEDHA hai — kyunki har request ka kaam\n";
    cout << "       alag hai, aur RR ko wo dikhta hi nahi.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Simple: ek counter, O(1), zero tracking overhead.\n";
    cout << "  ✅ Requests ki ginti bilkul barabar bat-ti hai.\n";
    cout << "  ❌ Server bhara hai ya khaali — dekhta hi nahi (STATIC).\n";
    cout << "  ❌ Mixed/variable requests pe latency bikhar jaati hai.\n";
    cout << "  ❌ Servers alag capacity ke hon to aur bura (chhota server marega).\n";
    cout << "\n  Alag capacity ka ilaaj -> WEIGHTED ROUND ROBIN (file 02).\n";
    cout << "  Server ki asli load dekhne ka ilaaj -> LEAST CONNECTIONS (dynamic).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
