// ============================================================================
//  #5 — 05_compare_all.cpp
//  SAARI POLICIES x SAARE WORKLOADS — aur Belady ka ceiling
// ----------------------------------------------------------------------------
//  Ab sab kuch ek jagah. Har workload pe har policy, aur har row me BELADY ka
//  optimal — taaki pata chale ki "sabse achha jo ho SAKTA tha" wo kya tha.
//
//  ⭐⭐ IS FILE KA EK HI MAQSAD HAI: ye sabit karna ki "sabse achhi eviction
//      policy" naam ki koi cheez HOTI HI NAHI. Har row ka winner alag hai.
//      Sahi sawaal hamesha "mere workload pe kaunsi achhi hai" hota hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra -O2 05_compare_all.cpp -o compare_demo
// ============================================================================
#include <bits/stdc++.h>

#include "cache_common.h"
#include "policies/arc_cache.h"
#include "policies/clock_cache.h"
#include "policies/fifo_cache.h"
#include "policies/lfu_cache.h"
#include "policies/lru_cache.h"
#include "policies/random_cache.h"

using namespace std;
using namespace cache;

int main()
{
    const size_t CAPACITY = 100;

    cout << "=========================================================\n";
    cout << " #5 : SAARI POLICIES KA POORA MUQABLA\n";
    cout << " cache size = " << CAPACITY << " (har test me)\n";
    cout << "=========================================================\n\n";

    LruCache lru(CAPACITY);
    LfuCache lfu(CAPACITY);
    FifoCache fifo(CAPACITY);
    ClockCache clock(CAPACITY);
    RandomCache random(CAPACITY);
    ArcCache arc(CAPACITY);

    vector<CachePolicy *> policies = {&lru, &lfu, &fifo, &clock, &random, &arc};

    struct Workload
    {
        string name;
        string why;
        vector<string> trace;
    };

    vector<Workload> workloads = {
        {"Zipf (normal web traffic)", "sabse aam case — kuch keys popular",
         makeZipfWorkload(5000, 100000)},
        {"Shifting popularity", "news/trending — LFU ka dushman",
         makeShiftingWorkload(300, 10, 10000)},
        {"Working set + scan", "analytics job cache bahaa deta hai — LRU ka dushman",
         makeWorkingSetScanWorkload(100, 200, 20000, 2000)},
        {"Loop (120 keys)", "cache se thoda bada loop — LRU/FIFO/CLOCK ka dushman",
         makeLoopWorkload(120, 200)},
        {"Uniform (koi locality nahi)", "worst case — cache ka faayda hi kam",
         makeUniformWorkload(2000, 100000)}};

    // ---- Har workload ka detail ------------------------------------------
    for (Workload &workload : workloads)
    {
        cout << "-------------------------------------------------------\n";
        cout << " " << workload.name << "\n";
        cout << " (" << workload.why << ")\n";
        cout << "-------------------------------------------------------\n";
        printComparison("", workload.trace, policies, CAPACITY);
        cout << "\n";
    }

    // ---- Ek hi table me sab ----------------------------------------------
    cout << "=========================================================\n";
    cout << " SAB EK SAATH (hit rate %)\n";
    cout << "=========================================================\n\n";

    cout << "    workload              |  LRU |  LFU | FIFO |CLOCK |RANDOM|  ARC | OPT\n";
    cout << "    ----------------------+------+------+------+------+------+------+------\n";

    vector<string> winners;
    for (Workload &workload : workloads)
    {
        cout << "    " << left << setw(21) << workload.name.substr(0, 21) << right << " |";

        double best = -1.0;
        string bestName;
        for (CachePolicy *policy : policies)
        {
            double hitRate = runWorkload(*policy, workload.trace);
            if (hitRate > best)
            {
                best = hitRate;
                bestName = policy->name();
            }
            cout << setw(5) << fixed << setprecision(1) << hitRate << "|";
        }
        cout << setw(5) << beladyOptimalHitRate(workload.trace, CAPACITY) << "\n";
        winners.push_back(workload.name + " -> " + bestName);
    }
    cout << left;
    cout << right;

    // ---- Winners -----------------------------------------------------------
    cout << "\n    ⭐⭐ HAR WORKLOAD KA WINNER ALAG HAI:\n\n";
    for (const string &winner : winners)
    {
        cout << "       " << winner << "\n";
    }

    cout << "\n=========================================================\n";
    cout << " NATEEJA\n";
    cout << "=========================================================\n\n";

    cout << "    ⭐ 1. \"SABSE ACHHI POLICY\" HOTI HI NAHI\n";
    cout << "       Upar ki list dekho — har workload ka winner alag hai. Jo policy\n";
    cout << "       ek jagah top pe hai, wahi doosri jagah sabse neeche ho sakti hai\n";
    cout << "       (LFU: Zipf pe best, shifting pe barbaad).\n";
    cout << "       Isliye interview me 'main LRU use karunga' bolna adhoora jawab hai.\n";
    cout << "       Sahi jawab: 'pehle access pattern dekhunga, phir chununga'.\n\n";

    cout << "    ⭐ 2. BELADY KA COLUMN SABSE KAAM KA HAI\n";
    cout << "       Wo batata hai ki 'sabse achha jo ho SAKTA tha' kya tha. Agar\n";
    cout << "       aapki policy 40% de rahi hai aur optimal 42% hai, to aur mehnat\n";
    cout << "       karna bekaar hai — problem policy me nahi, CACHE SIZE me hai.\n";
    cout << "       Bina is number ke aap ye jaan hi nahi sakte.\n\n";

    cout << "    ⭐ 3. ARC SABSE 'SAFE' HAI — PAR AMAR NAHI\n";
    cout << "       Zipf, shifting, scan — teeno jagah ARC ya to top pe hai ya top ke\n";
    cout << "       bilkul paas. Wo kabhi LFU ki tarah 13% pe nahi dhah-ta.\n";
    cout << "       Production me aksar yahi chahiye, kyunki aapko nahi pata kal traffic\n";
    cout << "       kaisa hoga. (Yahi 'p99 latency' wali soch hai — average se zyada\n";
    cout << "       zaroori hai ki WORST case kitna bura hai.)\n";
    cout << "\n       ⚠ PAR loop wali row dekho: ARC bhi 0.0% hai. Wahan LRU, LFU, FIFO,\n";
    cout << "         CLOCK, ARC — SAB ZERO hain. Sirf Random bacha (68%).\n\n";

    cout << "    ⭐ 4. AUR YAHI SABSE GEHRI BAAT HAI\n";
    cout << "       Loop wali row koi ittefaq nahi hai. Har DETERMINISTIC policy ek\n";
    cout << "       niyam se chalti hai — aur agar workload us niyam ko theek ULTA\n";
    cout << "       pakad le, to wo policy har baar galat faisla legi. Har. Ek. Baar.\n";
    cout << "\n       ARC 'adaptive' hai, par wo bhi deterministic hai — isliye wo bhi\n";
    cout << "       is jaal me phans jaata hai.\n";
    cout << "\n       Random bach gaya kyunki uske paas koi niyam hai hi NAHI. Use ulta\n";
    cout << "       pakadna namumkin hai.\n";
    cout << "\n       📌 Isi liye asli systems me thoda randomness JAAN-BUJH KE daala\n";
    cout << "          jaata hai — jaise Redis ka 'approximated LRU' (5 random keys\n";
    cout << "          uthao, unme se purani nikalo). Wo exact LRU se thoda kam accurate\n";
    cout << "          hai, par is tarah ke pathological pattern se bacha rehta hai.\n\n";

    cout << "    📌 PRACTICAL SALAH:\n";
    cout << "       * Pata nahi workload kaisa hai?        -> LRU se shuru karo\n";
    cout << "       * Popularity stable hai?               -> LFU (+aging)\n";
    cout << "       * Scan/analytics chalte rehte hain?    -> ARC ya 2Q\n";
    cout << "       * Bahut high throughput chahiye?       -> CLOCK\n";
    cout << "       * Kuch bhi decide nahi kar pa rahe?    -> Random baseline se compare karo\n";
    cout << "\n       Aur sabse zaroori: apne ASLI traffic ka trace le kar ye\n";
    cout << "       comparison khud chalao. Ye demos aapko tareeka de rahe hain,\n";
    cout << "       jawab nahi — jawab aapke apne data me hai.\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: SABSE ZAROORI INVARIANT ----------------------------------
    //  Belady optimal ek UPPER BOUND hai. Koi bhi policy usse aage nikal jaaye
    //  to ya policy me bug hai ya Belady me — dono soorat me build fail hona chahiye.
    for (Workload &w : workloads)
    {
        double optimal = beladyOptimalHitRate(w.trace, CAPACITY);
        for (CachePolicy *policy : policies)
        {
            demo::check(runWorkload(*policy, w.trace) <= optimal + 1e-9,
                        string(policy->name()) + " Belady optimal se upar nahi ja sakta (" +
                            w.name + ")");
        }
    }

    return demo::report();
}
