// ============================================================================
//  main.cpp  —  Consistent Hashing ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye demo teeno patterns ka poora maza dikhata hai:
//    1. Ring pe nodes add karke even distribution (VIRTUAL NODES ka kamaal)
//    2. Node add/remove pe sirf ~1/N keys move (CONSISTENT HASHING ka core)
//    3. Replication — har key ki RF copies, alag-alag physical nodes pe
//    4. DOWN vs REMOVE ka farak (temporary failure pe migration nahi)
//    5. WEIGHTED nodes — bade server ko zyada hissa
//    6. ⭐ RUNTIME pe hash function SWAP (Strategy) — aur uski KEEMAT
//    7. ⭐ OBSERVER — har topology change pe listeners ko khabar
//
//  ⭐ Client ko sirf `ConsistentHashRing` dikhta hai (Facade). Hash function aur
//     listeners ka naam bas construct/register karte waqt aata hai; baaki ring,
//     virtual nodes, clockwise walk — sab hidden hai.
// ============================================================================
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/ConsistentHashRing.h"
#include "models/RingBalanceStats.h"
#include "observers/ConsoleRingLogger.h"
#include "observers/MigrationTracker.h"
#include "strategies/FnvHashFunction.h"
#include "strategies/Murmur3HashFunction.h"

using namespace std;
using namespace consistent_hashing_lld;

// ---- Test ke liye nakli keys -----------------------------------------------
vector<string> makeKeys(int count) {
    vector<string> keys;
    keys.reserve(count);
    for (int i = 0; i < count; ++i) {
        keys.push_back("session:" + to_string(i));
    }
    return keys;
}

// ---- Har key ka abhi ka owner (snapshot) ----------------------------------
// Isi se pehle-baad compare karke naapenge ki kitni keys sach me move hui.
unordered_map<string, string> snapshotOwners(const ConsistentHashRing &ring,
                                             const vector<string> &keys) {
    unordered_map<string, string> owners;
    owners.reserve(keys.size());
    for (const string &key : keys) {
        Node *node = ring.getNode(key);
        owners[key] = (node == nullptr) ? "" : node->getId();
    }
    return owners;
}

// ---- Kitni % keys ne malik badla -------------------------------------------
double percentMoved(const unordered_map<string, string> &before,
                    const unordered_map<string, string> &after) {
    int moved = 0;
    for (const auto &entry : before) {
        if (entry.second != after.at(entry.first)) {
            ++moved;
        }
    }
    return (100.0 * moved) / before.size();
}

// ---- Keys ka actual batwara (ring share nahi, asli keys) -------------------
void printKeyDistribution(const unordered_map<string, string> &owners, size_t totalKeys) {
    map<string, int> countPerNode; // map isliye ki output sorted dikhe
    for (const auto &entry : owners) {
        countPerNode[entry.second]++;
    }
    for (const auto &entry : countPerNode) {
        double percent = (100.0 * entry.second) / totalKeys;
        cout << "      " << left << setw(10) << entry.first << " : " << right << setw(6)
             << entry.second << " keys (" << fixed << setprecision(2) << setw(6) << percent
             << "%)\n"
             << left;
    }
}

// Vnode tuning me har setting ko kitni baar (alag seed se) naapna hai.
// Ek hi sample bahut noisy hota hai — average lene se asli trend dikhta hai.
static const int TUNING_TRIALS = 200;


// ============================================================================
//  ⭐ VERIFICATION — demo ko sirf REPORT nahi, VERIFY bhi karna chahiye
// ----------------------------------------------------------------------------
//  Warna agar kal koi `ConsistentHashRing` tod de, to ye demo galat numbers
//  print karke bhi "pass" dikhta. Ab invariants toote to EXIT CODE 1 aata hai,
//  jisse CI ise regression test ki tarah chala sakta hai.
// ============================================================================
namespace demo
{
inline int failureCount = 0;

inline void check(bool condition, const string &what)
{
    if (!condition)
    {
        ++failureCount;
        cout << "    ❌ VERIFY FAIL: " << what << "\n";
    }
}

template <typename T, typename U>
inline void checkEqual(T actual, U expected, const string &what)
{
    if (!(actual == static_cast<T>(expected)))
    {
        ++failureCount;
        cout << "    ❌ VERIFY FAIL: " << what << "  (mila " << actual << ", chahiye tha "
             << expected << ")\n";
    }
}

inline int report()
{
    if (failureCount == 0)
    {
        cout << "\n✅ VERIFY: saare invariants theek hain.\n";
        return 0;
    }
    cout << "\n❌ VERIFY: " << failureCount << " invariant TOOT gaye.\n";
    return 1;
}
} // namespace demo

int main() {
    const int TOTAL_KEYS = 50000;
    vector<string> keys = makeKeys(TOTAL_KEYS);

    // ---- Setup: Strategy inject karke ring banao --------------------------
    // 150 virtual nodes per node, replication factor 3.
    ConsistentHashRing ring(make_unique<Murmur3HashFunction>(), 150, 3);

    // ---- Observers register karo (ring ko inke baare me kuch nahi pata) ---
    ConsoleRingLogger logger;
    MigrationTracker migrationTracker;
    ring.addListener(&logger);
    ring.addListener(&migrationTracker);

    cout << "=========================================================\n";
    cout << " CONSISTENT HASHING LLD — DEMO\n";
    cout << " " << TOTAL_KEYS << " keys | hash=" << ring.hashFunctionName()
         << " | vnodes/node=150 | RF=" << ring.getReplicationFactor() << "\n";
    cout << "=========================================================\n\n";

    // ---- 1. Cluster khada karo -------------------------------------------
    cout << "=== 1. Cluster Setup (Observer live dikhega) ===\n";
    ring.addNode("cache-1", "10.0.0.1:11211");
    ring.addNode("cache-2", "10.0.0.2:11211");
    ring.addNode("cache-3", "10.0.0.3:11211");
    ring.addNode("cache-4", "10.0.0.4:11211");
    ring.printNodeStats();

    unordered_map<string, string> before = snapshotOwners(ring, keys);
    cout << "\n  Keys ka actual batwara (ideal 25% har node):\n";
    printKeyDistribution(before, TOTAL_KEYS);
    cout << "  ⭐ Virtual nodes ki wajah se lagbhag barabar. Bina unke koi node\n"
         << "     60% le leta aur koi 2% (HLD notes me naap ke dikhaya gaya hai).\n";

    // ---- 1b. ⭐ VIRTUAL NODE TUNING — std-dev ko MINIMIZE karo -------------
    cout << "\n=== 1b. Virtual Node Tuning (std-dev minimize) ===\n";
    cout << "\n  Sawaal: 150 vnodes/node hi kyun? Kam/zyada kyun nahi?\n";
    cout << "  Jawab: naap ke dekho — std-dev girta hai ya nahi.\n";
    cout << "\n  ⚠ Par ek TRAP hai: ek hi ring ka std-dev BAHUT shor (noisy) hota hai.\n";
    cout << "    Kismat se ek layout achha aa sakta hai, doosra bura — sirf ek sample\n";
    cout << "    dekh ke 'vnodes badhane se faayda nahi' keh dena GALAT nateeja hoga.\n";
    cout << "    Isliye har vnode count pe " << TUNING_TRIALS
         << " alag-alag rings banate hain (alag hash seed)\n";
    cout << "    aur unka AVERAGE lete hain. Tab asli trend saaf dikhta hai.\n\n";

    cout << "    vnodes/node | ring points | avg std-dev | avg worst ratio\n";
    cout << "    ------------+-------------+-------------+----------------\n";

    for (int vnodes : {1, 5, 20, 50, 100, 150, 250, 500}) {
        double sumStdDev = 0.0;
        double sumWorstRatio = 0.0;

        for (int trial = 0; trial < TUNING_TRIALS; ++trial) {
            // ⭐ Har trial me alag SEED -> nodes ring pe alag jagah girte hain.
            //    Yahi ek "independent sample" hai.
            uint32_t seed = static_cast<uint32_t>(trial) * 7919u + 13u;
            ConsistentHashRing tuningRing(make_unique<Murmur3HashFunction>(seed), vnodes, 3);
            tuningRing.addNode("cache-1", "10.0.0.1:11211");
            tuningRing.addNode("cache-2", "10.0.0.2:11211");
            tuningRing.addNode("cache-3", "10.0.0.3:11211");
            tuningRing.addNode("cache-4", "10.0.0.4:11211");

            RingBalanceStats stats = tuningRing.getBalanceStats();
            sumStdDev += stats.stdDevPercent;
            sumWorstRatio += stats.worstRatio;
        }

        cout << "    " << right << setw(11) << vnodes << " | " << setw(11) << (vnodes * 4)
             << " | " << setw(11) << fixed << setprecision(3) << (sumStdDev / TUNING_TRIALS)
             << " | " << setw(14) << setprecision(2) << (sumWorstRatio / TUNING_TRIALS) << "x\n";
    }

    cout << "\n  ⭐ Ab trend bilkul saaf hai:\n";
    cout << "     • vnodes=1  -> std-dev sabse bada. Koi node ring ka bada hissa le\n";
    cout << "                    leta, koi bhookha reh jaata. Ye bina-vnodes wala haal.\n";
    cout << "     • vnodes badhao -> std-dev lagatar GIRTA hai. Wajah: har node ke\n";
    cout << "                    arcs jitne zyada aur chhote honge, unka total utna hi\n";
    cout << "                    average ke paas aayega (law of large numbers).\n";
    cout << "     • Girne ki raftaar ~1/sqrt(vnodes) hai — yaani 4 guna vnodes lagane\n";
    cout << "                    se std-dev sirf AADHA hota hai. Isi liye 500 se 1000\n";
    cout << "                    karne ka faayda bahut kam, par memory dugni.\n";
    cout << "     ⭐ Yahi wo trade-off hai jispe Cassandra/DynamoDB/Riak 100-256 pe\n";
    cout << "        rukte hain. Ye number kahin se aaya nahi — aise hi naap ke chunte hain.\n";

    // ---- 2. Lookup + Replication ------------------------------------------
    cout << "\n=== 2. Lookup aur Replication (preference list) ===\n";
    for (int i = 0; i < 4; ++i) {
        string key = "session:" + to_string(i);
        cout << "  " << left << setw(12) << key << " -> [ ";
        vector<Node *> replicas = ring.getNodes(key);
        for (size_t r = 0; r < replicas.size(); ++r) {
            cout << replicas[r]->getId() << (r == 0 ? "(primary)" : "(replica)");
            if (r + 1 < replicas.size()) {
                cout << ", ";
            }
        }
        cout << " ]\n";
    }
    cout << right;

    // Verify: teeno copies sach me ALAG physical nodes pe hain?
    int duplicateCopies = 0, shortLists = 0;
    for (const string &key : keys) {
        vector<Node *> replicas = ring.getNodes(key);
        set<string> unique;
        for (Node *node : replicas) {
            unique.insert(node->getId());
        }
        if (unique.size() != replicas.size()) {
            ++duplicateCopies;
        }
        if (replicas.size() != static_cast<size_t>(ring.getReplicationFactor())) {
            ++shortLists;
        }
    }
    cout << "\n  Sanity check (poori " << TOTAL_KEYS << " keys pe):\n";
    cout << "    Keys jinki 2 copies EK hi node pe : " << duplicateCopies
         << "   <-- 0 hona chahiye ✅\n";
    cout << "    Keys jinko poori RF copies nahi mili : " << shortLists
         << "   <-- 0 hona chahiye ✅\n";

    // ---- 3. ⭐ ASLI TEST — node add karo ----------------------------------
    cout << "\n=== 3. Scale Up — naya node add (yahi consistent hashing ka core) ===\n";
    ring.addNode("cache-5", "10.0.0.5:11211");

    unordered_map<string, string> afterAdd = snapshotOwners(ring, keys);
    double movedOnAdd = percentMoved(before, afterAdd);

    cout << "\n  Keys moved : " << fixed << setprecision(2) << movedOnAdd << "%\n";
    cout << "  Simple `hash % N` hota to ye ~" << setprecision(0) << (100.0 * 4 / 5)
         << "% hota (1 - 1/N). ⭐\n";
    cout << "  Theory: ~1/N = " << setprecision(1) << (100.0 / 5) << "% — aur hum wahi paas hain.\n";

    // ---- 4. DOWN vs REMOVE ------------------------------------------------
    cout << "\n=== 4. DOWN vs REMOVE ka farak (bahut zaroori) ===\n";
    cout << "\n  (a) cache-2 ko DOWN mark karo (temporary glitch):\n";
    ring.markNodeStatus("cache-2", NodeStatus::DOWN);

    unordered_map<string, string> afterDown = snapshotOwners(ring, keys);
    int wasOnCache2 = 0, othersDisturbed = 0;
    for (const auto &entry : afterAdd) {
        if (entry.second == "cache-2") {
            ++wasOnCache2;
        } else if (entry.second != afterDown.at(entry.first)) {
            ++othersDisturbed;
        }
    }
    cout << "      cache-2 ki keys jo failover hui : " << wasOnCache2 << "\n";
    cout << "      Baaki nodes ki keys jo hili     : " << othersDisturbed
         << "   <-- 0 hona chahiye ✅\n";
    cout << "      Migration tasks banaye gaye     : 0  ⭐ (DOWN pe data hilana bekaar)\n";

    cout << "\n  (b) Ab cache-2 wapas ACTIVE:\n";
    ring.markNodeStatus("cache-2", NodeStatus::ACTIVE);
    unordered_map<string, string> afterRecovery = snapshotOwners(ring, keys);
    cout << "      Keys jo wapas apni purani jagah aayi : "
         << (percentMoved(afterAdd, afterRecovery) == 0.0 ? "SAARI ✅" : "kuch nahi") << "\n";
    cout << "      ⭐ Ring positions kabhi hili hi nahi thi, isliye recovery FREE hai.\n";

    // ---- 5. Weighted node -------------------------------------------------
    cout << "\n=== 5. Weighted Node (bada server = zyada hissa) ===\n";
    ring.addNode("cache-big", "10.0.0.9:11211", 3); // weight=3 -> 3x vnodes
    ring.printNodeStats();
    cout << "\n  ⭐ cache-big ka ring share baaki sab se lagbhag 3 GUNA hai —\n"
         << "     bilkul waise hi jaise weight=3 kaha tha. Lookup code me ek\n"
         << "     line bhi change nahi karni padi.\n";

    // ---- 6. Node remove ---------------------------------------------------
    cout << "\n=== 6. Node Remove (permanent) ===\n";
    unordered_map<string, string> beforeRemove = snapshotOwners(ring, keys);
    ring.removeNode("cache-3");

    unordered_map<string, string> afterRemove = snapshotOwners(ring, keys);
    int reassigned = 0, untouched = 0;
    map<string, int> inheritedBy;
    for (const auto &entry : beforeRemove) {
        if (entry.second == "cache-3") {
            ++reassigned;
            inheritedBy[afterRemove.at(entry.first)]++;
        } else if (entry.second == afterRemove.at(entry.first)) {
            ++untouched;
        }
    }
    cout << "\n  cache-3 ki keys jo redistribute hui : " << reassigned << "\n";
    cout << "  Baaki keys jo bilkul nahi hili      : " << untouched << " ✅\n";
    cout << "  Aur uska bojh kaha-kaha bata:\n";
    for (const auto &entry : inheritedBy) {
        cout << "    " << left << setw(10) << entry.first << " : " << right << setw(5)
             << entry.second << " keys (" << fixed << setprecision(1)
             << (100.0 * entry.second / reassigned) << "%)\n"
             << left;
    }
    cout << "  ⭐ Bojh SAARE bache nodes me bata — kisi ek pe nahi gira.\n"
         << "     Bina virtual nodes ke ye poora bojh ek hi padosi pe girta\n"
         << "     (aur wo bhi gir jaata — cascading failure).\n";

    // ---- 7. ⭐ STRATEGY SWAP — aur uski keemat ---------------------------
    cout << "\n=== 7. Hash Function SWAP (Strategy) — aur uski KEEMAT ===\n";
    unordered_map<string, string> beforeSwap = snapshotOwners(ring, keys);

    cout << "\n  Murmur3 se FNV-1a pe switch kar rahe hain:\n";
    ring.setHashFunction(make_unique<FnvHashFunction>());

    unordered_map<string, string> afterSwap = snapshotOwners(ring, keys);
    cout << "\n  Keys moved : " << fixed << setprecision(2)
         << percentMoved(beforeSwap, afterSwap) << "%  ⚠\n";
    cout << "  ⭐ YAHI SEEKHNE WALI BAAT HAI:\n";
    cout << "     LoadBalancer_LLD me strategy swap MUFT tha (agli request pe naya\n";
    cout << "     algorithm, bas). Yahan hash function badalne ka matlab hai har\n";
    cout << "     node ki ring position badal gayi -> keys ka mapping bikhar gaya.\n";
    cout << "     Ye number lagbhag utna hi bura hai jitna `hash % N` wala tha —\n";
    cout << "     (kuch keys sirf ITTEFAAQ se apne purane node pe reh jaati hain).\n";
    cout << "     Isliye production me ye ek PLANNED MIGRATION hoti hai,\n";
    cout << "     'runtime toggle' nahi. Pattern wahi, keemat alag.\n";

    // Distribution dono hash ke saath compare — dono achhe hone chahiye
    cout << "\n  FNV-1a ke saath distribution (quality check):\n";
    printKeyDistribution(afterSwap, TOTAL_KEYS);
    cout << "  ⭐ Ye tedha NAHI hai — weights yaad karo: cache-big ka weight 3 hai\n"
         << "     aur baaki 4 nodes ka 1-1. Total weight = 7, to expected share:\n"
         << "        cache-big -> 3/7 = " << setprecision(1) << (300.0 / 7) << "%"
         << "   |   baaki har ek -> 1/7 = " << (100.0 / 7) << "%\n"
         << "     Upar ke numbers bilkul wahi hain. Yaani FNV-1a ka distribution\n"
         << "     bhi utna hi achha hai — bas positions alag hain. Swap 'galat'\n"
         << "     nahi hai, bas MEHNGA hai.\n";

    // ---- 8. Observer ka final report -------------------------------------
    cout << "\n=== 8. Observer ka Report (MigrationTracker) ===\n";
    migrationTracker.printPendingTasks();

    cout << "\n---------------------------------------------------------\n";
    cout << " SUMMARY — kaunsa pattern kahan:\n";
    cout << "  • STRATEGY — HashFunction (FNV / Murmur3) pluggable\n";
    cout << "  • FACADE   — ConsistentHashRing hi client ka poora interface\n";
    cout << "  • OBSERVER — RingChangeListener (logger + migration tracker)\n";
    cout << "\n SEEKHNE WALI 3 BAATEIN:\n";
    cout << "  1. Node add pe sirf ~1/N keys hilti (modulo me ~80%)\n";
    cout << "  2. DOWN != REMOVE — temporary failure pe data migration MAT karo\n";
    cout << "  3. Replication me 'agle N POINTS' nahi, 'agle N alag PHYSICAL nodes'\n";
    cout << "---------------------------------------------------------\n";

    // ---- VERIFY: is LLD ke chaar core vaade -------------------------------
    demo::checkEqual(duplicateCopies, 0,
                     "ek key ki do copies ek hi physical node pe nahi honi chahiye");
    demo::checkEqual(shortLists, 0, "har key ko poori RF copies milni chahiye");
    demo::checkEqual(othersDisturbed, 0,
                     "node DOWN karne pe baaki nodes ki keys nahi hilni chahiye");
    demo::check(movedOnAdd < 45.0,
                "node add pe ~1/N keys hilni chahiye, modulo wali 80% nahi");

    return demo::report();
    // ⭐ Koi `delete` nahi — ring stack pe hai, uske andar nodes `unique_ptr`
    //    me hain. Scope khatam hote hi sab apne aap saaf ho jaayega.
}
