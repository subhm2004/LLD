/**
 * 01 — Multi-threaded merge sort: parallelism analysis (no sort run).
 */
#include <iostream>

using namespace std;

int main() {
    cout << "====== Multi-threaded Merge Sort — Overview ======\n\n";

    cout << "Sequential merge sort: O(n log n)\n";
    cout << "  divide -> sort left + sort right -> merge (serial combine)\n\n";

    cout << "Parallel idea:\n";
    cout << "  Sort left and right SUBRANGES on different threads.\n";
    cout << "  MERGE step stays serial in basic versions (Amdahl bottleneck).\n\n";

    cout << "Approaches in this folder:\n";
    cout << "  02 — sequential baseline\n";
    cout << "  03 — thread-per-subtask (naive; threshold cutoff)\n";
    cout << "  04 — fixed thread pool + threshold\n";
    cout << "  05 — fork-join via std::async\n";
    cout << "  06 — compare timings (same input)\n\n";

    cout << "Key interview points:\n";
    cout << "  - Use THRESHOLD: small subarrays -> sequential (avoid thread explosion)\n";
    cout << "  - Thread creation cost >> work for tiny ranges\n";
    cout << "  - Merge phase limits speedup unless parallel merge (advanced)\n";
    cout << "  - Production: thread pool or fork-join framework (OpenMP, TBB)\n\n";

    return 0;
}
