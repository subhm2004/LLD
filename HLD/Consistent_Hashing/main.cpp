// ============================================================================
//  main.cpp  —  CAPSTONE DEMO: ek nakli distributed cache
// ----------------------------------------------------------------------------
//  `ConsistentHashRing` ko ek asli scenario me chala ke dekhte hain:
//  ek distributed cache jisme servers add/remove hote rehte hain, aur hum
//  har mod pe NAAPTE hain ki kitna nuksan hua.
//
//  Kahani kuch aisi chalegi:
//     1. 4 cache servers, 50,000 keys cache me bhari
//     2. Traffic spike -> 2 naye servers add (scale up) -> kitne cache miss?
//     3. Ek server crash -> replicas ne bachaya?
//     4. Ek server chhota hai -> weight kam karke usko kam load do
//
//  ⭐ Poore demo me dhyan ISPE rakhna: har badlav ke baad "kitna % data hila".
//     Wahi ek number decide karta hai ki system production me tikega ya nahi.
// ============================================================================
#include <bits/stdc++.h>

#include "ConsistentHashRing.h"

using namespace std;
using namespace consistent_hashing;

// ============================================================================
//  CacheCluster — ring ke upar ek patli si layer (jo asli kaam karti)
// ----------------------------------------------------------------------------
//  ⭐ Dhyan do ki ring aur cache ALAG-ALAG hain:
//     * Ring bas ROUTING batata hai — "ye key kis server pe honi chahiye".
//     * Cache asal data rakhta hai.
//  Ye alagav (separation of concerns) hi wajah hai ki ring itna reusable hai —
//  wahi ring kal ko sharded DB ya load balancer me bhi laga sakte ho.
// ============================================================================
class CacheCluster
{
public:
    CacheCluster(int virtualNodes, int replicationFactor)
        : ring_(virtualNodes, replicationFactor) {}

    void addServer(const string &serverId, int weight = 1)
    {
        ring_.addNode(serverId, weight);
        storage_[serverId]; // khaali cache bana do
    }

    void removeServer(const string &serverId)
    {
        ring_.removeNode(serverId);
        storage_.erase(serverId); // server gaya = uski RAM gayi (data gaya)
    }

    // ---- Write: saari replicas pe likho ---------------------------------
    void put(const string &key, const string &value)
    {
        for (const string &server : ring_.getNodes(key))
        {
            storage_[server][key] = value;
        }
    }

    // ---- Read: pehle primary, wo na mile to replicas ---------------------
    // Return: {mila?, kis server se mila}
    pair<bool, string> get(const string &key) const
    {
        for (const string &server : ring_.getNodes(key))
        {
            auto serverIt = storage_.find(server);
            if (serverIt == storage_.end())
            {
                continue; // ye server ab zinda nahi
            }
            auto keyIt = serverIt->second.find(key);
            if (keyIt != serverIt->second.end())
            {
                return {true, server}; // HIT
            }
        }
        return {false, ""}; // MISS — kisi bhi replica pe nahi mila
    }

    const ConsistentHashRing &ring() const { return ring_; }

    size_t entriesOn(const string &server) const
    {
        auto it = storage_.find(server);
        return it == storage_.end() ? 0 : it->second.size();
    }

private:
    ConsistentHashRing ring_;
    map<string, map<string, string>> storage_; // serverId -> (key -> value)
};

// ---- Cluster ki abhi ki haalat print karo --------------------------------
void printClusterState(const CacheCluster &cluster)
{
    map<string, double> share = cluster.ring().ownershipPercent();
    cout << "      server      | ring share | cached entries\n";
    cout << "      ------------+------------+---------------\n";
    for (const auto &entry : share)
    {
        cout << "      " << left << setw(11) << entry.first << " | " << right << setw(9)
             << fixed << setprecision(2) << entry.second << "% | " << setw(14)
             << cluster.entriesOn(entry.first) << "\n";
    }
    cout << left;
}

// ---- Poore keyset pe cache hit-rate naapo -------------------------------
double measureHitRate(const CacheCluster &cluster, const vector<string> &keys)
{
    int hits = 0;
    for (const string &key : keys)
    {
        if (cluster.get(key).first)
        {
            ++hits;
        }
    }
    return (100.0 * hits) / keys.size();
}

int main()
{
    const int TOTAL_KEYS = 50000;
    const int VNODES = 150;
    const int RF = 3;

    vector<string> keys;
    keys.reserve(TOTAL_KEYS);
    for (int i = 0; i < TOTAL_KEYS; ++i)
    {
        keys.push_back("session:" + to_string(i));
    }

    cout << "=========================================================\n";
    cout << " CAPSTONE : DISTRIBUTED CACHE (ConsistentHashRing ka use)\n";
    cout << " " << TOTAL_KEYS << " keys | vnodes=" << VNODES << " | RF=" << RF << "\n";
    cout << "=========================================================\n\n";

    // ---- Act 1: cluster khada karo aur cache bharo ------------------------
    cout << "[1] 4 cache servers, aur saari keys cache me bhar di:\n\n";
    CacheCluster cluster(VNODES, RF);
    for (const string &server : {"cache-1", "cache-2", "cache-3", "cache-4"})
    {
        cluster.addServer(server);
    }
    for (const string &key : keys)
    {
        cluster.put(key, "value-of-" + key);
    }
    printClusterState(cluster);
    cout << "\n      Hit rate: " << fixed << setprecision(2) << measureHitRate(cluster, keys)
         << "%  (sab kuch cache me hai, hona hi chahiye)\n";
    cout << "      Note: har server pe ~" << (100 * RF / 4) << "% entries hain kyunki RF=" << RF
         << " (har key ki " << RF << " copies).\n\n";

    // ---- Act 2: SCALE UP — traffic badha, 2 server aur ------------------
    cout << "[2] Traffic spike! 2 naye servers add kar rahe hain (4 -> 6):\n\n";

    // Add karne se PEHLE har key ka primary yaad kar lo
    map<string, string> primaryBefore;
    for (const string &key : keys)
    {
        primaryBefore[key] = cluster.ring().getNode(key);
    }

    cluster.addServer("cache-5");
    cluster.addServer("cache-6");

    int primaryChanged = 0;
    for (const string &key : keys)
    {
        if (cluster.ring().getNode(key) != primaryBefore[key])
        {
            ++primaryChanged;
        }
    }

    printClusterState(cluster);
    cout << "\n      Keys jinka primary badla : " << primaryChanged << " ("
         << fixed << setprecision(1) << (100.0 * primaryChanged / TOTAL_KEYS) << "%)\n";
    cout << "      Modulo hashing hota to ye ~83% hota (1 - 1/6). ⭐\n";

    // Naye servers khaali hain — un keys ka data unpe abhi gaya hi nahi.
    // Par RF ki wajah se purane replicas ke paas ab bhi copy hai, isliye hit rate bacha hua hai.
    cout << "\n      Hit rate ab: " << fixed << setprecision(2) << measureHitRate(cluster, keys)
         << "%\n";
    cout << "      ⭐ 100% kyun? Naye servers khaali hain, PAR purane replicas ke paas\n"
         << "         data ab bhi hai — get() replicas tak dhoondhta hai. RF ka bonus.\n\n";

    // ---- Act 3: CRASH — ek server mar gaya ------------------------------
    cout << "[3] cache-2 CRASH ho gaya (uski poori RAM gayi):\n\n";
    cluster.removeServer("cache-2");
    printClusterState(cluster);

    double hitAfterCrash = measureHitRate(cluster, keys);
    cout << "\n      Hit rate: " << fixed << setprecision(2) << hitAfterCrash << "%\n";
    cout << "      ⭐ Ek server poora ud gaya, phir bhi hit rate " << setprecision(0)
         << hitAfterCrash << "% — kyunki har key ki " << RF << " copies thi.\n";
    cout << "         RF=1 hota to seedha ~" << (100 / 5) << "% data gayab ho jaata.\n\n";

    // ---- Act 4: WEIGHTED node — chhota server, kam bojh ------------------
    cout << "[4] Naya server aaya par wo CHHOTA hai (aadhi RAM) — weight se handle karo:\n\n";
    cout << "      cache-7 ko weight=3 (bada server) aur cache-8 ko weight=1 (chhota):\n\n";
    cluster.addServer("cache-7", 3); // 3x vnodes -> 3x keys
    cluster.addServer("cache-8", 1);
    printClusterState(cluster);
    cout << "\n      ⭐ cache-7 ka ring share cache-8 se lagbhag 3 GUNA hai —\n"
         << "         bilkul waise hi jaise weight kaha tha. Lookup code me ek\n"
         << "         line bhi change nahi karni padi.\n\n";

    // ---- Act 5: ek key ka poora safar ------------------------------------
    cout << "[5] Ek key ka poora routing (jaise production me debug karte ho):\n\n";
    const string sampleKey = "session:12345";
    vector<string> preferenceList = cluster.ring().getNodes(sampleKey);
    cout << "      key            : " << sampleKey << "\n";
    cout << "      ring position  : " << fixed << setprecision(2)
         << ringPercent(hashKey(sampleKey)) << "% of ring\n";
    cout << "      preference list: ";
    for (size_t i = 0; i < preferenceList.size(); ++i)
    {
        cout << preferenceList[i] << (i == 0 ? " (primary)" : " (replica)");
        if (i + 1 < preferenceList.size())
        {
            cout << ", ";
        }
    }
    cout << "\n";
    pair<bool, string> lookup = cluster.get(sampleKey);
    cout << "      GET result     : " << (lookup.first ? "HIT from " + lookup.second : "MISS")
         << "\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " SUMMARY — ConsistentHashRing ne kya-kya sambhala:\n";
    cout << "  ✅ Scale up (4->6): sirf " << fixed << setprecision(1)
         << (100.0 * primaryChanged / TOTAL_KEYS) << "% keys hili (modulo me ~83% hoti)\n";
    cout << "  ✅ Server crash: hit rate " << setprecision(0) << hitAfterCrash
         << "% bacha raha (RF=" << RF << " ki wajah se)\n";
    cout << "  ✅ Weighted nodes: bade server ko zyada, chhote ko kam — apne aap\n";
    cout << "  ✅ Load har server pe lagbhag barabar (vnodes=" << VNODES << ")\n";
    cout << "  ✅ Thread-safe reads (shared_mutex) — kai threads ek saath padh sakte\n";
    cout << "\n  Yahi class Memcached client, Redis Cluster, Cassandra —\n";
    cout << "  sab ke andar kisi na kisi roop me baithi hai.\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: capstone ke teen vaade -----------------------------------
    demo::check(primaryChanged < TOTAL_KEYS / 2,
                "4->6 scale up pe aadhi se kam keys hilni chahiye (modulo me ~83% hoti)");
    demo::checkNear(hitAfterCrash, 100.0, 0.01,
                    "RF=3 ke saath ek server crash pe hit rate 100% rehna chahiye");
    demo::check(!preferenceList.empty(), "preference list khaali nahi honi chahiye");

    return demo::report();
}
