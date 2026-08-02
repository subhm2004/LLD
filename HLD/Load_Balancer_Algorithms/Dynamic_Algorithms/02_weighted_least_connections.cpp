// ============================================================================
//  DYNAMIC #2 — 02_weighted_least_connections.cpp
//  WEIGHTED LEAST CONNECTIONS — "kam connections" + "capacity ka khayal"
// ----------------------------------------------------------------------------
//  Least Connections ki ek khamosh galti hai: wo maanta hai ki 5 connections
//  har server pe barabar bojh hain. Par agar ek server 16-core ka hai aur doosra
//  4-core ka, to 5 connections dono ke liye BILKUL alag baat hai.
//
//  ⭐ Ilaaj: connections ko seedha compare mat karo — pehle capacity se DIVIDE karo:
//
//        load = activeConnections / weight
//        chosenServer = jiska `load` sabse KAM
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ EK MISAAL SE POORI BAAT SAAF                                          │
//  │                                                                          │
//  │      S1-big : 8 connections,  weight 16  ->  8/16 = 0.50                 │
//  │      S2-sml : 3 connections,  weight 4   ->  3/4  = 0.75                 │
//  │                                                                          │
//  │  Plain Least Connections S2 ko chunta (3 < 8) — aur GALTI karta, kyunki   │
//  │  S2 asal me apni capacity ka 75% bhar chuka hai jabki S1 sirf 50%.        │
//  │                                                                          │
//  │  Weighted Least Connections S1 ko chunta (0.50 < 0.75) — SAHI. ✅         │
//  │                                                                          │
//  │  ⭐ Yaani hum "kitni connections" nahi, "kitna BHARA HUA hai" dekh rahe   │
//  │     hain. Absolute number ki jagah PERCENTAGE. Yahi poora idea hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ DIVISION ka dhyan: integer division mat karna! `8/16` integer me 0 ho
//     jaayega aur `3/4` bhi 0 — dono barabar lagenge aur algorithm toot jaayega.
//     Isliye neeche `static_cast<double>` lagaya hai. Ye chhoti si galti asli
//     code me bahut hoti hai.
//
//  ✅ Faayde   : Least Connections ka sara faayda + alag capacity ka handling
//  ❌ Nuksan   : weight phir bhi haath se set karna padta hai
//  📌 Kab use  : servers alag capacity ke hon AUR requests ka time variable ho
//                (ye sabse common real-world combination hai — isiliye HAProxy
//                aur nginx dono me ye available hai)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 02_weighted_least_connections.cpp -o wlc_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " DYNAMIC #2 : WEIGHTED LEAST CONNECTIONS\n";
    cout << " 2 bade (weight 16) + 2 chhote (weight 4) servers\n";
    cout << "=========================================================\n\n";

    // ---- Part A: wahi misaal, haath se ------------------------------------
    cout << "[A] Kyun plain Least Connections yahan GALAT faisla leta hai:\n\n";
    {
        vector<string> names = {"S1-big", "S2-sml"};
        vector<int> connections = {8, 3};
        vector<int> weights = {16, 4};

        cout << "    server   | conns | weight | conns/weight (kitna BHARA)\n";
        cout << "    ---------+-------+--------+---------------------------\n";
        for (size_t i = 0; i < names.size(); ++i)
        {
            double load = static_cast<double>(connections[i]) / weights[i];
            cout << "    " << left << setw(8) << names[i] << " | " << right << setw(5)
                 << connections[i] << " | " << setw(6) << weights[i] << " | " << setw(8) << fixed
                 << setprecision(2) << load << "\n"
                 << left;
        }
        cout << right;
        cout << "\n    Plain Least Connections -> S2-sml chunega (3 < 8)   ❌ GALAT\n";
        cout << "    Weighted Least Conns    -> S1-big chunega (0.50 < 0.75) ✅ SAHI\n";
        cout << "\n    S2-sml apni capacity ka 75% bhar chuka hai, S1-big sirf 50%.\n\n";
    }

    // ---- Part B: baseline — plain Least Connections ----------------------
    cout << "[B] Baseline: PLAIN Least Connections (capacity ignore karke):\n\n";

    vector<Server> lcServers = makeWeightedServers();
    Selector plainLeastConnections = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        for (size_t i = 1; i < pool.size(); ++i)
        {
            if (pool[i].activeConnections < pool[best].activeConnections)
            {
                best = i;
            }
        }
        return best;
    };
    SimResult lcResult = runSimulation(lcServers, plainLeastConnections);

    printServerTable(lcServers);
    cout << "\n";
    printResult(lcResult);
    cout << "\n    ⭐ Dhyan se dekho — plain LC utna bura nahi kar raha jitna aap\n";
    cout << "       soch rahe the. Usne chhote servers ko KAM requests di hain!\n";
    cout << "       Kyunki chhote server jaldi bhar jaate hain, to unki ginti badh\n";
    cout << "       jaati hai, aur LC unse bachna shuru kar deta hai (self-correcting).\n";
    cout << "\n    ❌ PAR usne galti kahan ki — 'peak conns' column dekho:\n";
    cout << "       SAB servers ka peak lagbhag BARABAR hai (~6).\n";
    cout << "       Yaani plain LC ne sabki CONNECTION GINTI barabar kar di —\n";
    cout << "       chahe kisi ki capacity 16 ho ya 4!\n";
    cout << "       Chhote server ke liye 6 connections matlab wo thusa hua hai,\n";
    cout << "       bade server ke liye 6 matlab wo aaram me hai. Isi liye chhote\n";
    cout << "       servers ki latency lagbhag DUGNI hai (upar table dekho).\n";

    // ---- Part C: WEIGHTED Least Connections ------------------------------
    cout << "\n[C] Ab WEIGHTED Least Connections:\n\n";

    vector<Server> wlcServers = makeWeightedServers();

    // ⭐ POORA ALGORITHM — connections/weight sabse kam wala.
    Selector weightedLeastConnections = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        // ⚠ `static_cast<double>` zaroori — warna integer division sab 0 kar degi.
        double bestLoad = static_cast<double>(pool[0].activeConnections) / pool[0].weight;

        for (size_t i = 1; i < pool.size(); ++i)
        {
            double load = static_cast<double>(pool[i].activeConnections) / pool[i].weight;
            if (load < bestLoad)
            {
                bestLoad = load;
                best = i;
            }
        }
        return best;
    };
    SimResult wlcResult = runSimulation(wlcServers, weightedLeastConnections);

    printServerTable(wlcServers);
    cout << "\n";
    printResult(wlcResult);

    // ---- Part D: muqabla -------------------------------------------------
    cout << "\n[D] ⭐ MUQABLA (wahi servers, wahi traffic):\n\n";
    cout << "                        | Plain LC | Weighted LC\n";
    cout << "    --------------------+----------+------------\n";
    cout << "    Avg latency         | " << setw(8) << fixed << setprecision(2)
         << lcResult.avgLatency << " | " << setw(11) << wlcResult.avgLatency << "\n";
    cout << "    p95 latency         | " << setw(8) << lcResult.p95Latency << " | " << setw(11)
         << wlcResult.p95Latency << "\n";
    cout << "    Max latency         | " << setw(8) << lcResult.maxLatency << " | " << setw(11)
         << wlcResult.maxLatency << "\n";

    cout << "\n    ⭐⭐ ASLI FARAK 'peak conns' COLUMN ME HAI — dono tables compare karo:\n\n";
    cout << "       Plain LC    : sab servers ka peak ~6  (ginti barabar ki)\n";
    cout << "       Weighted LC : bade ~7-8, chhote ~2    (UTILIZATION barabar ki)\n\n";
    cout << "       Aur 8:2 ka ratio = 4:1 — bilkul wahi jo weights ka ratio hai\n";
    cout << "       (16:4). Yaani Weighted LC ne har server ko uski capacity ke\n";
    cout << "       hisaab se BARABAR PERCENT bhara.\n\n";
    cout << "       ⭐ Yahi poore algorithm ka nichod hai:\n";
    cout << "          Plain LC    -> sabki CONNECTION GINTI barabar karta hai (galat)\n";
    cout << "          Weighted LC -> sabki UTILIZATION barabar karta hai (sahi)\n\n";
    cout << "       Isi liye latency ka failaav bhi kam ho gaya — ab chhote aur bade\n";
    cout << "       servers ki latency paas-paas hai, pehle dugni ka farak tha.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Least Connections ka sara faayda, PLUS alag capacity ka khayal.\n";
    cout << "  ✅ Formula bas itna: load = connections / weight (absolute nahi, %).\n";
    cout << "  ✅ Real production me sabse zyada use hone wala dynamic algorithm —\n";
    cout << "     kyunki asli clusters me servers barabar hote hi nahi.\n";
    cout << "  ⚠ Integer division ki galti mat karna (double me cast karo).\n";
    cout << "  ❌ Weight phir bhi haath se set karna padta hai.\n";
    cout << "\n  Weight bhi khud pata chale? -> LEAST RESPONSE TIME (file 03)\n";
    cout << "  ya RESOURCE-BASED (file 04) — wo server se khud poochte hain.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
