// ============================================================================
//  STATIC #3 — 03_ip_hashing.cpp
//  IP HASH — "ek client hamesha usi server pe" (session affinity)
// ----------------------------------------------------------------------------
//  Round Robin aur Weighted RR me ek client ki requests har baar ALAG server pe
//  jaati hain. Agar server STATEFUL hai (session uski memory me hai), to ye
//  disaster hai — user login karke agli request pe hi logged out ho jaayega.
//
//  ⭐ Ilaaj: server chunne ke liye counter ki jagah CLIENT KA IP use karo:
//
//        serverIndex = hash(client_ip) % serverCount
//
//  Client ka IP nahi badalta, hash nahi badalta, server nahi badalta.
//  Isi ko SESSION AFFINITY ya STICKY SESSION kehte hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FAAYDE — sirf session hi nahi                                         │
//  │                                                                          │
//  │  1. STICKY SESSIONS — stateful backends kaam kar jaate hain               │
//  │  2. CACHE LOCALITY — ek client ka data baar-baar usi server ke cache me   │
//  │     milta hai (cache hit rate badhta hai). Ye faayda stateless servers    │
//  │     ke liye bhi kaam ka hai!                                             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ TEEN BADI KAMZORIYAN                                                  │
//  │                                                                          │
//  │  1. UNEVEN LOAD — hash clients ko barabar baant-ta hai, par unke TRAFFIC  │
//  │     ko nahi. Ek corporate NAT ke peeche 10,000 log ho sakte hain — unka   │
//  │     ek hi IP hai, to wo sara traffic EK server pe girega.                 │
//  │                                                                          │
//  │  2. SERVER ADD/REMOVE = TABAAHI — `% N` me N badla to lagbhag SAARE       │
//  │     clients ka server badal jaata hai. Matlab sabki sessions ud gayi.     │
//  │     (Ye demo wahi naap ke dikhayega — jawab ~80% aayega.)                 │
//  │                                                                          │
//  │  3. STATIC — server ki live load phir bhi nahi dikhti.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐⭐ POINT #2 KA ILAAJ HI CONSISTENT HASHING HAI:
//      Wahan `% N` ki jagah ek hash RING hota hai, jisme server add/remove pe
//      sirf ~1/N clients hilte hain (na ki 80%). Uska poora runnable code:
//          ../../Consistent_Hashing/   (step-by-step, naap ke)
//
//  📌 Aur sabse achha "ilaaj"? Server ko STATELESS bana do (session Redis/JWT me
//     rakho). Tab affinity ki zaroorat hi khatam, aur koi bhi algorithm chalega.
//     Interview me ye jawab sabse zyada impress karta hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 03_ip_hashing.cpp -o iphash_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " STATIC #3 : IP HASH (session affinity)\n";
    cout << " serverIndex = hash(client_ip) % N\n";
    cout << "=========================================================\n\n";

    vector<string> serverNames = {"S1", "S2", "S3", "S4"};

    // ---- Part A: STICKINESS — yahi iska poora maqsad hai -----------------
    cout << "[A] Same client, 5 alag-alag requests — kahan jaati hain?\n\n";
    {
        vector<string> sampleIps = {"203.0.113.45", "198.51.100.7", "192.0.2.199"};
        for (const string &ip : sampleIps)
        {
            size_t chosen = hashIp(ip) % serverNames.size();
            cout << "    Client " << left << setw(15) << ip << " -> ";
            for (int request = 1; request <= 5; ++request)
            {
                // ⭐ Har baar wahi hisaab, isliye hamesha wahi jawab.
                cout << serverNames[hashIp(ip) % serverNames.size()] << " ";
            }
            cout << "  (hamesha " << serverNames[chosen] << ") ✅\n"
                 << right;
        }
        cout << "\n    ⭐ Yahi session affinity hai — client ka server kabhi nahi badalta.\n";
        cout << "       Isliye stateful server pe uski session zinda rehti hai.\n\n";
    }

    // ---- Part B: distribution — clients barabar, par TRAFFIC? ------------
    cout << "[B] Distribution: 10,000 alag clients ko baanto:\n\n";
    {
        vector<int> clientsPerServer(serverNames.size(), 0);
        for (int i = 0; i < 10000; ++i)
        {
            string ip = "10." + to_string(i / 65536) + "." + to_string((i / 256) % 256) + "." +
                        to_string(i % 256);
            clientsPerServer[hashIp(ip) % serverNames.size()]++;
        }
        for (size_t i = 0; i < serverNames.size(); ++i)
        {
            cout << "    " << serverNames[i] << " : " << setw(5) << clientsPerServer[i]
                 << " clients (" << fixed << setprecision(1)
                 << (100.0 * clientsPerServer[i] / 10000) << "%)\n";
        }
        cout << "\n    ✅ CLIENTS to barabar bat gaye (~25% har server).\n\n";
    }

    // ---- Part C: ⚠ par TRAFFIC barabar nahi — NAT/proxy problem ----------
    cout << "[C] ⚠ Ab asli duniya: sab clients barabar traffic nahi bhejte\n\n";
    {
        // 1000 clients, par unka traffic Zipf jaisa — kuch clients bahut heavy
        // (corporate NAT, mobile carrier gateway, bot, etc.)
        vector<long long> requestsPerServer(serverNames.size(), 0);
        long long totalRequests = 0;

        for (int i = 0; i < 1000; ++i)
        {
            string ip = "172.16." + to_string(i / 256) + "." + to_string(i % 256);
            // Client i ka traffic ~ 1/(i+1) — pehla client sabse bhaari
            long long clientRequests = 100000 / (i + 1);
            requestsPerServer[hashIp(ip) % serverNames.size()] += clientRequests;
            totalRequests += clientRequests;
        }

        cout << "    Wahi 1000 clients, par traffic tedha (NAT/heavy users):\n";
        double maxShare = 0.0, minShare = 100.0;
        for (size_t i = 0; i < serverNames.size(); ++i)
        {
            double share = (100.0 * requestsPerServer[i]) / totalRequests;
            maxShare = max(maxShare, share);
            minShare = min(minShare, share);
            cout << "    " << serverNames[i] << " : " << setw(8) << requestsPerServer[i]
                 << " requests (" << fixed << setprecision(1) << share << "%)\n";
        }
        cout << "\n    ❌ Ab " << setprecision(1) << minShare << "% se " << maxShare
             << "% tak ka farak! Clients barabar the, traffic nahi.\n";
        cout << "       IP Hash ko ye dikhta hi nahi — wo sirf IP hash karta hai.\n\n";
    }

    // ---- Part D: ⭐ SABSE BADI PROBLEM — server add/remove --------------
    cout << "[D] ⭐ SABSE BADI PROBLEM — ek server add/remove karo:\n\n";
    {
        const int TOTAL_CLIENTS = 10000;
        vector<string> clientIps;
        clientIps.reserve(TOTAL_CLIENTS);
        for (int i = 0; i < TOTAL_CLIENTS; ++i)
        {
            clientIps.push_back("10." + to_string(i / 65536) + "." +
                                to_string((i / 256) % 256) + "." + to_string(i % 256));
        }

        // 4 -> 5 servers (ek add kiya)
        int moved = 0;
        for (const string &ip : clientIps)
        {
            if ((hashIp(ip) % 4) != (hashIp(ip) % 5))
            {
                ++moved;
            }
        }
        cout << "    4 -> 5 servers : " << fixed << setprecision(1)
             << (100.0 * moved / TOTAL_CLIENTS) << "% clients ka server BADAL gaya\n";

        // 4 -> 3 servers (ek crash)
        moved = 0;
        for (const string &ip : clientIps)
        {
            if ((hashIp(ip) % 4) != (hashIp(ip) % 3))
            {
                ++moved;
            }
        }
        cout << "    4 -> 3 servers : " << (100.0 * moved / TOTAL_CLIENTS)
             << "% clients ka server BADAL gaya\n";

        cout << "\n    ❌ Matlab ek server add karte hi ~80% users ki SESSION UD GAYI.\n";
        cout << "       Sab dobara login karenge. Ek server add karna = mass logout.\n";
        cout << "\n    ⭐⭐ ISI PROBLEM KO SOLVE KARNE KE LIYE CONSISTENT HASHING BANI:\n";
        cout << "        wahan `% N` ki jagah hash RING hoti hai, aur server add/remove\n";
        cout << "        pe sirf ~1/N clients hilte hain (80% nahi).\n";
        cout << "        Poora runnable code: ../../Consistent_Hashing/\n\n";
    }

    // ---- Part E: asli traffic pe latency ---------------------------------
    cout << "[E] Asli traffic pe performance (wahi simulator):\n\n";
    {
        vector<Server> servers = makeEqualServers();

        // ⭐ IP Hash ka selector — ek line. Koi state nahi, koi counter nahi.
        Selector ipHash = [](const vector<Server> &pool, const string &clientIp) -> size_t
        {
            return hashIp(clientIp) % pool.size();
        };

        SimResult result = runSimulation(servers, ipHash);
        printServerTable(servers);
        cout << "\n";
        printResult(result);
        cout << "\n    Note: yahan clients uniformly random hain, isliye distribution\n";
        cout << "    theek dikhega. Asli duniya me (part C wala) tedha hota hai.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Session affinity — ek client hamesha usi server pe (stateful ke liye).\n";
    cout << "  ✅ Cache locality — client ka data usi server ke cache me milta hai.\n";
    cout << "  ✅ Stateless algorithm — koi counter/state LB me rakhne ki zaroorat nahi\n";
    cout << "     (isliye multi-instance LB me bhi sab same jawab denge).\n";
    cout << "  ❌ Traffic uneven ho sakta hai (NAT ke peeche 10,000 log = ek IP).\n";
    cout << "  ❌ Server add/remove pe ~80% clients remap -> mass session loss.\n";
    cout << "  ❌ STATIC — server ki live load phir bhi nahi dikhti.\n";
    cout << "\n  Remap problem ka ilaaj -> CONSISTENT HASHING (../../Consistent_Hashing/)\n";
    cout << "  Asli ilaaj -> server ko STATELESS banao (session Redis/JWT me).\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
