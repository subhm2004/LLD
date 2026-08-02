// ============================================================================
//  DYNAMIC #1 — 01_least_connections.cpp
//  LEAST CONNECTIONS — "jo sabse khaali hai, usko do"
// ----------------------------------------------------------------------------
//  Yahan se DYNAMIC algorithms shuru hote hain. Static algorithms (Round Robin,
//  WRR, IP Hash) ek baat maante the: "sab requests lagbhag barabar hain".
//  Ye galat hai. Dynamic algorithms is jhoot ko chhod dete hain.
//
//  ⭐ IDEA: request bhejne se pehle DEKHO ki kis server pe abhi sabse kam kaam
//     chal raha hai, aur usi ko do.
//
//        chosenServer = wo server jiska activeConnections sabse KAM hai
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ YE ITNA ACHHA KYUN KAAM KARTA HAI — "self-correcting" hai             │
//  │                                                                          │
//  │  LB ko ye BILKUL NAHI pata ki aane wali request bhaari hai ya halki.      │
//  │  Aur usse zaroorat bhi nahi!                                             │
//  │                                                                          │
//  │  Kyunki: agar koi bhaari request kisi server pe chali gayi, to wo         │
//  │  request LAMBE SAMAY tak "active" rahegi. Matlab us server ka             │
//  │  activeConnections zyada dikhega. Matlab agli requests apne aap doosre    │
//  │  servers pe jaayengi.                                                    │
//  │                                                                          │
//  │  Yaani "request kitni bhaari hai" ye baat activeConnections me apne aap   │
//  │  jhalakti hai. LB ko kuch measure karne ki zaroorat hi nahi — bas ginti   │
//  │  dekhni hai. Yahi is algorithm ki khoobsurti hai. ⭐                      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ KAMZORIYAN                                                            │
//  │                                                                          │
//  │  1. "CONNECTION = LOAD" hamesha sach nahi. HTTP keep-alive me ek          │
//  │     connection khula pada reh sakta hai bina kuch kiye. Tab ginti to      │
//  │     zyada dikhegi par server asal me khaali hoga.                        │
//  │                                                                          │
//  │  2. LB ko STATE rakhni padti hai (har server ki live ginti) — Round Robin │
//  │     ke ek counter se zyada mehnga.                                       │
//  │                                                                          │
//  │  3. MULTI-LB PROBLEM ⭐ — agar 3 load balancers hain, to har ek ko sirf   │
//  │     APNE connections dikhte hain. Teeno ko lagta hai "S1 khaali hai" aur  │
//  │     teeno ek saath S1 pe bhej dete hain. Ise "herd behaviour" kehte hain. │
//  │     Ilaaj: shared state, ya "power of two choices" (neeche note).         │
//  │                                                                          │
//  │  4. NAYA SERVER = THUNDERING HERD — naya server 0 connections ke saath    │
//  │     aata hai, to SAARA naya traffic ek dam usi pe toot padta hai.         │
//  │     Ilaaj: slow start (dheere-dheere weight badhao).                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 POWER OF TWO CHOICES (bahut famous trick): saare servers me se sabse kam
//     wala dhoondhne ki jagah, RANDOM 2 uthao aur unme se kam wala chuno.
//     Lagbhag utna hi achha result, par O(1) aur herd behaviour bhi nahi hoti.
//     Bade systems (Nginx, Envoy) yahi use karte hain.
//
//  ✅ Faayde   : real load-aware, variable-duration requests pe bahut behtar
//  ❌ Nuksan   : state tracking, keep-alive me galat picture, multi-LB issue
//  📌 Kab use  : long-lived connections (WebSocket, DB pools), variable request time
//
//  Compile: g++ -std=c++17 -Wall -Wextra 01_least_connections.cpp -o lc_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " DYNAMIC #1 : LEAST CONNECTIONS\n";
    cout << " 4 identical servers | live connection count dekhta hai\n";
    cout << "=========================================================\n\n";

    // ---- Part A: algorithm ko haath se chalao ----------------------------
    cout << "[A] Algorithm kaise sochta hai:\n\n";
    {
        vector<int> connections = {5, 2, 8, 3};
        vector<string> names = {"S1", "S2", "S3", "S4"};

        cout << "    Abhi ki haalat: ";
        for (size_t i = 0; i < names.size(); ++i)
        {
            cout << names[i] << "=" << connections[i] << " conns" << (i + 1 < names.size() ? ", " : "\n");
        }

        size_t best = 0;
        for (size_t i = 1; i < connections.size(); ++i)
        {
            if (connections[i] < connections[best])
            {
                best = i;
            }
        }
        cout << "    => Sabse kam " << names[best] << " (" << connections[best]
             << " conns) -> agli request usko. ⭐\n";
        cout << "\n    Round Robin yahan ANDHA hota — jiski baari hoti usko de deta,\n";
        cout << "    chahe uspe 8 connections chal rahi hon.\n\n";
    }

    // ---- Part B: baseline — Round Robin ----------------------------------
    cout << "[B] Baseline: ROUND ROBIN (static) wahi traffic pe:\n\n";

    vector<Server> rrServers = makeEqualServers();
    size_t nextIndex = 0;
    Selector roundRobin = [&nextIndex](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        return nextIndex++ % pool.size();
    };
    SimResult rrResult = runSimulation(rrServers, roundRobin);

    printServerTable(rrServers);
    cout << "\n";
    printResult(rrResult);

    // ---- Part C: LEAST CONNECTIONS ---------------------------------------
    cout << "\n[C] Ab LEAST CONNECTIONS (dynamic) — wahi servers, wahi traffic:\n\n";

    vector<Server> lcServers = makeEqualServers();

    // ⭐ POORA ALGORITHM — sabse kam activeConnections wala dhoondho.
    Selector leastConnections = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip; // client kaun hai, isse matlab nahi
        size_t best = 0;
        for (size_t i = 1; i < pool.size(); ++i)
        {
            // ⭐ Yahi wo LIVE STATE hai jo static algorithms nahi dekhte the.
            if (pool[i].activeConnections < pool[best].activeConnections)
            {
                best = i;
            }
        }
        return best;
    };
    SimResult lcResult = runSimulation(lcServers, leastConnections);

    printServerTable(lcServers);
    cout << "\n";
    printResult(lcResult);

    // ---- Part D: muqabla -------------------------------------------------
    cout << "\n[D] ⭐ MUQABLA — wahi servers, wahi traffic, sirf algorithm alag:\n\n";
    cout << "                        | Round Robin | Least Connections\n";
    cout << "    --------------------+-------------+------------------\n";
    cout << "    Avg latency         | " << setw(11) << fixed << setprecision(2)
         << rrResult.avgLatency << " | " << setw(17) << lcResult.avgLatency << "\n";
    cout << "    p95 latency         | " << setw(11) << rrResult.p95Latency << " | " << setw(17)
         << lcResult.p95Latency << "\n";
    cout << "    Max latency         | " << setw(11) << rrResult.maxLatency << " | " << setw(17)
         << lcResult.maxLatency << "\n";

    int rrPeak = 0, lcPeak = 0;
    for (const Server &server : rrServers)
    {
        rrPeak = max(rrPeak, server.peakConnections);
    }
    for (const Server &server : lcServers)
    {
        lcPeak = max(lcPeak, server.peakConnections);
    }
    cout << "    Worst peak conns    | " << setw(11) << rrPeak << " | " << setw(17) << lcPeak << "\n";

    cout << "\n    ⭐⭐ SABSE ZAROORI OBSERVATION — upar dono tables ki 'requests'\n";
    cout << "       column compare karo:\n\n";
    cout << "       Round Robin       : sabko BARABAR ginti (900, 900, 900, 900)\n";
    cout << "       Least Connections : ginti ALAG-ALAG (dekho upar wali table)\n\n";
    cout << "       Aur yahi POINT hai. Least Connections ne jaan-bujh ke kisi server\n";
    cout << "       ko kam requests di aur kisi ko zyada — kyunki uska maqsad barabar\n";
    cout << "       GINTI dena hai hi nahi, barabar KAAM dena hai.\n";
    cout << "       Jis server pe bhaari requests atki thi, usko kam requests mili.\n\n";
    cout << "       Static algorithms 'fairness' ko ginti se naapte hain (galat paimana).\n";
    cout << "       Dynamic algorithms use LOAD se naapte hain (sahi paimana). ⭐\n";

    // ---- Part E: thundering herd ka khatra -------------------------------
    cout << "\n[E] ⚠ Ek khatra: NAYA SERVER = THUNDERING HERD\n\n";
    cout << "    Naya server 0 connections ke saath aata hai. Least Connections\n";
    cout << "    ko wo sabse khaali dikhta hai, to SAARA naya traffic ek dam\n";
    cout << "    usi pe toot padta hai — aur naya server pehle hi minute me gir\n";
    cout << "    sakta hai (uska cache bhi thanda hai, DB connections bhi nahi bane).\n";
    cout << "\n    ✅ Ilaaj: SLOW START — naye server ka weight dheere-dheere\n";
    cout << "       badhao (0% -> 100% kuch minute me). nginx/Envoy me ye built-in hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Server ki ASLI load dekhta hai — yahi 'dynamic' ka matlab.\n";
    cout << "  ✅ Bhaari requests apne aap handle ho jaati hain (self-correcting):\n";
    cout << "     bhaari request lambi chalti -> ginti zyada dikhti -> agli requests\n";
    cout << "     kahin aur chali jaati. LB ko cost jaanne ki zaroorat hi nahi. ⭐\n";
    cout << "  ✅ Tail latency (p95/max) me sabse bada sudhaar.\n";
    cout << "  ❌ Connection = load hamesha nahi (keep-alive idle connections).\n";
    cout << "  ❌ LB ko live state rakhni padti (RR ke ek counter se mehnga).\n";
    cout << "  ❌ Kai LB hon to har ek ko sirf apne connections dikhte (herd problem).\n";
    cout << "  ❌ Naya server = thundering herd (slow start se bachao).\n";
    cout << "\n  Servers ki capacity alag ho to -> WEIGHTED LEAST CONNECTIONS (file 02).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
