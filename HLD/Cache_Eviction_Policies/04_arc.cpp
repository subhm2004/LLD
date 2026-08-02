// ============================================================================
//  #4 — 04_arc.cpp
//  ARC — jo workload dekh kar KHUD faisla karta hai
// ----------------------------------------------------------------------------
//  Ab tak ki poori kahani ek hi baat kehti hai:
//
//     LRU  -> recency pe daav lagata hai. Zipf pe theek, scan/loop pe barbaad.
//     LFU  -> frequency pe daav lagata hai. Stable pe shandaar, shifting pe barbaad.
//
//  Dono ek FIXED andaaza lagate hain aur usi pe ade rehte hain. Agar workload
//  badal jaaye to unhe pata hi nahi chalta.
//
//  ⭐ ARC (IBM, 2003) ka jawab: dono rakho, aur beech ka batwara WORKLOAD DEKH
//     KAR khud badalte raho. Koi config nahi, koi tuning nahi — cache khud
//     seekhta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ GHOST LISTS — poora jaadu inhi me hai                                 │
//  │                                                                          │
//  │  ARC nikaali gayi keys ke NAAM yaad rakhta hai (data nahi — sirf key,     │
//  │  isliye lagbhag muft):                                                   │
//  │                                                                          │
//  │    B1 = recency wali jo nikaali gayi   B2 = frequency wali jo nikaali gayi│
//  │                                                                          │
//  │  Ab agar B1 me koi key WAPAS maangi gaye, to ARC ko pata chal gaya ki     │
//  │  "maine recency wale hisse ko chhota rakh kar GALTI ki" — aur wo turant   │
//  │  us hisse ko bada kar deta hai. B2 pe hit ho to ulta.                     │
//  │                                                                          │
//  │  Yaani ARC apni PICHHLI GALTIYON se seekhta hai. Yahi uski poori khoobi.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Compile: g++ -std=c++17 -Wall -Wextra -O2 04_arc.cpp -o arc_demo
// ============================================================================
#include <bits/stdc++.h>

#include "cache_common.h"
#include "policies/arc_cache.h"
#include "policies/lfu_cache.h"
#include "policies/lru_cache.h"

using namespace std;
using namespace cache;

int main()
{
    cout << "=========================================================\n";
    cout << " #4 : ARC (Adaptive Replacement Cache)\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ⭐ ARC ka "mood" badalte hue dekho --------------------
    cout << "[A] ⭐ ARC ko live SEEKHTE hue dekho (`p` = T1 ka target size):\n\n";
    {
        const size_t CAPACITY = 100;

        cout << "    `p` bada  = ARC keh raha hai 'abhi RECENCY important hai'\n";
        cout << "    `p` chhota= ARC keh raha hai 'abhi FREQUENCY important hai'\n\n";

        cout << "      workload                    | final p | T1  | T2  | ARC ne kya chuna\n";
        cout << "      ----------------------------+---------+-----+-----+------------------\n";

        struct Case
        {
            string name;
            vector<string> trace;
        };
        vector<Case> cases = {
            {"Working set 100 (fits)", makeUniformWorkload(110, 60000)},
            {"Zipf 5000 (popular few)", makeZipfWorkload(5000, 100000)},
            {"Working set + scan", makeWorkingSetScanWorkload(100, 200, 20000, 2000)},
            {"Uniform 400 (no locality)", makeUniformWorkload(400, 60000)},
            {"Uniform 1000 (bahut bada)", makeUniformWorkload(1000, 60000)}};

        for (Case &c : cases)
        {
            ArcCache arc(CAPACITY);
            runWorkload(arc, c.trace);
            string verdict = (arc.targetT1() > CAPACITY / 4) ? "RECENCY (T1 bada)"
                                                             : "FREQUENCY (T2 bada)";
            cout << "      " << left << setw(27) << c.name << right << " | " << setw(7) << fixed
                 << setprecision(1) << arc.targetT1() << " | " << setw(3) << arc.t1Size() << " | "
                 << setw(3) << arc.t2Size() << " | " << verdict << "\n";
        }
        cout << left;
        cout << right;

        cout << "\n    ⭐⭐ Upar wali teen rows me ARC ne T2 (frequency) chuna, aur\n";
        cout << "       neeche wali do me T1 (recency) ki taraf shift kar gaya.\n";
        cout << "       Humne kuch configure nahi kiya — sirf traffic badla.\n";
        cout << "\n       WAJAH samajhna zaroori hai:\n";
        cout << "         * Upar wale cases me kuch keys BAAR-BAAR aati hain. Unhe T2 me\n";
        cout << "           rakhna hi sahi hai — to ARC ne p ko 0 ke paas rakha.\n";
        cout << "         * Neeche wale cases me key-space cache se 4-10x bada hai.\n";
        cout << "           Koi key dobara hit hone se pehle hi nikal jaati hai, yaani\n";
        cout << "           'frequency' ka koi matlab hi nahi bachta. ARC ko ye ghost\n";
        cout << "           list B1 ke hits se PATA CHAL JAATA hai, aur wo T1 ko bada\n";
        cout << "           kar deta hai.\n";
        cout << "\n       Yahi 'adaptive' ka matlab hai — koi knob nahi, cache khud seekhta hai.\n\n";
    }

    // ---- Part B: ⭐⭐ SCAN RESISTANCE — ARC ka sabse bada karnama -------
    cout << "[B] ⭐⭐ SCAN RESISTANCE — file 01 wala test, ab ARC ke saath:\n\n";
    {
        const size_t CAPACITY = 100;
        vector<string> noScan = makeWorkingSetScanWorkload(100, 0, 20000, 1 << 30);
        vector<string> withScan = makeWorkingSetScanWorkload(100, 200, 20000, 2000);

        LruCache lruClean(CAPACITY), lruDirty(CAPACITY);
        ArcCache arcClean(CAPACITY), arcDirty(CAPACITY);

        double lruBefore = hotKeyHitRate(lruClean, noScan);
        double lruAfter = hotKeyHitRate(lruDirty, withScan);
        double arcBefore = hotKeyHitRate(arcClean, noScan);
        double arcAfter = hotKeyHitRate(arcDirty, withScan);

        cout << "    (Sirf ASLI KAAM KI keys pe hit rate — scan ki apni misses nahi gini)\n\n";
        cout << "      policy | bina scan | scan ke saath | miss rate ka farak\n";
        cout << "      -------+-----------+---------------+--------------------\n";
        cout << "      LRU    |  " << fixed << setprecision(2) << setw(7) << lruBefore << "% |  "
             << setw(11) << lruAfter << "% | " << setprecision(2) << (100.0 - lruBefore)
             << "% -> " << (100.0 - lruAfter) << "%  (" << setprecision(1)
             << ((100.0 - lruAfter) / (100.0 - lruBefore)) << "x)\n";
        cout << "      ARC    |  " << setprecision(2) << setw(7) << arcBefore << "% |  "
             << setw(11) << arcAfter << "% | " << (100.0 - arcBefore) << "% -> "
             << (100.0 - arcAfter) << "%  (" << setprecision(1)
             << ((100.0 - arcAfter) / (100.0 - arcBefore)) << "x)\n";

        cout << "\n    ⭐⭐ LRU ka miss rate 10 GUNA badh gaya, ARC ka lagbhag utna hi raha.\n";
        cout << "\n       Wajah bahut khoobsurat hai: scan ki har key PEHLI baar aati hai,\n";
        cout << "       to wo sirf T1 me jaati hai. Aur asli kaam ki keys — jo baar-baar\n";
        cout << "       maangi jaati hain — T2 me baithi hain. Scan T2 ko CHHOO hi nahi\n";
        cout << "       pata!\n";
        cout << "\n       LRU ke paas ye do-hisse wala dhaancha hai hi nahi — uske liye\n";
        cout << "       'ek baar dekhi' aur 'hazaar baar dekhi' key barabar hai.\n\n";
    }

    // ---- Part C: har workload pe ARC ka haal ----------------------------
    cout << "[C] Ab poora imtihaan — har workload pe teeno:\n\n";
    {
        const size_t CAPACITY = 100;
        LruCache lru(CAPACITY);
        LfuCache lfu(CAPACITY);
        ArcCache arc(CAPACITY);

        vector<pair<string, vector<string>>> workloads = {
            {"Zipf (normal)", makeZipfWorkload(5000, 100000)},
            {"Shifting popularity", makeShiftingWorkload(300, 10, 10000)},
            {"Working set + scan", makeWorkingSetScanWorkload(100, 200, 20000, 2000)},
            {"Loop (120 keys)", makeLoopWorkload(120, 200)}};

        cout << "      workload              |   LRU  |   LFU  |   ARC  | OPTIMAL\n";
        cout << "      ----------------------+--------+--------+--------+--------\n";
        for (auto &w : workloads)
        {
            cout << "      " << left << setw(21) << w.first << right << " | " << setw(5) << fixed
                 << setprecision(1) << runWorkload(lru, w.second) << "% | " << setw(5)
                 << runWorkload(lfu, w.second) << "% | " << setw(5) << runWorkload(arc, w.second)
                 << "% | " << setw(5) << beladyOptimalHitRate(w.second, CAPACITY) << "%\n";
        }
        cout << left;
        cout << right;

        cout << "\n    ⭐ Pehli TEEN rows dekho: LFU shifting pe 13% pe dhah gaya,\n";
        cout << "       LRU scan pe pichhda — par ARC teeno jagah top pe ya top ke\n";
        cout << "       bilkul paas raha. Yahi uski asli keemat hai:\n";
        cout << "       wo har jagah #1 nahi hota, par kahin DHAH-TA bhi nahi.\n";
        cout << "\n       Production me aksar yahi chahiye hota hai — kyunki aapko nahi\n";
        cout << "       pata kal traffic kaisa aayega. (Yahi 'p99 latency' wali soch:\n";
        cout << "       average se zyada zaroori hai ki WORST case kitna bura hai.)\n";

        cout << "\n    ⚠⚠ PAR AAKHRI ROW DEKHO — loop pe ARC bhi 0.0% hai!\n";
        cout << "\n       Aur ye ARC ki koi kami nahi — wahan LRU, LFU, FIFO, CLOCK,\n";
        cout << "       ARC, SAB zero dete hain (file 03 me dekha tha ki sirf Random\n";
        cout << "       bachta hai, 68% ke saath).\n";
        cout << "\n       Wajah gehri hai: ARC 'adaptive' zaroor hai, par wo phir bhi\n";
        cout << "       DETERMINISTIC hai — ek niyam se chalta hai. Aur cyclic loop us\n";
        cout << "       niyam ko theek ulta pakad leta hai, to wo har baar bilkul wahi\n";
        cout << "       key nikaalta hai jo agli maangi jaane wali thi.\n";
        cout << "\n       ⭐ Yaani: koi bhi deterministic policy is jaal me phans sakti\n";
        cout << "          hai. Bachne ka ek hi raasta hai — RANDOMNESS.\n";
        cout << "          Isi liye Redis ka default 'approximated LRU' hai (5 random\n";
        cout << "          keys uthao, unme se purani nikalo) — thoda kam accurate,\n";
        cout << "          par aise pathological patterns se mehfooz.\n\n";
    }

    // ---- Part D: keemat --------------------------------------------------
    cout << "[D] To ARC hamesha kyun nahi use karte?\n\n";
    cout << "    ❌ COMPLEXITY — 4 lists ka hisaab. LRU ~20 line ka hai, ARC ~150 ka.\n";
    cout << "       Zyada code = zyada bugs, aur debug karna mushkil.\n\n";
    cout << "    ❌ METADATA — ghost lists ki wajah se 2x keys track karni padti hain\n";
    cout << "       (data nahi, par phir bhi memory aur pointer chasing).\n\n";
    cout << "    ❌ PATENT — IBM ne ARC patent kiya tha. Isi wajah se bahut se\n";
    cout << "       open-source projects ne ise use nahi kiya aur apne alternatives\n";
    cout << "       banaye (jaise Linux ka 2Q-jaisa approach, ya Caffeine ka TinyLFU).\n";
    cout << "       (Patent ab expire ho chuka hai.)\n\n";
    cout << "    ❌ Hit path pe LRU se zyada kaam — high-throughput cache me wo\n";
    cout << "       CLOCK jaisi sasti policy se haar sakta hai (file 03 wali baat).\n";
    cout << "\n    📌 Jahan ARC/2Q milta hai: ZFS ka ARC (naam wahi se aaya),\n";
    cout << "       PostgreSQL ka buffer manager (2Q-jaisa), aur bahut se DB engines.\n";
    cout << "       Yaani jahan SCAN aam baat hai, wahan ye zaroori ho jaata hai.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Recency aur frequency, dono ka hisaab — aur balance KHUD chunta hai.\n";
    cout << "  ✅ SCAN-RESISTANT: scan T2 ko chhoo hi nahi pata (upar naapa —\n";
    cout << "     LRU ka miss rate 10x hua, ARC ka lagbhag wahi raha).\n";
    cout << "  ✅ Kisi bhi workload pe buri tarah nahi girta (ye sabse badi baat hai).\n";
    cout << "  ✅ Zero tuning — koi config knob nahi.\n";
    cout << "  ❌ Code lamba, metadata zyada, hit path mehnga.\n";
    cout << "\n  ⭐ POORE FOLDER KI SEEKH: policy chunna 'best' dhoondhna nahi hai —\n";
    cout << "     apne workload ko pehchan-na hai. Aur agar workload badalta rehta\n";
    cout << "     hai, to adaptive policy (ARC) hi sabse safe hai.\n";
    cout << "\n  Sab kuch ek saath -> 05_compare_all.cpp\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
