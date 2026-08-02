// ============================================================================
//  DYNAMIC #4 — 04_resource_based.cpp
//  RESOURCE-BASED (adaptive) — "server se KHUD poocho ki wo kaisa hai"
// ----------------------------------------------------------------------------
//  Ab tak ke saare algorithms BAHAR se andaaza laga rahe the:
//     * Round Robin       -> kuch bhi nahi dekhta
//     * Least Connections -> connections gin ke andaaza
//     * Least Resp Time   -> latency dekh ke andaaza
//
//  Ye sab INDIRECT saboot hain. Resource-based algorithm seedha server ke ANDAR
//  jhaankta hai: uska CPU kitna use ho raha hai, memory kitni bachi hai.
//
//  ⭐ IDEA: har server pe ek chhota AGENT chalta hai jo apni CPU/RAM/disk ki
//     report LB ko bhejta rehta hai (ya LB ek `/health` endpoint poll karta hai).
//     LB usi report ke hisaab se sabse "healthy" server chunta hai.
//
//        chosenServer = jiska CPU (ya composite score) sabse KAM
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ YE SABSE SAHI TASVEER KYUN DETA HAI                                   │
//  │                                                                          │
//  │  Kuch cheezein sirf server ke ANDAR se dikhti hain:                       │
//  │                                                                          │
//  │   * Us server pe koi AUR process (cron job, backup, log rotation) CPU     │
//  │     kha raha hai. Connections kam hain, latency abhi tak theek hai —      │
//  │     par CPU 95% pe hai. Sirf resource-based ise pakad paayega.            │
//  │                                                                          │
//  │   * Memory bhar rahi hai aur server GC/swap me jaane wala hai. Latency    │
//  │     abhi normal hai, par 10 second baad disaster aane wala hai.           │
//  │                                                                          │
//  │  Baaki algorithms problem hone ke BAAD react karte hain (latency badhi,   │
//  │  tab pata chala). Resource-based problem hone se PEHLE dekh leta hai. ⭐  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ KEEMAT — aur ye keemat asli hai                                       │
//  │                                                                          │
//  │  1. INFRASTRUCTURE — har server pe agent chahiye (deploy, monitor,        │
//  │     upgrade karna padega). Ye poora ek system ban jaata hai.              │
//  │                                                                          │
//  │  2. STALE DATA ⭐ — agent har 5-10 second me report bhejta hai. Us beech  │
//  │     me server ka CPU 20% se 95% ja sakta hai. Yaani LB PURANI khabar pe   │
//  │     faisla le raha hai. Isi liye ise akela use karna khatarnak hai.       │
//  │                                                                          │
//  │  3. OSCILLATION — stale data + sab LB ka ek jaisa faisla = saara traffic  │
//  │     ek server pe, phir doosre pe. Traffic jhoolta rehta hai.              │
//  │                                                                          │
//  │  ✅ ISI LIYE PRACTICE ME: resource metrics ko akela faisla lene nahi dete │
//  │     — unhe Least Connections ke SAATH mila ke use karte hain (jaise       │
//  │     neeche part C me kiya hai). Connections turant milte hain (fresh),    │
//  │     CPU dheere milta hai (stale) — dono milkar behtar hain.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde   : load ki sabse sahi tasveer, problem se pehle pata chal jaata
//  ❌ Nuksan   : agents chahiye, data purana hota hai, sabse complex setup
//  📌 Kab use  : heterogeneous workloads, ya jab servers pe LB ke traffic ke
//                alawa bhi kaam chalta ho (batch jobs, cron, shared machines)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 04_resource_based.cpp -o resource_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " DYNAMIC #4 : RESOURCE-BASED (adaptive)\n";
    cout << " Server ke CPU/memory dekh ke faisla\n";
    cout << "=========================================================\n\n";

    // ---- Part A: wo case jo SIRF ye algorithm pakad sakta hai -------------
    cout << "[A] Wo case jo baaki koi algorithm nahi pakad sakta:\n\n";
    {
        vector<string> names = {"S1", "S2", "S3-busy"};
        vector<int> connections = {5, 6, 2};      // S3 pe sabse KAM connections
        vector<double> latency = {3.0, 3.2, 3.1}; // latency bhi normal!
        vector<double> cpu = {40.0, 45.0, 94.0};  // par CPU 94%! (cron job chal raha)

        cout << "    server   | conns | latency | CPU%\n";
        cout << "    ---------+-------+---------+------\n";
        for (size_t i = 0; i < names.size(); ++i)
        {
            cout << "    " << left << setw(8) << names[i] << " | " << right << setw(5)
                 << connections[i] << " | " << setw(7) << fixed << setprecision(1) << latency[i]
                 << " | " << setw(5) << cpu[i] << "\n"
                 << left;
        }
        cout << right;

        cout << "\n    S3-busy pe ek backup/cron job chal raha hai — LB ka traffic to\n";
        cout << "    kam hai, par machine ki CPU lagbhag khatam hai.\n\n";
        cout << "    Least Connections   -> S3-busy chunega (sirf 2 conns)   ❌\n";
        cout << "    Least Response Time -> S3-busy chunega (latency normal) ❌\n";
        cout << "                           (latency abhi normal hai — problem ABHI\n";
        cout << "                            tak dikhi nahi, par aa rahi hai)\n";
        cout << "    Resource-based      -> S1 chunega (CPU 40% sabse kam)   ✅\n";
        cout << "\n    ⭐ Yahi is algorithm ka poora justification hai: wo problem ko\n";
        cout << "       HONE SE PEHLE dekh leta hai, baaki uske baad react karte hain.\n\n";
    }

    // ---- Part B: pure resource-based (CPU only) --------------------------
    cout << "[B] Pure resource-based (sirf CPU dekh ke):\n\n";

    vector<Server> cpuServers = makeEqualServers();
    Selector resourceBased = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        for (size_t i = 1; i < pool.size(); ++i)
        {
            // ⭐ Sabse kam CPU wala server jeetta hai.
            if (pool[i].cpuPercent < pool[best].cpuPercent)
            {
                best = i;
            }
        }
        return best;
    };
    SimResult cpuResult = runSimulation(cpuServers, resourceBased);
    printServerTable(cpuServers);
    cout << "\n";
    printResult(cpuResult);

    // ---- Part C: ⭐ HYBRID — jaisa asli production me hota hai ------------
    cout << "\n[C] ⭐ HYBRID (CPU + connections) — asli production wala tareeka:\n\n";

    vector<Server> hybridServers = makeEqualServers();
    Selector hybrid = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        double bestScore = 1e18;

        for (size_t i = 0; i < pool.size(); ++i)
        {
            // ⭐ Do signal mila ke ek score:
            //    - connections : TURANT milta hai (fresh), par indirect hai
            //    - CPU         : SEEDHA sach hai, par thoda PURANA (stale)
            //    Dono ki kamzoriyan ek doosre ko dhak leti hain.
            double connectionLoad = static_cast<double>(pool[i].activeConnections) / pool[i].weight;
            double cpuLoad = pool[i].cpuPercent / 100.0;

            double score = (0.5 * connectionLoad) + (0.5 * cpuLoad);
            if (score < bestScore)
            {
                bestScore = score;
                best = i;
            }
        }
        return best;
    };
    SimResult hybridResult = runSimulation(hybridServers, hybrid);
    printServerTable(hybridServers);
    cout << "\n";
    printResult(hybridResult);

    // ---- Part D: sabka muqabla -------------------------------------------
    cout << "\n[D] ⭐ SAARE ALGORITHMS EK SAATH (wahi servers, wahi traffic):\n\n";

    // Round Robin
    vector<Server> rrServers = makeEqualServers();
    size_t nextIndex = 0;
    Selector roundRobin = [&nextIndex](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        return nextIndex++ % pool.size();
    };
    SimResult rrResult = runSimulation(rrServers, roundRobin);

    // Least Connections
    vector<Server> lcServers = makeEqualServers();
    Selector leastConnections = [](const vector<Server> &pool, const string &ip) -> size_t
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
    SimResult lcResult = runSimulation(lcServers, leastConnections);

    cout << "    algorithm            | avg latency | p95 | max\n";
    cout << "    ---------------------+-------------+-----+-----\n";
    cout << "    Round Robin (static) | " << setw(11) << fixed << setprecision(2)
         << rrResult.avgLatency << " | " << setw(3) << setprecision(0) << rrResult.p95Latency
         << " | " << setw(3) << rrResult.maxLatency << "\n";
    cout << "    Least Connections    | " << setw(11) << setprecision(2) << lcResult.avgLatency
         << " | " << setw(3) << setprecision(0) << lcResult.p95Latency << " | " << setw(3)
         << lcResult.maxLatency << "\n";
    cout << "    Resource (CPU only)  | " << setw(11) << setprecision(2) << cpuResult.avgLatency
         << " | " << setw(3) << setprecision(0) << cpuResult.p95Latency << " | " << setw(3)
         << cpuResult.maxLatency << "\n";
    cout << "    Hybrid (CPU + conns) | " << setw(11) << setprecision(2)
         << hybridResult.avgLatency << " | " << setw(3) << setprecision(0)
         << hybridResult.p95Latency << " | " << setw(3) << hybridResult.maxLatency << "\n";

    cout << "\n    📌 Is simulation me CPU metric seedha connections se hi banta hai\n";
    cout << "       (lb_common.h dekho), isliye 'CPU only' aur 'Least Connections'\n";
    cout << "       ka result kareeb aayega — ye simulator ki seemaa hai, algorithm\n";
    cout << "       ki nahi. Asli faayda part A wale case me hai, jahan CPU kisi AUR\n";
    cout << "       wajah se bhari hui thi — wo cheez koi simulator aasani se nahi\n";
    cout << "       bana sakta, par production me roz hoti hai. ⭐\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Load ki SABSE SAHI tasveer — seedha server ke andar se.\n";
    cout << "  ✅ Wo problems pakadta hai jo bahar se dikhti hi nahi (cron job,\n";
    cout << "     memory pressure, shared machine pe koi aur process).\n";
    cout << "  ✅ Problem hone se PEHLE react karta hai, baad me nahi.\n";
    cout << "  ❌ Har server pe agent chahiye — poora infra ka kaam.\n";
    cout << "  ❌ Data PURANA hota hai (5-10s), isliye akela use karna khatarnak.\n";
    cout << "  ❌ Sabse complex setup — sirf tab lagao jab sach me zaroorat ho.\n";
    cout << "  ⭐ Practice me HYBRID use karo: fresh signal (connections) +\n";
    cout << "     sach wala signal (CPU) — dono milakar.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
