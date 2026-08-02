// ============================================================================
//  #3 — 03_fifo_random_clock.cpp
//  SASTI POLICIES — FIFO, Random, CLOCK
// ----------------------------------------------------------------------------
//  Ab tak humne hit RATE ki baat ki. Par asli system me ek aur kharcha hai
//  jo aksar bhula diya jaata hai: HIT PATH KA KAAM.
//
//  ⭐ Sochiye ek cache jo second me 10 lakh requests jhel raha hai. Har HIT pe
//     LRU ko list me node utha kar front pe le jaana padta hai. Aur agar kai
//     threads hain to us list pe LOCK lagana padta hai — matlab saare threads
//     ek line me khade ho jaate hain. Hit rate 2% behtar hai, par throughput
//     aadha reh gaya. Ye sauda ghaate ka hai.
//
//  Isi liye asli systems aksar "thodi kam accuracy, bahut zyada speed" wali
//  policies chunte hain. Ye file unhi teen ki hai:
//
//     FIFO   -> hit pe KUCH nahi karta (sabse sasta)
//     Random -> hit pe KUCH nahi karta, evict bhi O(1)
//     CLOCK  -> hit pe sirf EK BIT likhta hai (LRU ka sasta andaaza)
//
//  Compile: g++ -std=c++17 -Wall -Wextra -O2 03_fifo_random_clock.cpp -o cheap_demo
// ============================================================================
#include <bits/stdc++.h>

#include "cache_common.h"
#include "policies/clock_cache.h"
#include "policies/fifo_cache.h"
#include "policies/lru_cache.h"
#include "policies/random_cache.h"

using namespace std;
using namespace cache;

int main()
{
    cout << "=========================================================\n";
    cout << " #3 : SASTI POLICIES — FIFO, Random, CLOCK\n";
    cout << "=========================================================\n\n";

    // ---- Part A: FIFO vs LRU ka farak ------------------------------------
    cout << "[A] FIFO aur LRU ka asli farak (dono queue jaise dikhte hain):\n\n";
    {
        FifoCache fifo(3);
        LruCache lru(3);

        vector<string> trace = {"A", "B", "C", "A", "D"};
        cout << "    Cache size 3. Trace: A B C A D\n\n";
        cout << "      access | FIFO | LRU\n";
        cout << "      -------+------+-----\n";
        for (const string &key : trace)
        {
            bool f = fifo.access(key);
            bool l = lru.access(key);
            cout << "        " << key << "    | " << (f ? "HIT " : "miss") << " | "
                 << (l ? "HIT " : "miss") << "\n";
        }
        cout << "\n    ⭐ 'D' aane pe dono ne alag cheez nikaali:\n";
        cout << "       LRU  -> 'B' nikaala (kyunki A abhi-abhi hit hua tha)\n";
        cout << "       FIFO -> 'A' nikaala (wo sabse pehle AAYA tha — hit se koi\n";
        cout << "               farak nahi padta FIFO ko)\n";
        cout << "\n       Yahi ek line ka farak hai: LRU hit pe order badalta hai,\n";
        cout << "       FIFO nahi. Aur isi se hit rate me kaafi farak aata hai.\n\n";
    }

    // ---- Part B: normal traffic pe teeno ka haal -------------------------
    cout << "[B] Normal traffic (Zipf) pe hit rate:\n\n";
    {
        const size_t CAPACITY = 100;
        vector<string> zipf = makeZipfWorkload(5000, 200000);

        LruCache lru(CAPACITY);
        ClockCache clock(CAPACITY);
        FifoCache fifo(CAPACITY);
        RandomCache random(CAPACITY);

        double lruHit = runWorkload(lru, zipf);
        double clockHit = runWorkload(clock, zipf);
        double fifoHit = runWorkload(fifo, zipf);
        double randomHit = runWorkload(random, zipf);

        cout << "      policy | hit rate | LRU ka kitna % | hit path pe kaam\n";
        cout << "      -------+----------+----------------+------------------------\n";
        cout << "      LRU    | " << fixed << setprecision(2) << setw(7) << lruHit << "% | "
             << setw(13) << "100.0"
             << "% | list me node move (LOCK!)\n";
        cout << "      CLOCK  | " << setw(7) << clockHit << "% | " << setw(13) << setprecision(1)
             << (100.0 * clockHit / lruHit) << "% | ek bit likhna ⭐\n";
        cout << "      FIFO   | " << setw(7) << setprecision(2) << fifoHit << "% | " << setw(13)
             << setprecision(1) << (100.0 * fifoHit / lruHit) << "% | kuch nahi\n";
        cout << "      Random | " << setw(7) << setprecision(2) << randomHit << "% | " << setw(13)
             << setprecision(1) << (100.0 * randomHit / lruHit) << "% | kuch nahi\n";

        cout << "\n    ⭐⭐ CLOCK dekho — LRU ka " << setprecision(1)
             << (100.0 * clockHit / lruHit) << "% hit rate deta hai, par hit path pe\n";
        cout << "       sirf EK BIT likhta hai (koi list, koi lock nahi).\n";
        cout << "       Yahi wajah hai ki Linux ka page cache aur PostgreSQL ka buffer\n";
        cout << "       manager exact LRU nahi, CLOCK-family algorithm use karte hain.\n";
        cout << "       Thodi si accuracy ka nuksaan, bahut bada throughput ka faayda.\n\n";
    }

    // ---- Part C: ⭐ Random ka chaunkane wala jeet ----------------------
    cout << "[C] ⭐ Ab wo case jahan RANDOM sabko harata hai:\n\n";
    {
        const size_t CAPACITY = 100;
        vector<string> loop = makeLoopWorkload(120, 200);

        LruCache lru(CAPACITY);
        ClockCache clock(CAPACITY);
        FifoCache fifo(CAPACITY);
        RandomCache random(CAPACITY);

        cout << "    Workload: 120 keys pe baar-baar loop, cache 100 ka\n\n";
        cout << "      LRU    : " << fixed << setprecision(2) << runWorkload(lru, loop) << "%\n";
        cout << "      CLOCK  : " << runWorkload(clock, loop) << "%\n";
        cout << "      FIFO   : " << runWorkload(fifo, loop) << "%\n";
        cout << "      Random : " << runWorkload(random, loop) << "%   ⭐⭐\n";
        cout << "      OPTIMAL: " << beladyOptimalHitRate(loop, CAPACITY) << "%\n";

        cout << "\n    ⭐⭐ Ye is folder ka sabse hairaan karne wala nateeja hai.\n";
        cout << "       LRU, CLOCK, FIFO — teeno ZERO. Random 68%.\n";
        cout << "\n       Wajah: LRU/CLOCK/FIFO teeno ek NIYAM se chalte hain, aur ye\n";
        cout << "       workload us niyam ko theek ULTA pakad leta hai. Har baar wo\n";
        cout << "       BILKUL wahi key nikaalte hain jo agli maangi jaane wali thi.\n";
        cout << "\n       Random ke paas koi niyam hi nahi hai — isliye use ulta pakadna\n";
        cout << "       NAMUMKIN hai. Uski 'bewakoofi' hi yahan uska bachav ban gayi.\n";
        cout << "\n    📌 SEEKH: kabhi-kabhi 'smart' hona hi kamzori ban jaata hai.\n";
        cout << "       Isi liye Random ko hamesha baseline me rakho — agar aapki\n";
        cout << "       'smart' policy Random se saaf behtar nahi hai, to uski\n";
        cout << "       complexity ka koi faayda nahi.\n\n";
    }

    // ---- Part D: teeno workloads pe ek saath ----------------------------
    cout << "[D] Teeno workload ek saath — koi bhi policy har jagah nahi jeetti:\n\n";
    {
        const size_t CAPACITY = 100;
        LruCache lru(CAPACITY);
        ClockCache clock(CAPACITY);
        FifoCache fifo(CAPACITY);
        RandomCache random(CAPACITY);

        vector<pair<string, vector<string>>> workloads = {
            {"Zipf (normal traffic)", makeZipfWorkload(5000, 100000)},
            {"Loop (120 keys)", makeLoopWorkload(120, 200)},
            {"Shifting popularity", makeShiftingWorkload(300, 10, 10000)}};

        cout << "      workload              |   LRU  | CLOCK  |  FIFO  | Random\n";
        cout << "      ----------------------+--------+--------+--------+--------\n";
        for (auto &workload : workloads)
        {
            cout << "      " << left << setw(21) << workload.first << right << " | " << setw(5)
                 << fixed << setprecision(1) << runWorkload(lru, workload.second) << "% | "
                 << setw(5) << runWorkload(clock, workload.second) << "% | " << setw(5)
                 << runWorkload(fifo, workload.second) << "% | " << setw(5)
                 << runWorkload(random, workload.second) << "%\n";
        }
        cout << left;
        cout << right;
        cout << "\n    ⭐ Har row ka winner alag hai. Isi liye 'sabse achhi policy'\n";
        cout << "       naam ki koi cheez hoti hi nahi — sirf 'is workload ke liye\n";
        cout << "       sabse achhi' hoti hai.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ⭐ CLOCK — asli winner in teeno me. LRU ka ~97% hit rate, par hit\n";
    cout << "     path pe sirf ek bit. Isi liye OS/DB isi ko use karte hain.\n";
    cout << "  ⭐ Random — sabse achha BASELINE. Koi bhi policy tabhi useful hai\n";
    cout << "     jab wo Random se saaf behtar ho. Aur loop jaise pattern pe wo\n";
    cout << "     sabse 'smart' policies ko bhi hara deta hai.\n";
    cout << "  📌 FIFO — sabse simple, par LRU se lagbhag hamesha peeche. Uska\n";
    cout << "     sudhra roop (CLOCK) hi asal me use hota hai.\n";
    cout << "  ⚠ Hit rate akela paimana nahi hai — hit path ka kharcha aur lock\n";
    cout << "     contention bhi utne hi zaroori hain.\n";
    cout << "\n  Ab wo policy jo khud dekh kar faisla karti hai -> ARC (file 04)\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
