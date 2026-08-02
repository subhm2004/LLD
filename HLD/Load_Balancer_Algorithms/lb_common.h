// ============================================================================
//  lb_common.h  —  Saare LB algorithms ka COMMON simulator
// ----------------------------------------------------------------------------
//  Har algorithm ki apni file hai, par unka MUQABLA tabhi meaningful hai jab
//  sabko bilkul same traffic mile. Isliye simulation ka code yahan ek jagah hai
//  — har .cpp file sirf apna SELECTION LOGIC likhti hai ("agli request kis
//  server pe?"), baaki sab yahan se aata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SIMULATOR KA MODEL — "processor sharing"                              │
//  │                                                                          │
//  │  Har server har tick me `weight` jitna KAAM kar paata hai. Wo kaam uske   │
//  │  saare active requests me BARABAR bat jaata hai:                          │
//  │                                                                          │
//  │        har request ko per tick = weight / activeConnections               │
//  │                                                                          │
//  │  Iska matlab: server pe jitni zyada requests, har ek utni hi DHEERE       │
//  │  khatam hogi. Bilkul asli server jaisa (CPU sabme bat‑ta hai).            │
//  │                                                                          │
//  │  ⭐ Yahi ek baat poore comparison ko imaandaar banati hai — agar server   │
//  │     overload hone pe SLOW na hota, to Least Connections aur Round Robin   │
//  │     me koi farak hi nahi dikhta. Latency hi asli scoreboard hai.          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ SABSE ZAROORI DESIGN DECISION — LB ko request ka "cost" NAHI dikhta:
//     Selector function ko sirf `clientIp` milta hai, request ka size/duration
//     nahi. Ye jaan-bujh ke hai, kyunki ASLI load balancer ko bhi nahi pata
//     hota ki aane wali request bhaari hai ya halki. Agar hum use cost dikha
//     dete to saare algorithms "cheat" kar lete aur comparison bekaar ho jaata.
//
//  📌 Traffic me jaan-bujh ke MIX rakha hai: zyadatar halki requests, par
//     kuch bahut bhaari ("heavy tail"). Asli duniya aisi hi hoti hai — aur
//     yahi wo cheez hai jo Round Robin ko todti hai.
// ============================================================================
#ifndef LOAD_BALANCER_ALGORITHMS_LB_COMMON_H
#define LOAD_BALANCER_ALGORITHMS_LB_COMMON_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace lb
{

// ============================================================================
//  Server — ek backend server aur uske saare live metrics
// ============================================================================
struct Server
{
    string id;
    int weight = 1; // capacity (per tick kitna kaam kar sakta hai)

    // ---- Live state (LB ye padh sakta hai) --------------------------------
    int activeConnections = 0; // abhi kitni requests chal rahi hain
    double avgLatency = 0.0;   // ab tak ka average response time (EWMA)
    double cpuPercent = 0.0;   // abhi ka CPU load (resource-based ke liye)

    // ---- Statistics (sirf report ke liye) ---------------------------------
    long long assignedRequests = 0;  // kitni requests MILI
    long long completedRequests = 0; // kitni POORI hui
    double totalWorkDone = 0.0;      // kitna asli KAAM kiya (ye count se alag hai!)
    double totalLatency = 0.0;
    int peakConnections = 0;

    double averageLatency() const
    {
        return completedRequests == 0 ? 0.0 : totalLatency / completedRequests;
    }
};

// ============================================================================
//  Selector — yahi har algorithm ka DIL hai
// ----------------------------------------------------------------------------
//  Input : saare servers (unki live state ke saath) + client ka IP
//  Output: kis server ka index chuna
//
//  ⚠ Dhyan do: request ka COST yahan NAHI aata (upar wala note dekho).
// ============================================================================
using Selector = function<size_t(const vector<Server> &, const string &)>;

// ============================================================================
//  Simulation ka result
// ============================================================================
struct SimResult
{
    double avgLatency = 0.0;
    double p95Latency = 0.0; // 95% requests isse tez thi (tail latency)
    double maxLatency = 0.0;
    long long completed = 0;
};

// ---- Client IPs banao (kuch IPs "zyada active" honge — asli duniya jaisa) --
inline vector<string> makeClientIps(int uniqueClients)
{
    vector<string> ips;
    ips.reserve(uniqueClients);
    for (int i = 0; i < uniqueClients; ++i)
    {
        ips.push_back("192.168." + to_string(i / 256) + "." + to_string(i % 256));
    }
    return ips;
}

// ---- IP ko number me badalne ke liye (IP Hash algorithm ke kaam ka) -------
// FNV-1a + murmur finalizer — deterministic aur achhe se bikhra hua.
inline uint32_t hashIp(const string &ip)
{
    uint32_t h = 2166136261u;
    for (unsigned char c : ip)
    {
        h ^= c;
        h *= 16777619u;
    }
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// ============================================================================
//  ⭐ runSimulation — traffic chalao aur naapo
// ----------------------------------------------------------------------------
//  Har tick pe:
//    1. Nayi requests aati hain -> selector se server chuna jaata hai
//    2. Har server apna `weight` jitna kaam apne active requests me baant-ta hai
//    3. Jinka kaam poora ho gaya wo complete ho jaati hain (latency record)
// ============================================================================
inline SimResult runSimulation(vector<Server> &servers,
                               const Selector &selector,
                               int totalTicks = 1500,
                               int arrivalsPerTick = 3,
                               unsigned seed = 42u)
{
    struct ActiveRequest
    {
        size_t serverIndex;
        double remainingWork;
        int startTick;
    };

    vector<ActiveRequest> active;
    vector<double> latencies;
    vector<string> clientIps = makeClientIps(500);

    mt19937 rng(seed);
    uniform_int_distribution<int> clientPick(0, static_cast<int>(clientIps.size()) - 1);
    uniform_real_distribution<double> costRoll(0.0, 1.0);
    uniform_real_distribution<double> lightCost(1.0, 4.0);
    uniform_real_distribution<double> heavyCost(25.0, 60.0);

    for (int tick = 0; tick < totalTicks; ++tick)
    {
        // ---- 1. Nayi requests -------------------------------------------
        // Aakhri kuch ticks me nayi requests band — taaki jo chal rahi hain
        // wo poori ho sakein (warna latency ka data adhoora reh jaata).
        if (tick < totalTicks - 300)
        {
            for (int a = 0; a < arrivalsPerTick; ++a)
            {
                const string &clientIp = clientIps[clientPick(rng)];

                // ⭐ 15% requests BHAARI hain (heavy tail) — yahi asli duniya hai
                //    aur yahi Round Robin ko todta hai.
                double work = (costRoll(rng) < 0.15) ? heavyCost(rng) : lightCost(rng);

                size_t chosen = selector(servers, clientIp);

                servers[chosen].activeConnections++;
                servers[chosen].assignedRequests++;
                servers[chosen].peakConnections =
                    max(servers[chosen].peakConnections, servers[chosen].activeConnections);

                active.push_back({chosen, work, tick});
            }
        }

        // ---- 2. Har server apna kaam apne requests me baanto -------------
        // ⭐ Yahi "processor sharing" hai: jitni zyada requests, utna kam
        //    hissa har ek ko -> overloaded server SLOW ho jaata hai.
        for (ActiveRequest &request : active)
        {
            const Server &server = servers[request.serverIndex];
            if (server.activeConnections > 0)
            {
                request.remainingWork -= static_cast<double>(server.weight) / server.activeConnections;
            }
        }

        // ---- 3. Jo poori ho gayi unhe hatao ------------------------------
        for (size_t i = 0; i < active.size();)
        {
            if (active[i].remainingWork <= 0.0)
            {
                Server &server = servers[active[i].serverIndex];
                double latency = (tick + 1) - active[i].startTick;

                server.activeConnections--;
                server.completedRequests++;
                server.totalLatency += latency;
                server.totalWorkDone += 1.0;

                // EWMA — naya sample 20%, purana average 80%. Isi tarah asli
                // systems bhi "recent" latency track karte hain.
                server.avgLatency = (server.avgLatency == 0.0)
                                        ? latency
                                        : (0.8 * server.avgLatency + 0.2 * latency);

                latencies.push_back(latency);

                active[i] = active.back(); // O(1) removal (order matter nahi karta)
                active.pop_back();
            }
            else
            {
                ++i;
            }
        }

        // ---- 4. CPU metric update (resource-based algorithm ke liye) -----
        // Load = kitna kaam pending hai vs server ki capacity. 100% pe cap.
        // (Divisor 0.5 se aaram se busy server ~100% pe pahunchta hai.)
        for (Server &server : servers)
        {
            double load = (100.0 * server.activeConnections) / (server.weight * 0.5);
            server.cpuPercent = min(100.0, load);
        }
    }

    // ---- Final numbers -----------------------------------------------------
    SimResult result;
    result.completed = static_cast<long long>(latencies.size());
    if (!latencies.empty())
    {
        double sum = 0.0;
        for (double latency : latencies)
        {
            sum += latency;
        }
        result.avgLatency = sum / latencies.size();

        sort(latencies.begin(), latencies.end());
        result.p95Latency = latencies[static_cast<size_t>(latencies.size() * 0.95)];
        result.maxLatency = latencies.back();
    }
    return result;
}

// ---- Server-wise table print ----------------------------------------------
inline void printServerTable(const vector<Server> &servers)
{
    cout << "    server | weight | requests | avg latency | peak conns\n";
    cout << "    -------+--------+----------+-------------+-----------\n";
    for (const Server &server : servers)
    {
        cout << "    " << left << setw(6) << server.id << " | " << right << setw(6) << server.weight
             << " | " << setw(8) << server.assignedRequests << " | " << setw(11) << fixed
             << setprecision(2) << server.averageLatency() << " | " << setw(10)
             << server.peakConnections << "\n"
             << left;
    }
    cout << right;
}

// ---- Overall result print --------------------------------------------------
inline void printResult(const SimResult &result)
{
    cout << "    Completed requests : " << result.completed << "\n";
    cout << "    Avg latency        : " << fixed << setprecision(2) << result.avgLatency << " ticks\n";
    cout << "    p95 latency        : " << result.p95Latency << " ticks   <-- tail latency\n";
    cout << "    Max latency        : " << result.maxLatency << " ticks\n";
}

// ============================================================================
//  Standard server sets
// ----------------------------------------------------------------------------
//  ⚠ CAPACITY vs ARRIVAL RATE ka hisaab (ye theek na ho to demo bekaar):
//
//    Average request ka kaam = 0.85 x 2.5  +  0.15 x 42.5  ≈  8.5 units
//    Arrivals                = 3 per tick
//    => Demand               ≈ 25.5 work units per tick
//
//    Isliye total capacity (sab weights ka jod) 40 rakhi hai
//    => utilization ≈ 64%
//
//  ⭐ Ye number JAAN-BUJH KE chuna hai. Agar capacity demand se KAM hoti to
//     har server bas queue karta rehta aur SAARE algorithms ek jaise bure
//     lagte (system collapse me algorithm ka koi role nahi bachta). Aur agar
//     capacity bahut ZYADA hoti to koi queue banti hi nahi aur saare algorithms
//     ek jaise ACHHE lagte. Farak sirf beech me dikhta hai — 60-80% load pe.
//     Asli production systems bhi isi range me chalte hain.
// ============================================================================

// ---- 4 servers, sab barabar capacity ke (total 40) ------------------------
inline vector<Server> makeEqualServers()
{
    return {{"S1", 10, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S2", 10, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S3", 10, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S4", 10, 0, 0, 0, 0, 0, 0, 0, 0}};
}

// ---- 4 servers, ALAG capacity — 4:1 ka ratio (total wahi 40) --------------
inline vector<Server> makeWeightedServers()
{
    return {{"S1-big", 16, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S2-big", 16, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S3-sml", 4, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S4-sml", 4, 0, 0, 0, 0, 0, 0, 0, 0}};
}

// ---- 3 healthy + 1 BIMAAR server -----------------------------------------
//  ⭐ Ye set Least Response Time / Resource-based ke liye hai.
//
//  Kahani: S3 machine me kuch kharabi hai (disk dying, koi aur process CPU kha
//  raha hai, ya wo doosre data-center me hai). Uski asli capacity sirf 2 hai
//  jabki baaki 10 ke hain — yaani wo 5x DHEERA hai.
//
//  ⚠ SABSE ZAROORI BAAT: LOAD BALANCER KO YE PATA NAHI HAI.
//     Config me sabka weight barabar likha hai. Koi ne LB ko nahi bataya ki
//     S3 bimaar hai — kyunki asli duniya me bhi koi nahi batata. Server
//     dheere-dheere kharab hota hai, aur config purani padi rehti hai.
//
//  Isi liye:
//     * Least Connections   -> S3 ko barabar traffic deta rahega (use farak
//                              hi nahi dikhta) ❌
//     * Least Response Time -> S3 ka response time badhta dekhega aur usse
//                              apne aap bachna shuru kar dega ✅
inline vector<Server> makeServersWithSickNode()
{
    return {{"S1", 10, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S2", 10, 0, 0, 0, 0, 0, 0, 0, 0},
            {"S3-sick", 2, 0, 0, 0, 0, 0, 0, 0, 0}, // 5x dheera!
            {"S4", 10, 0, 0, 0, 0, 0, 0, 0, 0}};
}

} // namespace lb

#endif // LOAD_BALANCER_ALGORITHMS_LB_COMMON_H
