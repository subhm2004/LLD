// ============================================================================
//  core/ConsistentHashRing.h  —  Poore system ka FACADE (+ Context + Subject)
// ----------------------------------------------------------------------------
//  Ye class hi client ke liye "consistent hashing" hai. Andar hash ring, virtual
//  nodes, physical nodes ki registry, replication logic, aur listeners — sab
//  manage karti hai. Bahar se client ko sirf saaf methods dikhte hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TEEN ROLE EK CLASS ME:                                                │
//  │                                                                          │
//  │  1. FACADE  — client ko ring/vnodes/hashing ki complexity se bachata hai  │
//  │  2. CONTEXT (Strategy) — ek `HashFunction` hold karta hai aur position    │
//  │     nikalne ka kaam usko DELEGATE karta hai                              │
//  │  3. SUBJECT (Observer) — topology badle to listeners ko khabar deta hai   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DATA STRUCTURE: `std::map<uint32_t, VirtualNode>` (Red-Black tree)    │
//  │                                                                          │
//  │  Ring = SORTED positions. "Clockwise agla node" = "is position se bada    │
//  │  ya barabar pehla element" = `lower_bound()` — O(log V).                  │
//  │                                                                          │
//  │  Aur agar `lower_bound` end pe pahunch gaya (ring ke aakhri point se bhi  │
//  │  aage), to circle ki tarah GHOOM ke `begin()` pe aao. Bas yahi ek line    │
//  │  ring ko "circle" banati hai — warna wo seedhi line hai.                 │
//  │                                                                          │
//  │  ⚠ `unordered_map` yahan CHAL HI NAHI SAKTA — usme order hota hi nahi,    │
//  │     aur yahan poora khel "agla bada element" pe hai. Ye interview me      │
//  │     bahut poocha jaata hai.                                              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MEMORY: nodes `unique_ptr` me hain -> koi manual delete nahi, koi     │
//  │     leak nahi, koi double-free nahi. `VirtualNode` andar sirf raw `Node*` │
//  │     rakhta hai jo NON-OWNING hai (bas point karta hai).                   │
//  │                                                                          │
//  │     Copy/move phir bhi EXPLICITLY delete kiye hain: agar ring copy ho     │
//  │     jaata to naye ring ke VirtualNode purane ring ke Node* pe point karte │
//  │     rehte — dangling pointer ka pakka bulawa. Compiler `unique_ptr` ki    │
//  │     wajah se waise bhi copy rok deta, par yahan NIYAT saaf likhi hai      │
//  │     (Rule of Three/Five).                                                │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_CORE_CONSISTENTHASHRING_H
#define CONSISTENT_HASHING_LLD_CORE_CONSISTENTHASHRING_H

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../enums/NodeStatus.h"
#include "../models/Node.h"
#include "../models/RingBalanceStats.h"
#include "../models/VirtualNode.h"
#include "../observers/RingChangeListener.h"
#include "../strategies/HashFunction.h"

using namespace std;

namespace consistent_hashing_lld {

class ConsistentHashRing {
public:
    // HashFunction inject hoti hai (Dependency Injection) — ring usse own karta hai.
    ConsistentHashRing(unique_ptr<HashFunction> hashFunction,
                       int baseVirtualNodes = 150,
                       int replicationFactor = 3)
        : hashFunction_(std::move(hashFunction)),
          baseVirtualNodes_(baseVirtualNodes),
          replicationFactor_(replicationFactor) {
        if (hashFunction_ == nullptr) {
            throw invalid_argument("HashFunction cannot be null");
        }
        if (baseVirtualNodes_ <= 0) {
            throw invalid_argument("baseVirtualNodes must be greater than 0");
        }
        if (replicationFactor_ <= 0) {
            throw invalid_argument("replicationFactor must be greater than 0");
        }
    }

    // ⭐ Copy/move BAND — VirtualNode raw Node* rakhta hai (upar wala note).
    ConsistentHashRing(const ConsistentHashRing &) = delete;
    ConsistentHashRing &operator=(const ConsistentHashRing &) = delete;
    ConsistentHashRing(ConsistentHashRing &&) = delete;
    ConsistentHashRing &operator=(ConsistentHashRing &&) = delete;

    // ========================================================================
    //  NODE MANAGEMENT
    // ========================================================================

    // ---- Node onboard karo (duplicate id reject) --------------------------
    void addNode(const string &nodeId, const string &host, int weight = 1) {
        if (nodesById_.count(nodeId) != 0) {
            throw runtime_error("Node already exists: " + nodeId);
        }

        auto node = make_unique<Node>(nodeId, host, weight);
        Node *nodePtr = node.get();      // raw ptr ring ke liye (non-owning)
        nodesById_[nodeId] = std::move(node); // ownership yahan aa gayi

        int pointsAdded = placeOnRing(nodePtr);
        notifyNodeAdded(*nodePtr, pointsAdded);
    }

    // ---- Node permanently hatao -------------------------------------------
    void removeNode(const string &nodeId) {
        Node *node = getNodeOrThrow(nodeId);

        // Pehle event bhejo (tab tak node zinda hai, listener uska data padh sake).
        int pointsRemoved = node->virtualNodeCount(baseVirtualNodes_);
        notifyNodeRemoved(*node, pointsRemoved);

        removeFromRing(node);
        nodesById_.erase(nodeId); // unique_ptr yahan Node ko delete kar dega
    }

    // ---- Health badlo (ACTIVE <-> DOWN) -----------------------------------
    // ⚠ Ring positions BILKUL nahi hilti — sirf routing skip karne lagti hai.
    //    Isi liye ye REMOVE se bahut sasta hai (koi migration nahi).
    void markNodeStatus(const string &nodeId, NodeStatus status) {
        Node *node = getNodeOrThrow(nodeId);
        NodeStatus oldStatus = node->getStatus();
        if (oldStatus == status) {
            return; // kuch badla hi nahi, event bhejne ka faayda nahi
        }
        node->setStatus(status);
        notifyNodeStatusChanged(*node, oldStatus);
    }

    // ========================================================================
    //  ⭐ LOOKUP — system ka dil
    // ========================================================================

    // ---- Key ka primary node (sirf ACTIVE) --------------------------------
    Node *getNode(const string &key) const {
        vector<Node *> nodes = collectClockwise(key, 1);
        return nodes.empty() ? nullptr : nodes.front();
    }

    // ---- Preference list: primary + replicas ------------------------------
    // Return: [primary, replica1, ...] — sab ALAG PHYSICAL nodes, sab ACTIVE.
    vector<Node *> getNodes(const string &key) const {
        return collectClockwise(key, static_cast<size_t>(replicationFactor_));
    }

    // ========================================================================
    //  STRATEGY SWAP (mehnga hai — poora ring dobara banta hai)
    // ========================================================================
    void setHashFunction(unique_ptr<HashFunction> hashFunction) {
        if (hashFunction == nullptr) {
            throw invalid_argument("HashFunction cannot be null");
        }
        string oldName = hashFunction_->name();
        hashFunction_ = std::move(hashFunction);

        // ⭐ Nayi hash function = sab positions badal gayi = ring dobara banao.
        ring_.clear();
        for (const auto &entry : nodesById_) {
            placeOnRing(entry.second.get());
        }
        notifyRingRebuilt("hash function " + oldName + " -> " + hashFunction_->name(), ring_.size());
    }

    // ========================================================================
    //  OBSERVER REGISTRATION
    // ========================================================================
    // ⚠ Ring listener ko OWN nahi karta (non-owning raw pointer) — listener ki
    //    life caller sambhalta hai. Isliye listener ring se ZYADA jeena chahiye.
    void addListener(RingChangeListener *listener) {
        if (listener != nullptr) {
            listeners_.push_back(listener);
        }
    }

    // ========================================================================
    //  INTROSPECTION / STATS
    // ========================================================================
    size_t nodeCount() const { return nodesById_.size(); }
    size_t virtualNodeCount() const { return ring_.size(); }
    int getReplicationFactor() const { return replicationFactor_; }
    string hashFunctionName() const { return hashFunction_->name(); }

    bool hasNode(const string &nodeId) const { return nodesById_.count(nodeId) != 0; }

    // ---- Har node ko ring ka kitna % hissa mila ---------------------------
    // ⭐ Ye ASLI arc measurement hai (keys sample kiye bina). Virtual node count
    //    tune karne ka sabse seedha tareeka — number 100/N ke jitna paas, utna achha.
    map<string, double> getLoadDistribution() const {
        map<string, double> share;
        for (const auto &entry : nodesById_) {
            share[entry.first] = 0.0;
        }
        if (ring_.empty()) {
            return share;
        }

        // Har ring point us arc ka maalik hai jo PICHHLE point ke baad shuru hota.
        uint32_t previous = ring_.rbegin()->first; // wrap: aakhri point se shuru
        for (const auto &entry : ring_) {
            // Ghatana modular hai (2^32 pe wrap) — unsigned arithmetic khud ye
            // sambhaal leti hai, isliye koi extra `if` nahi chahiye.
            uint32_t arc = entry.first - previous;
            share[entry.second.getPhysicalNode()->getId()] += (100.0 * arc) / RING_SIZE;
            previous = entry.first;
        }
        return share;
    }

    // ---- ⭐ BALANCE QUALITY — wahi number jise minimize karna hai ----------
    //  Har node ka EXPECTED share uske weight se aata hai:
    //        expected = 100 × (node ka weight) / (sab weights ka total)
    //  Deviation = actual - expected. Std-dev inhi deviations ka RMS hai.
    //
    //  ⭐ Weight ka dhyan isliye zaroori hai: weight=3 wale node ko 3 guna
    //     milna CHAHIYE — wo imbalance nahi hai. Agar hum sabka expected
    //     100/N maan lete to har weighted cluster "kharaab" dikhta (galat).
    RingBalanceStats getBalanceStats() const {
        RingBalanceStats stats;
        if (nodesById_.empty()) {
            return stats;
        }

        map<string, double> share = getLoadDistribution();
        stats.nodeCount = nodesById_.size();

        int totalWeight = 0;
        for (const auto &entry : nodesById_) {
            totalWeight += entry.second->getWeight();
        }

        double sumSquaredDeviation = 0.0;
        stats.minSharePercent = 100.0;
        stats.maxSharePercent = 0.0;

        for (const auto &entry : share) {
            double actual = entry.second;
            double expected = (100.0 * nodesById_.at(entry.first)->getWeight()) / totalWeight;

            double deviation = actual - expected;
            sumSquaredDeviation += deviation * deviation;

            stats.minSharePercent = min(stats.minSharePercent, actual);
            stats.maxSharePercent = max(stats.maxSharePercent, actual);

            // Sabse bura node apne expected se kitna guna hai (1.0 = perfect).
            if (expected > 0.0) {
                stats.worstRatio = max(stats.worstRatio, actual / expected);
            }
        }

        stats.stdDevPercent = sqrt(sumSquaredDeviation / share.size());
        return stats;
    }

    void printNodeStats() const {
        map<string, double> share = getLoadDistribution();
        cout << "\n=== Ring Stats (hash=" << hashFunction_->name()
             << ", vnodes/node=" << baseVirtualNodes_ << ", RF=" << replicationFactor_ << ") ===\n";
        cout << "  node       | host              | status | weight | ring share\n";
        cout << "  -----------+-------------------+--------+--------+-----------\n";
        // `share` ek sorted `map` hai — isliye output hamesha ek hi order me
        // aayega. (`nodesById_` unordered hai, uspe ghoomte to order har run me
        // badal sakta tha — demo output diff karna mushkil ho jaata.)
        for (const auto &entry : share) {
            const Node *node = nodesById_.at(entry.first).get();
            cout << "  " << left << setw(10) << node->getId() << " | " << setw(17) << node->getHost()
                 << " | " << setw(6) << (node->isAvailable() ? "ACTIVE" : "DOWN") << " | "
                 << setw(6) << node->getWeight() << " | " << right << setw(8) << fixed
                 << setprecision(2) << entry.second << "%\n"
                 << left;
        }
        RingBalanceStats stats = getBalanceStats();
        cout << "  Total virtual nodes on ring: " << ring_.size() << "\n";
        cout << "  ⭐ Balance: std-dev = " << fixed << setprecision(3) << stats.stdDevPercent
             << "%  |  worst node apne expected se " << setprecision(2) << stats.worstRatio
             << "x  (1.00 = perfect)\n";
    }

private:
    static constexpr double RING_SIZE = 4294967296.0; // 2^32

    // ---- Node ko ring pe uske saare virtual points ke saath rakho ---------
    // Return: kitne points add hue.
    int placeOnRing(Node *node) {
        int points = node->virtualNodeCount(baseVirtualNodes_);
        for (int i = 0; i < points; ++i) {
            // ⭐ Har vnode ka apna alag naam -> apna alag hash -> apni alag jagah.
            uint32_t position = hashFunction_->hash(VirtualNode::virtualId(node->getId(), i));

            // ⚠ COLLISION: do alag vnodes ka hash same aa sakta hai (birthday
            //    paradox — 2^32 space me hazaaron points pe possible hai).
            //    `map` me wo silently overwrite ho jaata. Yahan hum pehle wale
            //    ko rehne dete hain (skip) — warna doosre node ka point gayab
            //    ho jaata aur uska remove karna toot jaata.
            ring_.emplace(position, VirtualNode(position, i, node));
        }
        return points;
    }

    // ---- Node ke saare points ring se hatao ------------------------------
    void removeFromRing(Node *node) {
        int points = node->virtualNodeCount(baseVirtualNodes_);
        for (int i = 0; i < points; ++i) {
            uint32_t position = hashFunction_->hash(VirtualNode::virtualId(node->getId(), i));

            // ⚠ Sirf tab hatao jab wo point SACH ME isi node ka ho — collision
            //    ki wajah se us position pe kisi aur ka point ho sakta hai.
            auto it = ring_.find(position);
            if (it != ring_.end() && it->second.getPhysicalNode() == node) {
                ring_.erase(it);
            }
        }
    }

    // ---- ⭐ CLOCKWISE WALK — lookup aur replication dono ka engine ---------
    //  Ring pe key ki position se aage chalte raho, aur `wanted` alag PHYSICAL
    //  ACTIVE nodes jama karte jao.
    vector<Node *> collectClockwise(const string &key, size_t wanted) const {
        vector<Node *> result;
        if (ring_.empty()) {
            return result;
        }

        // Cluster me utne nodes hi na hon to jitne hain utne hi de do.
        wanted = min(wanted, nodesById_.size());

        uint32_t position = hashFunction_->hash(key);
        auto it = ring_.lower_bound(position); // clockwise agla point

        set<const Node *> seen; // ⭐ DUPLICATE PHYSICAL NODE ROKO (neeche note)

        // Poora ring ghoomne se zyada kabhi nahi — utne me saare nodes mil jaate.
        for (size_t steps = 0; steps < ring_.size() && result.size() < wanted; ++steps) {
            if (it == ring_.end()) {
                it = ring_.begin(); // ⭐ WRAP AROUND — yahi ring ko circle banata hai
            }

            Node *candidate = it->second.getPhysicalNode();

            // ⚠ SABSE BADI GALTI JO LOG KARTE HAIN:
            //    "clockwise agle N POINTS le lo" — ye GALAT hai! Vnodes ki wajah
            //    se agle 3 points aaram se EK HI physical node ke ho sakte hain
            //    (cache-1#7, cache-1#88, cache-1#3 — teeno cache-1!). Tab
            //    aapki "3 copies" asal me ek hi server pe hain — wo server gaya,
            //    teeno copies gayi, aur RF ka poora matlab khatam.
            //    ✅ Isi liye `seen` set: sirf NAYE physical node ko lo.
            //
            //    Aur DOWN node bhi skip — usko key dena bekaar hai.
            if (candidate->isAvailable() && seen.insert(candidate).second) {
                result.push_back(candidate);
            }
            ++it;
        }
        return result;
    }

    // Node dhoondho — na mile to saaf error (silent nullptr nahi = fail-fast).
    Node *getNodeOrThrow(const string &nodeId) const {
        auto it = nodesById_.find(nodeId);
        if (it == nodesById_.end()) {
            throw runtime_error("Node not found: " + nodeId);
        }
        return it->second.get();
    }

    // ---- Observer notifications -------------------------------------------
    void notifyNodeAdded(const Node &node, int points) {
        for (RingChangeListener *listener : listeners_) {
            listener->onNodeAdded(node, points);
        }
    }
    void notifyNodeRemoved(const Node &node, int points) {
        for (RingChangeListener *listener : listeners_) {
            listener->onNodeRemoved(node, points);
        }
    }
    void notifyNodeStatusChanged(const Node &node, NodeStatus oldStatus) {
        for (RingChangeListener *listener : listeners_) {
            listener->onNodeStatusChanged(node, oldStatus);
        }
    }
    void notifyRingRebuilt(const string &reason, size_t total) {
        for (RingChangeListener *listener : listeners_) {
            listener->onRingRebuilt(reason, total);
        }
    }

    unique_ptr<HashFunction> hashFunction_;              // Strategy (swappable)
    int baseVirtualNodes_;                               // per node, weight se multiply hota
    int replicationFactor_;                              // har key ki kitni copies
    map<uint32_t, VirtualNode> ring_;                    // ⭐ SORTED ring (position -> vnode)
    unordered_map<string, unique_ptr<Node>> nodesById_;  // physical nodes ka MAALIK
    vector<RingChangeListener *> listeners_;             // Observers (non-owning)
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_CORE_CONSISTENTHASHRING_H
