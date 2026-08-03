// ============================================================================
//  #1 — 01_lru.cpp
//  LRU — sabse popular policy, aur uske do andhe dhabbe
// ----------------------------------------------------------------------------
//  "Jise sabse der se use nahi kiya, usko nikalo."
//
//  Ye default choice hai — Redis, Memcached, har ORM cache, browser cache.
//  Aur aksar sahi bhi hai. Par is demo ka maqsad LRU ki taareef nahi, uske
//  DO ANDHE DHABBE naap kar dikhana hai — kyunki interview me wahi poocha
//  jaata hai, aur production me wahi kaat-te hain.
//
//  ⭐ LRU ka andaaza (assumption): "jo abhi use hua, wo phir use hoga."
//     Jab traffic is andaaze se milta hai, LRU shandaar hai.
//     Jab NAHI milta, LRU sabse buri policies me se ek ban jaata hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra -O2 01_lru.cpp -o lru_demo
// ============================================================================
#include <bits/stdc++.h>

#include "cache_common.h"
#include "policies/fifo_cache.h"
#include "policies/lru_cache.h"
#include "policies/random_cache.h"

using namespace std;
using namespace cache;

int main()
{
    cout << "=========================================================\n";
    cout << " #1 : LRU (Least Recently Used)\n";
    cout << "=========================================================\n\n";

    // ---- Part A: LRU kaam karte hue dekho --------------------------------
    cout << "[A] Chhota sa trace — LRU ka faisla step by step:\n\n";
    {
        LruCache lru(3);
        vector<string> trace = {"A", "B", "C", "A", "D", "B"};

        cout << "    Cache size 3. Trace: A B C A D B\n\n";
        for (const string &key : trace)
        {
            bool hit = lru.access(key);
            cout << "      access(" << key << ") -> " << (hit ? "HIT ✅" : "MISS") << "\n";
        }
        cout << "\n    Dhyan do: 'D' aane pe 'B' nikla, 'C' nahi —\n";
        cout << "    kyunki 'A' abhi-abhi use hua tha aur 'C' usse pehle,\n";
        cout << "    par 'B' sabse purana tha. Isliye aakhri 'B' MISS hui.\n\n";
    }

    // ---- Part B: LRU ki TAAKAT — normal web traffic ----------------------
    cout << "[B] LRU ki taakat — normal web traffic (Zipf):\n\n";
    {
        vector<string> zipf = makeZipfWorkload(5000, 200000);
        const size_t CAPACITY = 100;

        LruCache lru(CAPACITY);
        FifoCache fifo(CAPACITY);
        RandomCache random(CAPACITY);

        cout << "    5000 distinct keys, 2 lakh requests, cache sirf " << CAPACITY << " ka\n";
        cout << "    (yaani cache me sirf " << (100.0 * CAPACITY / 5000) << "% data aa sakta hai)\n\n";
        cout << "      LRU    : " << fixed << setprecision(2) << runWorkload(lru, zipf) << "%\n";
        cout << "      FIFO   : " << runWorkload(fifo, zipf) << "%\n";
        cout << "      Random : " << runWorkload(random, zipf) << "%\n";
        cout << "      OPTIMAL: " << beladyOptimalHitRate(zipf, CAPACITY) << "%  (Belady)\n";

        cout << "\n    ⭐ LRU, FIFO aur Random se saaf aage hai. Wajah: usne 'popular'\n";
        cout << "       keys ko pehchan liya — har hit pe wo key wapas top pe chali\n";
        cout << "       jaati hai, isliye wo kabhi nikalti hi nahi.\n";
        cout << "       FIFO ye nahi kar pata (hit pe wo kuch karta hi nahi).\n\n";
    }

    // ---- Part C: ⚠ ANDHA DHABBA #1 — SEQUENTIAL SCAN -------------------
    cout << "[C] ⚠ Andha dhabba #1 — SEQUENTIAL SCAN (cache pollution):\n\n";
    {
        const size_t CAPACITY = 100;

        // Working set THEEK cache ke barabar — to bina scan ke sab hit hona chahiye
        vector<string> noScan = makeWorkingSetScanWorkload(100, 0, 20000, 1 << 30);
        vector<string> withScan = makeWorkingSetScanWorkload(100, 200, 20000, 2000);

        cout << "    Scenario: application 100 keys baar-baar maang raha hai, aur\n";
        cout << "    cache bhi theek 100 ka hai — yaani sab kuch cache me aa jaana\n";
        cout << "    chahiye. Beech-beech me ek analytics job 200 keys scan kar\n";
        cout << "    jaata hai (wo keys dobara kabhi nahi aayengi).\n\n";

        LruCache lruClean(CAPACITY);
        LruCache lruPolluted(CAPACITY);

        // ⭐ Sirf ASLI KAAM KI (hot) keys pe naapo — scan ki apni misses ginne
        //    se asli nuksan chhup jaata hai.
        double cleanHit = hotKeyHitRate(lruClean, noScan);
        double pollutedHit = hotKeyHitRate(lruPolluted, withScan);

        cout << "      LRU, bina scan ke  : " << fixed << setprecision(2) << cleanHit
             << "% hit   (miss rate " << (100.0 - cleanHit) << "%)\n";
        cout << "      LRU, scan ke saath : " << pollutedHit << "% hit   (miss rate "
             << (100.0 - pollutedHit) << "%)\n";

        double missBefore = 100.0 - cleanHit;
        double missAfter = 100.0 - pollutedHit;
        cout << "\n    ⭐⭐ Hit rate dekh kar lagta hai 'itna bura bhi nahi' — par CACHE\n";
        cout << "       me asli paimana MISS RATE hai, kyunki har miss ek DB query hai:\n";
        cout << "\n           miss rate " << setprecision(2) << missBefore << "%  ->  "
             << missAfter << "%   yaani DATABASE pe "
             << setprecision(1) << (missAfter / missBefore) << "x load!\n";

        cout << "\n    ⭐ Hua kya: scan ki har key LRU ke liye 'sabse naya' hai, to wo\n";
        cout << "       seedha top pe chadh jaati hai aur asli kaam ki keys ko bahar\n";
        cout << "       dhakel deti hai. Scan khatam hone ke baad cache un keys se\n";
        cout << "       bhara hota hai jinki dobara zaroorat hi nahi — aur application\n";
        cout << "       ko apni 100 keys DOBARA load karni padti hain.\n";
        cout << "       Isi ko CACHE POLLUTION kehte hain.\n";
        cout << "\n    📌 Asli duniya me ye roz hota hai: koi `SELECT * FROM orders`\n";
        cout << "       chala deta hai aur poore application ka cache bah jaata hai.\n";
        cout << "       Iska ilaaj ARC hai (file 04) — wo isi test me ~99.4% pe\n";
        cout << "       tikta hai, yaani scan ka asar lagbhag ZERO.\n\n";
    }

    // ---- Part D: ⚠⚠ ANDHA DHABBA #2 — LOOP (0% hit rate!) -------------
    cout << "[D] ⚠⚠ Andha dhabba #2 — LOOPING access (yahan LRU ZERO deta hai):\n\n";
    {
        const size_t CAPACITY = 100;
        vector<string> loop = makeLoopWorkload(120, 200); // loop cache se THODA bada

        LruCache lru(CAPACITY);
        FifoCache fifo(CAPACITY);
        RandomCache random(CAPACITY);

        cout << "    120 keys pe baar-baar loop, par cache sirf 100 ka hai.\n";
        cout << "    (Yaani cache me 83% data aa sakta tha — kaafi lagta hai!)\n\n";

        cout << "      LRU    : " << fixed << setprecision(2) << runWorkload(lru, loop)
             << "%   😱\n";
        cout << "      FIFO   : " << runWorkload(fifo, loop) << "%\n";
        cout << "      Random : " << runWorkload(random, loop) << "%   ⭐\n";
        cout << "      OPTIMAL: " << beladyOptimalHitRate(loop, CAPACITY) << "%\n";

        cout << "\n    ⭐⭐ LRU ka hit rate BILKUL ZERO hai — aur ye ittefaq nahi:\n";
        cout << "       Loop 120 ka hai, cache 100 ka. Jab key #1 dobara maangi jaati\n";
        cout << "       hai, LRU use THEEK PEHLE nikaal chuka hota hai (kyunki wo\n";
        cout << "       sabse purani thi). Har baar. Har single key pe.\n";
        cout << "       LRU ka andaaza yahan 100% ULTA pad raha hai.\n";
        cout << "\n    ⭐⭐ Aur dekho RANDOM ne kya kiya — 68%! Kyunki uske paas koi\n";
        cout << "       'niyam' hai hi nahi jise ye pattern ulta pakad sake. Uski\n";
        cout << "       'bewakoofi' hi yahan uski taakat ban gayi.\n";
        cout << "\n    📌 Isi liye asli systems (MySQL InnoDB, PostgreSQL) plain LRU\n";
        cout << "       use nahi karte — wo scan-resistant variants use karte hain.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Normal traffic (Zipf) pe LRU FIFO/Random se saaf behtar hai.\n";
    cout << "  ✅ O(1) — list + hashmap, aur code chhota.\n";
    cout << "  ❌ SEQUENTIAL SCAN: ek badi query poora cache bahaa deti hai.\n";
    cout << "  ❌ LOOP (cache se thoda bada): hit rate BILKUL 0 — Random se bhi bura.\n";
    cout << "  ⚠ Har hit pe list update karni padti hai — multi-thread me us list\n";
    cout << "     ka lock bottleneck ban jaata hai (isi ka ilaaj CLOCK hai, file 03).\n";
    cout << "\n  LRU 'kab' dekhta hai. 'KITNI BAAR' dekhne wala -> LFU (file 02)\n";
    cout << "  Scan se bachne wala                            -> ARC (file 04)\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: LRU ka DEFINING behaviour (isse FIFO alag hai) -----------
    //  ⭐ Ye check sabse zaroori hai. Upar wale checks (loop=0%, scan pollution)
    //     FIFO bhi pass kar leta hai — to agar koi galti se LRU ka reordering
    //     hata de, wo checks pakad hi nahi paate. Ye trace pakad leta hai:
    //
    //       A B C daale (cache size 3, ab full)
    //       A dobara chhua  -> LRU ke liye ab B sabse purana hai
    //       D daala         -> LRU 'B' nikaalega, FIFO 'A' nikaalega
    {
        LruCache v(3);
        v.access("A");
        v.access("B");
        v.access("C");
        v.access("A"); // A ab sabse naya
        v.access("D"); // is se B nikalna chahiye (A nahi)

        demo::check(v.access("A"), "LRU: A abhi-abhi use hua tha, wo cache me hona chahiye");
        demo::check(!v.access("B"), "LRU: B sabse purana tha, wo nikal jaana chahiye");
    }

    // ---- VERIFY: aur isi wajah se LRU ko FIFO se aage hona chahiye ---------
    {
        vector<string> z = makeZipfWorkload(5000, 100000);
        LruCache a(100);
        FifoCache b(100);
        demo::check(runWorkload(a, z) > runWorkload(b, z),
                    "Zipf traffic pe LRU ko FIFO se behtar hona chahiye (reordering ka faayda)");
    }

    // ---- VERIFY: LRU ke dono andhe dhabbe ---------------------------------
    {
        vector<string> loopTrace = makeLoopWorkload(120, 200);
        LruCache vl(100);
        RandomCache vr(100);
        demo::checkNear(runWorkload(vl, loopTrace), 0.0, 0.01,
                        "cache se bade loop pe LRU ka hit rate 0% hona chahiye");
        demo::check(runWorkload(vr, loopTrace) > 50.0,
                    "usi loop pe Random ko 50%+ dena chahiye (randomness ka bachav)");

        LruCache vc(100), vd(100);
        double clean = hotKeyHitRate(vc, makeWorkingSetScanWorkload(100, 0, 20000, 1 << 30));
        double dirty = hotKeyHitRate(vd, makeWorkingSetScanWorkload(100, 200, 20000, 2000));
        demo::check((100.0 - dirty) > 3.0 * (100.0 - clean),
                    "scan ko LRU ka miss rate kam se kam 3x badha dena chahiye");
    }

    return demo::report();
}
