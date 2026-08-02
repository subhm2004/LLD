// ============================================================================
//  #2 — 02_lfu.cpp
//  LFU — "kitni baar" dekhta hai, "kab" nahi
// ----------------------------------------------------------------------------
//  LRU dekhta hai: "ye kab use hua?"
//  LFU dekhta hai: "ye kitni BAAR use hua?"
//
//  Ye farak chhota lagta hai par nateeje bilkul alag hote hain. LFU ka andaaza
//  hai: "jo cheez ab tak popular rahi hai, wo aage bhi popular rahegi."
//
//  ⭐ Jab popularity STABLE ho (product catalog, config, static content) to ye
//     andaaza LRU se BEHTAR nikalta hai — aur ye demo wo naap ke dikhata hai.
//
//  ⚠ Par jab popularity BADALTI rehti ho (news, trends, viral content) to yahi
//     andaaza LFU ka sabse bada dushman ban jaata hai.
//
//  Compile: g++ -std=c++17 -Wall -Wextra -O2 02_lfu.cpp -o lfu_demo
// ============================================================================
#include <bits/stdc++.h>

#include "cache_common.h"
#include "policies/lfu_cache.h"
#include "policies/lru_cache.h"

using namespace std;
using namespace cache;

int main()
{
    cout << "=========================================================\n";
    cout << " #2 : LFU (Least Frequently Used)\n";
    cout << "=========================================================\n\n";

    // ---- Part A: LRU se farak, ek chhote trace pe -----------------------
    cout << "[A] LRU aur LFU ka farak — ek hi trace pe:\n\n";
    {
        LruCache lru(3);
        LfuCache lfu(3);

        // A ko baar-baar maango, phir B, C, D
        vector<string> trace = {"A", "A", "A", "A", "B", "C", "D", "A"};

        cout << "    Cache size 3. Trace: A A A A B C D A\n";
        cout << "    (A bahut popular hai — 4 baar aaya. Phir B, C, D naye aate hain.)\n\n";

        cout << "      access | LRU  | LFU\n";
        cout << "      -------+------+-----\n";
        for (const string &key : trace)
        {
            bool lruHit = lru.access(key);
            bool lfuHit = lfu.access(key);
            cout << "        " << key << "    | " << (lruHit ? "HIT " : "miss") << " | "
                 << (lfuHit ? "HIT " : "miss") << "\n";
        }

        cout << "\n    ⭐ Aakhri 'A' pe dhyan do:\n";
        cout << "       LRU ne A nikaal diya tha — kyunki B, C, D uske BAAD aaye the,\n";
        cout << "       to A 'sabse purana' ban gaya. LRU ko farak nahi padta ki A\n";
        cout << "       4 baar maanga gaya tha.\n";
        cout << "       LFU ne A ko rakha — uska count 4 tha, aur B/C/D ka sirf 1.\n\n";
    }

    // ---- Part B: LFU ki TAAKAT — stable popularity ----------------------
    cout << "[B] LFU ki taakat — jab popularity STABLE ho:\n\n";
    {
        const size_t CAPACITY = 100;
        vector<string> zipf = makeZipfWorkload(5000, 200000);

        LruCache lru(CAPACITY);
        LfuCache lfu(CAPACITY);

        double lruHit = runWorkload(lru, zipf);
        double lfuHit = runWorkload(lfu, zipf);
        double optimal = beladyOptimalHitRate(zipf, CAPACITY);

        cout << "    5000 keys, 2 lakh requests, cache 100 (Zipf — popularity fix hai)\n\n";
        cout << "      LRU    : " << fixed << setprecision(2) << lruHit << "%\n";
        cout << "      LFU    : " << lfuHit << "%   ⭐\n";
        cout << "      OPTIMAL: " << optimal << "%\n";

        cout << "\n    ⭐ LFU " << setprecision(1) << (lfuHit - lruHit)
             << " points aage hai — ye bada farak hai.\n";
        cout << "       Wajah: Zipf me kuch keys HAMESHA popular rehti hain. LFU un\n";
        cout << "       keys ke bade counts ki wajah se unhe kabhi nikaalta hi nahi.\n";
        cout << "       LRU unhe nikaal deta hai agar thodi der koi na maange.\n";
        cout << "\n       Optimal ke hisaab se: LRU " << setprecision(1)
             << (100.0 * lruHit / optimal) << "% pe hai, LFU " << (100.0 * lfuHit / optimal)
             << "% pe.\n\n";
    }

    // ---- Part C: ⚠⚠ LFU ka KILLER — badalti popularity ----------------
    cout << "[C] ⚠⚠ Ab LFU ka dushman — jab popularity BADALTI hai:\n\n";
    {
        const size_t CAPACITY = 100;
        // 10 phases, har phase me bilkul NAYE keys popular
        vector<string> shifting = makeShiftingWorkload(300, 10, 20000);

        LruCache lru(CAPACITY);
        LfuCache lfu(CAPACITY);

        double lruHit = runWorkload(lru, shifting);
        double lfuHit = runWorkload(lfu, shifting);
        double optimal = beladyOptimalHitRate(shifting, CAPACITY);

        cout << "    Scenario: 10 'phase' hain. Har phase me bilkul NAYA content\n";
        cout << "    popular hota hai (jaise news site — kal ki khabar aaj dead).\n\n";
        cout << "      LRU    : " << fixed << setprecision(2) << lruHit << "%   ⭐\n";
        cout << "      LFU    : " << lfuHit << "%   😱\n";
        cout << "      OPTIMAL: " << optimal << "%\n";

        cout << "\n    ⭐⭐ Ab poora ULTA ho gaya — LFU " << setprecision(1)
             << (lruHit - lfuHit) << " points PEECHE hai!\n";
        cout << "\n       Hua kya: phase 1 ke keys ka count 500+ ho gaya. Phase 2 ka\n";
        cout << "       naya content count 1, 2, 3 se shuru hota hai. LFU ke liye\n";
        cout << "       'count 500 wali purani cheez' hamesha 'count 3 wali nayi\n";
        cout << "       cheez' se zyada keemti hai — chahe wo purani cheez ab dead ho.\n";
        cout << "\n       Nateeja: cache me MURDA content bhara pada hai, aur naya\n";
        cout << "       content andar aa hi nahi pata. Ise CACHE POLLUTION (ya\n";
        cout << "       'LFU aging problem') kehte hain.\n";
        cout << "\n    ⭐ LRU yahan jeet gaya kyunki wo 'purana' bhool jaata hai —\n";
        cout << "       aur is workload me bhoolna hi sahi strategy thi.\n\n";
    }

    // ---- Part D: ilaaj ---------------------------------------------------
    cout << "[D] To LFU ka ilaaj kya hai?\n\n";
    cout << "    Asli systems plain LFU use karte hi nahi. Do sudhaar milte hain:\n\n";
    cout << "    1️⃣ LFU with AGING (ya 'decay')\n";
    cout << "       Har kuch der me saare counts ko aadha kar do (count /= 2).\n";
    cout << "       Isse purani popularity dheere-dheere bhool jaati hai, aur naya\n";
    cout << "       content mukabla kar pata hai. Redis ka `allkeys-lfu` yahi karta\n";
    cout << "       hai — uske counter me ek time component hota hai.\n\n";
    cout << "    2️⃣ WINDOW-TinyLFU (aajkal ka best)\n";
    cout << "       Frequency ko ek chhote sketch (Count-Min jaisa) me rakho aur\n";
    cout << "       usko time ke saath halka karte raho. Caffeine (Java) aur\n";
    cout << "       kai modern caches yahi use karte hain.\n";
    cout << "       📌 Wahi Count-Min Sketch jo aapke Probabilistic_Data_Structures\n";
    cout << "          folder me hai — yahan uska asli use dikhta hai.\n\n";
    cout << "    3️⃣ Ya phir dono duniya ka faayda lo -> ARC (file 04), jo recency\n";
    cout << "       aur frequency ke beech KHUD balance karta hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Stable popularity pe LFU LRU se saaf behtar hai (upar naapa).\n";
    cout << "  ✅ O(1) — frequency buckets + minFreq ka jugaad.\n";
    cout << "  ❌ Badalti popularity pe LFU LRU se BAHUT peeche gir jaata hai.\n";
    cout << "  ❌ Purana popular content cache me CHIPAK jaata hai (aging problem).\n";
    cout << "  ❌ Ek 'flash crowd' (achanak viral) hamesha ke liye cache me ghus jaati hai.\n";
    cout << "  📌 Isi liye plain LFU production me kam milta hai — LFU+aging ya\n";
    cout << "     TinyLFU milta hai.\n";
    cout << "\n  ⭐ SEEKH: LRU aur LFU me se koi 'behtar' nahi hai. Dono ek ANDAAZA\n";
    cout << "     lagate hain, aur jeet wahi jaata hai jiska andaaza aapke traffic\n";
    cout << "     se milta ho. Isi liye agla step: ARC (file 04) — jo khud dekh kar\n";
    cout << "     faisla karta hai.\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
