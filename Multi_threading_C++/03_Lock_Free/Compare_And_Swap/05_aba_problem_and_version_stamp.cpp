/**
 * 05 — ABA problem + fix with version stamp (tagged head idea).
 *
 * We simulate stack head with (node_id, version) packed in uint64_t
 * so the demo is safe on 64-bit (no truncated pointers).
 *
 * Scenario without version:
 *   T1 reads head=(A, v0), plans pop -> head=B
 *   T2 pops A, pops B, pushes A back -> head=(A, v2)  same id, new version
 *   T1 CAS with stale (A, v0) must FAIL when version is checked
 */
#include <atomic>
#include <cstdint>
#include <iostream>

using namespace std;

static constexpr uint32_t kNodeA = 1;
static constexpr uint32_t kNodeB = 2;

static uint64_t pack(uint32_t node_id, uint32_t version) {
    return (static_cast<uint64_t>(version) << 32) | static_cast<uint64_t>(node_id);
}

static uint32_t unpackNode(uint64_t packed) { return static_cast<uint32_t>(packed & 0xFFFFFFFFu); }

static uint32_t unpackVersion(uint64_t packed) { return static_cast<uint32_t>(packed >> 32); }

int main() {
    cout << "====== 05 ABA Problem + Version Stamp ======\n\n";

    atomic<uint64_t> head{pack(kNodeA, 0)};

    cout << "Step 1: head = node A (id=1), version=0\n";
    uint64_t t1_snapshot = head.load();
    cout << "  T1 snapshot: node=" << unpackNode(t1_snapshot)
         << " version=" << unpackVersion(t1_snapshot) << "\n";
    cout << "  T1 plans: CAS head A -> B (pop)\n";

    cout << "\nStep 2: T2 pops A, pops B, pushes A back (pointer/id looks same)\n";
    head.store(pack(kNodeA, 2), memory_order_relaxed);
    cout << "  head now: node=" << unpackNode(head.load())
         << " version=" << unpackVersion(head.load()) << "  <- structure changed!\n";

    cout << "\nStep 3: T1 CAS with stale snapshot (A, v0) -> (B, v0)\n";
    uint64_t expected = t1_snapshot;
    uint64_t desired = pack(kNodeB, 0);
    bool stale_cas = head.compare_exchange_strong(expected, desired);
    cout << "  CAS success? " << (stale_cas ? "yes (unsafe in real lock-free stack)" : "no")
         << " — expected updated to node=" << unpackNode(expected)
         << " version=" << unpackVersion(expected) << "\n";

    cout << "\nReal ABA on lock-free stack: same pointer address, freed node reused.\n";
    cout << "Fix: tagged pointer / generation counter (this demo) or hazard pointers.\n";

    return 0;
}
