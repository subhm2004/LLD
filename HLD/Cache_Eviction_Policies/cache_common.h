// ============================================================================
//  cache_common.h  —  WORKLOADS + runner + Belady ka optimal bound
// ----------------------------------------------------------------------------
//  Is poore folder ki sabse zaroori file yahi hai — aur wajah ye:
//
//  ⭐⭐ "KAUNSI EVICTION POLICY SABSE ACHHI HAI?" — is sawaal ka koi jawab
//      hai hi nahi. Sahi sawaal hai: "MERE WORKLOAD pe kaunsi sabse achhi hai?"
//
//  Kyunki har policy ek ANDAAZA (guess) lagati hai ki "aage kya maanga jaayega".
//  LRU maanta hai "jo abhi use hua wo phir hoga". LFU maanta hai "jo popular
//  hai wo popular rahega". Agar workload us andaaze se milta hai to policy
//  achhi lagti hai, warna buri. Policy khud me achhi ya buri hoti hi nahi.
//
//  Isliye yahan 5 ALAG workloads hain, jo jaan-bujh ke alag-alag policies ko
//  todne ke liye banaye gaye hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ BELADY'S OPTIMAL — "sabse achha jo ho SAKTA tha"                    │
//  │                                                                          │
//  │  1966 me Belady ne sabit kiya ki sabse behtareen policy ye hai:           │
//  │     "us cheez ko nikalo jo SABSE DER BAAD dobara maangi jaayegi"          │
//  │                                                                          │
//  │  Iske liye BHAVISHYA pata hona chahiye — isliye ye asli system me         │
//  │  IMPOSSIBLE hai. Par simulation me hamare paas poora trace pehle se hai,  │
//  │  to hum ise nikaal sakte hain.                                            │
//  │                                                                          │
//  │  ⭐ Iska faayda bahut bada hai: ye ek CEILING deta hai. Agar LRU 42% de   │
//  │     raha hai aur optimal 45% hai, to koi bhi policy 3% se zyada nahi      │
//  │     sudhaar sakti — aage mehnat karna bekaar hai.                         │
//  │     Bina is number ke aap ye jaan hi nahi sakte ki 42% "achha" hai ya     │
//  │     "bura". Yahi is folder ka sabse kaam ka tool hai.                     │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_CACHE_COMMON_H
#define CACHE_EVICTION_POLICIES_CACHE_COMMON_H

#include <cmath>
#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "policies/cache_policy.h"

using namespace std;

namespace cache
{

// ============================================================================
//  WORKLOADS — har ek kisi na kisi policy ko todne ke liye bana hai
// ============================================================================

// ---- 1. ZIPF — asli web traffic jaisa ------------------------------------
//  Kuch cheezein bahut popular, baaki lambi "tail". Har policy ke liye ye
//  sabse aasan workload hai (yahan sab theek karte hain).
inline vector<string> makeZipfWorkload(int distinctKeys, int totalRequests, unsigned seed = 42)
{
    mt19937 rng(seed);
    vector<double> cumulative;
    double sum = 0.0;
    for (int i = 0; i < distinctKeys; ++i)
    {
        sum += 1.0 / (i + 1);
        cumulative.push_back(sum);
    }
    uniform_real_distribution<double> pick(0.0, sum);

    vector<string> trace;
    trace.reserve(totalRequests);
    for (int i = 0; i < totalRequests; ++i)
    {
        double target = pick(rng);
        long index = lower_bound(cumulative.begin(), cumulative.end(), target) - cumulative.begin();
        trace.push_back("k" + to_string(index));
    }
    return trace;
}

// ---- 2. SEQUENTIAL SCAN — ⭐ LRU ka KILLER -------------------------------
//  Ek badi table/file ko shuru se ant tak padhna. Har key sirf EK BAAR aati
//  hai. Isme cache ka koi faayda hai hi nahi — par LRU phir bhi apna poora
//  cache in bekaar keys se bhar deta hai.
inline vector<string> makeScanWorkload(int distinctKeys, int passes = 1)
{
    vector<string> trace;
    trace.reserve(static_cast<size_t>(distinctKeys) * passes);
    for (int pass = 0; pass < passes; ++pass)
    {
        for (int i = 0; i < distinctKeys; ++i)
        {
            trace.push_back("scan" + to_string(i));
        }
    }
    return trace;
}

// ---- 3. ZIPF + beech me SCAN — ⭐ asli duniya wala case -------------------
//  Normal traffic chal raha hai, aur beech-beech me koi analytics/backup job
//  poori table scan kar jaata hai. Sawaal: scan ke baad normal traffic ka
//  hit rate bachta hai ya cache bah gaya?
inline vector<string> makeZipfWithScanWorkload(int hotKeys, int scanKeys, int totalRequests,
                                               int scanEvery, unsigned seed = 42)
{
    mt19937 rng(seed);
    vector<double> cumulative;
    double sum = 0.0;
    for (int i = 0; i < hotKeys; ++i)
    {
        sum += 1.0 / (i + 1);
        cumulative.push_back(sum);
    }
    uniform_real_distribution<double> pick(0.0, sum);

    vector<string> trace;
    int scanCounter = 0;
    for (int i = 0; i < totalRequests; ++i)
    {
        if (i > 0 && i % scanEvery == 0)
        {
            // Ek scan chala do — har baar naye keys (jaise nayi table)
            for (int s = 0; s < scanKeys; ++s)
            {
                trace.push_back("scan" + to_string(scanCounter) + "_" + to_string(s));
            }
            ++scanCounter;
        }
        double target = pick(rng);
        long index = lower_bound(cumulative.begin(), cumulative.end(), target) - cumulative.begin();
        trace.push_back("hot" + to_string(index));
    }
    return trace;
}

// ---- 3b. WORKING SET + SCAN — ⭐ scan pollution ka SAAF demo -------------
//  Ek fix "working set" (jaise 100 keys) jo barabar-barabar maangi jaati hai,
//  aur beech-beech me ek scan.
//
//  ⭐ Ye 3 wale se saaf demo hai. Wajah: Zipf me top 10-20 keys itni garam
//     hoti hain ki wo kisi bhi scan me bach jaati hain, isliye nuksan chhupa
//     rehta hai. Yahan saari 100 keys barabar zaroori hain — to scan ka
//     nuksan poora dikhta hai.
//
//  Cache size ko working set ke BARABAR rakho, tab ye sabse saaf dikhega:
//  bina scan ke ~100% hit, scan ke saath miss rate kai guna.
inline vector<string> makeWorkingSetScanWorkload(int workingSetSize, int scanKeys,
                                                 int totalRequests, int scanEvery,
                                                 unsigned seed = 42)
{
    mt19937 rng(seed);
    uniform_int_distribution<int> pick(0, workingSetSize - 1);

    vector<string> trace;
    int scanCounter = 0;
    for (int i = 0; i < totalRequests; ++i)
    {
        if (scanKeys > 0 && i > 0 && i % scanEvery == 0)
        {
            for (int s = 0; s < scanKeys; ++s)
            {
                trace.push_back("scan" + to_string(scanCounter) + "_" + to_string(s));
            }
            ++scanCounter;
        }
        trace.push_back("hot" + to_string(pick(rng)));
    }
    return trace;
}

// ---- Sirf "hot" keys pe hit rate (scan keys ko ginti me mat lo) ----------
//  ⭐ Scan pollution naapne ka SAHI tareeka yahi hai. Total hit rate me scan
//     ki apni misses mil jaati hain aur asli nuksan chhup jaata hai. Humein
//     ye jaanna hai ki scan ne ASLI KAAM KI keys ka kya haal kiya.
inline double hotKeyHitRate(CachePolicy &policy, const vector<string> &trace)
{
    policy.clear();
    long long hits = 0, total = 0;
    for (const string &key : trace)
    {
        bool hit = policy.access(key);
        if (key.rfind("hot", 0) == 0)
        {
            ++total;
            if (hit)
            {
                ++hits;
            }
        }
    }
    return total == 0 ? 0.0 : (100.0 * hits) / total;
}

// ---- 3c. UNIFORM — koi popularity nahi, sab barabar -----------------------
//  Har key ka chance barabar. Ye "worst case" hai — koi locality hi nahi, to
//  cache ka faayda sirf itna hai ki kuch keys ittefaq se dobara aa jaayein.
//
//  ⭐ Ye ARC ke liye khaas kaam ka hai: agar key-space cache se BAHUT bada ho
//     to koi bhi key dobara hit hone se pehle nikal jaati hai — yaani
//     "frequency" ka koi matlab hi nahi rehta. ARC ko ye khud pata chal jaata
//     hai aur wo recency (T1) ki taraf shift kar jaata hai. 04 wala demo ise
//     naap ke dikhata hai.
inline vector<string> makeUniformWorkload(int distinctKeys, int totalRequests,
                                          unsigned seed = 42)
{
    mt19937 rng(seed);
    uniform_int_distribution<int> pick(0, distinctKeys - 1);
    vector<string> trace;
    trace.reserve(totalRequests);
    for (int i = 0; i < totalRequests; ++i)
    {
        trace.push_back("u" + to_string(pick(rng)));
    }
    return trace;
}

// ---- 4. SHIFTING POPULARITY — ⭐ LFU ka KILLER ---------------------------
//  Traffic phases me chalta hai. Har phase me ek NAYA set popular hota hai,
//  aur purana bilkul dead. (News site, trending topics, viral content.)
//  LFU ke liye ye zeher hai: purane phase ke high counts naye content ko
//  andar aane hi nahi dete.
inline vector<string> makeShiftingWorkload(int keysPerPhase, int phases, int requestsPerPhase,
                                           unsigned seed = 42)
{
    mt19937 rng(seed);
    vector<string> trace;
    trace.reserve(static_cast<size_t>(phases) * requestsPerPhase);

    for (int phase = 0; phase < phases; ++phase)
    {
        vector<double> cumulative;
        double sum = 0.0;
        for (int i = 0; i < keysPerPhase; ++i)
        {
            sum += 1.0 / (i + 1);
            cumulative.push_back(sum);
        }
        uniform_real_distribution<double> pick(0.0, sum);

        for (int i = 0; i < requestsPerPhase; ++i)
        {
            double target = pick(rng);
            long index =
                lower_bound(cumulative.begin(), cumulative.end(), target) - cumulative.begin();
            // ⭐ Har phase ka apna alag key-space — purana bilkul dead
            trace.push_back("p" + to_string(phase) + "_k" + to_string(index));
        }
    }
    return trace;
}

// ---- 5. LOOPING — cache se THODA bada loop -------------------------------
//  Ek hi keys pe baar-baar loop, par loop cache se thoda bada hai.
//  ⭐ Ye LRU ka classic "pathological" case hai: jab tak key wapas aati hai,
//     LRU use theek pehle nikaal chuka hota hai. Hit rate lagbhag ZERO.
inline vector<string> makeLoopWorkload(int loopSize, int loops)
{
    vector<string> trace;
    trace.reserve(static_cast<size_t>(loopSize) * loops);
    for (int l = 0; l < loops; ++l)
    {
        for (int i = 0; i < loopSize; ++i)
        {
            trace.push_back("loop" + to_string(i));
        }
    }
    return trace;
}

// ============================================================================
//  RUNNER — ek policy pe poora trace chalao aur hit rate nikalo
// ============================================================================
inline double runWorkload(CachePolicy &policy, const vector<string> &trace)
{
    policy.clear();
    long long hits = 0;
    for (const string &key : trace)
    {
        if (policy.access(key))
        {
            ++hits;
        }
    }
    return (100.0 * hits) / trace.size();
}

// ============================================================================
//  ⭐⭐ BELADY'S OPTIMAL (OPT / MIN) — theoretical ceiling
// ----------------------------------------------------------------------------
//  Rule: jise SABSE DER BAAD dobara maanga jaayega, usko nikalo.
//        (Aur jo dobara maanga hi nahi jaayega, wo sabse pehle jaayega.)
//
//  Implementation:
//    1. Har key ke saare future positions ki list bana lo
//    2. Har access pe us list se current position hata do
//    3. Evict karna ho -> max-heap se wo key uthao jiska "agla use" sabse door
//
//  ⚠ Heap me purani (stale) entries reh jaati hain, kyunki priority badalti
//     rehti hai. Isliye "lazy deletion": pop karte waqt check karo ki entry
//     abhi bhi valid hai (cache me hai AUR uska nextUse wahi hai).
// ============================================================================
inline double beladyOptimalHitRate(const vector<string> &trace, size_t capacity)
{
    const long long INF = numeric_limits<long long>::max();

    // Step 1: har key kis-kis position pe aati hai
    unordered_map<string, deque<long long>> futurePositions;
    for (long long i = 0; i < static_cast<long long>(trace.size()); ++i)
    {
        futurePositions[trace[i]].push_back(i);
    }

    unordered_set<string> inCache;
    unordered_map<string, long long> nextUse; // key -> agla use kab

    // Max-heap: sabse door wala use pehle nikle
    priority_queue<pair<long long, string>> heap;

    long long hits = 0;

    for (long long i = 0; i < static_cast<long long>(trace.size()); ++i)
    {
        const string &key = trace[i];

        // Is position ko future list se hata do
        deque<long long> &positions = futurePositions[key];
        if (!positions.empty() && positions.front() == i)
        {
            positions.pop_front();
        }
        long long myNextUse = positions.empty() ? INF : positions.front();

        if (inCache.count(key))
        {
            ++hits;
            nextUse[key] = myNextUse;
            heap.push({myNextUse, key}); // nayi priority (purani stale ho jaayegi)
            continue;
        }

        // MISS — jagah chahiye?
        if (inCache.size() >= capacity)
        {
            // ⭐ Lazy deletion: stale entries hatao, phir asli victim uthao
            while (!heap.empty())
            {
                pair<long long, string> top = heap.top();
                if (inCache.count(top.second) && nextUse[top.second] == top.first)
                {
                    break; // ye entry valid hai
                }
                heap.pop();
            }
            if (!heap.empty())
            {
                pair<long long, string> victim = heap.top();
                heap.pop();
                inCache.erase(victim.second);
                nextUse.erase(victim.second);
            }
        }

        inCache.insert(key);
        nextUse[key] = myNextUse;
        heap.push({myNextUse, key});
    }

    return (100.0 * hits) / trace.size();
}

// ---- Ek workload pe saari policies ka result print karo ------------------
inline void printComparison(const string &workloadName, const vector<string> &trace,
                            vector<CachePolicy *> &policies, size_t capacity)
{
    double optimal = beladyOptimalHitRate(trace, capacity);

    cout << "    " << workloadName << "\n";
    cout << "    (" << trace.size() << " requests, cache size " << capacity << ")\n\n";
    cout << "      policy   | hit rate | optimal ka kitna % | bar\n";
    cout << "      ---------+----------+--------------------+--------------------\n";

    for (CachePolicy *policy : policies)
    {
        double hitRate = runWorkload(*policy, trace);
        double ofOptimal = (optimal > 0.0) ? (100.0 * hitRate / optimal) : 0.0;
        int barLength = static_cast<int>(hitRate / 3.0);

        cout << "      " << left << setw(8) << policy->name() << right << " | " << setw(7)
             << fixed << setprecision(2) << hitRate << "% | " << setw(17) << setprecision(1)
             << ofOptimal << "% | " << string(static_cast<size_t>(max(0, barLength)), '#') << "\n";
    }
    cout << "      " << left << setw(8) << "OPTIMAL" << right << " | " << setw(7) << fixed
         << setprecision(2) << optimal << "% | " << setw(17) << "100.0"
         << "% | " << string(static_cast<size_t>(max(0, static_cast<int>(optimal / 3.0))), '#')
         << "  <-- Belady (bhavishya jaan kar)\n";
    cout << left;
    cout << right;
}

} // namespace cache


// ============================================================================
//  ⭐ VERIFICATION — demo ko sirf REPORT nahi, VERIFY bhi karna chahiye
// ----------------------------------------------------------------------------
//  Pehle ye demos numbers print karte the aur hamesha `return 0` dete the.
//  Matlab agar kal koi implementation tod deta, to demo GALAT number print
//  karta aur phir bhi "pass" dikhta — kisi ko pata hi nahi chalta.
//
//  Ab har zaroori invariant `demo::check(...)` se guzarta hai, aur `main()` ke
//  ant me `demo::report()` failure count ke hisaab se EXIT CODE deta hai.
//  Isi wajah se CI in demos ko regression test ki tarah chala sakta hai.
//
//  ⚠ Ye helper har folder ke apne common header me hai (shared file nahi) —
//     taaki har folder standalone rahe aur akela copy kiya ja sake.
// ============================================================================
namespace demo
{

inline int failureCount = 0;

// Ek shart check karo. Fail ho to loud print + ginti badhao.
inline void check(bool condition, const std::string &what)
{
    if (!condition)
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "\n";
    }
}

// Do value barabar honi chahiye
template <typename T, typename U>
inline void checkEqual(T actual, U expected, const std::string &what)
{
    if (!(actual == static_cast<T>(expected)))
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "  (mila " << actual
                  << ", chahiye tha " << expected << ")\n";
    }
}

// Value expected ke aas-paas honi chahiye — theory vs measurement ke liye
inline void checkNear(double actual, double expected, double tolerance,
                      const std::string &what)
{
    if (std::fabs(actual - expected) > tolerance)
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "  (mila " << actual
                  << ", chahiye tha " << expected << " +/- " << tolerance << ")\n";
    }
}

// main() ke ant me: exit 0 = sab theek, exit 1 = kuch toota
inline int report()
{
    if (failureCount == 0)
    {
        std::cout << "\n✅ VERIFY: saare invariants theek hain.\n";
        return 0;
    }
    std::cout << "\n❌ VERIFY: " << failureCount << " invariant TOOT gaye.\n";
    return 1;
}

} // namespace demo

#endif // CACHE_EVICTION_POLICIES_CACHE_COMMON_H
