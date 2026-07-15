/*
    ===========================================================================
    APPROACH 3: CYCLE CANCELLATION (Graph-based Approach)
    ===========================================================================

    Idea:
    ------
    Debts ko ek DIRECTED WEIGHTED GRAPH ki tarah socho:
      - Har USER ek NODE hai
      - Agar "X owes Y" (X ko Y ko dena hai), to ek EDGE hoga X -> Y
        jiska weight (amount) hoga

    Ab is graph me agar koi CYCLE exist karta hai (jaise A->B->C->A),
    to iska matlab hai ki inn logo ke beech "circular debt" hai jo
    bina kisi ke financial position badle CANCEL ki ja sakti hai!

    Example:
      A owes B: 100   (A -> B, weight 100)
      B owes C: 100   (B -> C, weight 100)
      C owes A: 100   (C -> A, weight 100)

    Yeh ek perfect cycle hai. Ismein sabka net balance already 0 hai,
    isliye in teeno ke beech koi bhi transaction hone ki zaroorat nahi!

    ALGORITHM STEPS:
    ------------------
    1. Saare balances se ek directed graph banao (adjacency list)
    2. Jab tak graph me koi cycle exist kare:
         a. DFS/BFS se ek cycle dhundo
         b. Us cycle ke andar jo SABSE CHOTA edge-weight hai, wahi
            "bottleneck" amount hoga
         c. Us bottleneck amount ko cycle ke HAR edge se subtract kar do
         d. Jis edge ka weight 0 ho jaye, use graph se hata do
    3. Jab koi cycle na bache (graph ab acyclic/DAG ban jaye), to jo
       bhi edges bachi hain wahi FINAL SIMPLIFIED TRANSACTIONS hain

    Time Complexity : O(V × E) roughly — har cycle-detection pass O(V+E)
                       leta hai, aur worst case me O(E) cycles ho sakte hain
    Space Complexity: O(V + E)   [adjacency list ke liye]

    Yeh approach conceptually sabse "clean" hai kyunki yeh explicitly
    circular debts ko target karke cancel karti hai — baaki (non-cyclic)
    part automatically simplify ho jata hai.
    ===========================================================================
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

struct Transaction
{
    string from;
    string to;
    double amount;
    Transaction(string f, string t, double a) : from(f), to(t), amount(a) {}
};

class CycleCancellationSimplifier
{
private:
    static constexpr double EPSILON = 0.01;

    // -------------------------------------------------------------------
    // DFS based Cycle Detection function.
    //
    // graph[u][v] = weight  =>  edge u -> v exists with that weight
    //                            (matlab u ko v ko utna dena hai)
    //
    // Yeh function ek cycle dhundta hai aur agar mil jaye to us cycle
    // ke saare nodes "path" vector me store kar deta hai (in order).
    //
    // visited[] : kaunse nodes abhi tak DFS me touch ho chuke hain
    // inStack[] : kaunse nodes CURRENT recursion path me hain (isse
    //             hume "back edge" mil jayegi jo cycle indicate karti hai)
    // path[]    : current DFS path (cycle reconstruct karne ke liye)
    // -------------------------------------------------------------------
    static bool findCycleDFS(const string &node,
                             map<string, map<string, double>> &graph,
                             map<string, bool> &visited,
                             map<string, bool> &inStack,
                             vector<string> &path)
    {

        visited[node] = true;
        inStack[node] = true;
        path.push_back(node);

        // Node ke saare outgoing edges explore karo
        for (auto &edge : graph[node])
        {
            string neighbor = edge.first;
            double weight = edge.second;

            // Sirf valid (non-zero) edges hi consider karo
            if (weight <= EPSILON)
                continue;

            if (!visited[neighbor])
            {
                // Neighbor abhi explore nahi hua — DFS aage badhao
                if (findCycleDFS(neighbor, graph, visited, inStack, path))
                {
                    return true; // Cycle mil gayi neeche kahi, upar bubble up karo
                }
            }
            else if (inStack[neighbor])
            {
                // BACK EDGE mila! Matlab "neighbor" already current path
                // me hai — yehi CYCLE ka starting point hai.
                // Path ko trim karke sirf cycle wala portion rakho.
                path.push_back(neighbor); // cycle close karne ke liye end me daala
                while (path.front() != neighbor)
                {
                    path.erase(path.begin());
                }
                return true;
            }
        }

        // Is node ke saare neighbors explore ho gaye bina cycle mile —
        // backtrack karo (stack se hatao, path se bhi hatao)
        inStack[node] = false;
        path.pop_back();
        return false;
    }

    // -------------------------------------------------------------------
    // Ek cycle ko cancel karna: cycle ke andar minimum edge-weight
    // (bottleneck) dhundo aur use pure cycle ke har edge se subtract
    // kar do.
    // -------------------------------------------------------------------
    static void cancelCycle(vector<string> &cycle,
                            map<string, map<string, double>> &graph)
    {

        // Cycle ka bottleneck amount dhundo (sabse chota edge weight)
        double bottleneck = 1e18;
        for (int i = 0; i + 1 < (int)cycle.size(); i++)
        {
            double w = graph[cycle[i]][cycle[i + 1]];
            bottleneck = min(bottleneck, w);
        }

        // Har edge se bottleneck amount subtract karo
        for (int i = 0; i + 1 < (int)cycle.size(); i++)
        {
            graph[cycle[i]][cycle[i + 1]] -= bottleneck;

            // Agar edge ka weight ~0 ho gaya, to use map se hata do
            // (taaki future DFS calls me clutter na ho)
            if (graph[cycle[i]][cycle[i + 1]] < EPSILON)
            {
                graph[cycle[i]].erase(cycle[i + 1]);
            }
        }
    }

public:
    // -------------------------------------------------------------------
    // Main function: Raw balances lekar pehle cycles cancel karta hai,
    // phir bache hue (acyclic) graph ko hi final transactions bana deta hai
    // -------------------------------------------------------------------
    static vector<Transaction> simplifyDebts(
        map<string, map<string, double>> groupBalances)
    {

        // ---------------------------------------------------------------
        // Graph banate hain: agar balances[A][B] = amount hai (matlab
        // "B owes A"), to graph me edge B -> A hoga weight amount ke sath
        // (kyunki B ko A ko dena hai, B se A ki taraf edge)
        // ---------------------------------------------------------------
        map<string, map<string, double>> graph;
        for (auto &entry : groupBalances)
        {
            string creditor = entry.first;
            for (auto &inner : entry.second)
            {
                string debtor = inner.first;
                double amount = inner.second;
                if (amount > EPSILON)
                {
                    // debtor -> creditor edge (debtor ko creditor ko dena hai)
                    graph[debtor][creditor] += amount;
                }
            }
            // Ensure graph me key exist kare even if no outgoing edges
            if (graph.find(creditor) == graph.end())
            {
                graph[creditor] = map<string, double>();
            }
        }

        // ---------------------------------------------------------------
        // STEP: Jab tak graph me cycle milti rahe, use cancel karte raho
        // ---------------------------------------------------------------
        while (true)
        {
            map<string, bool> visited, inStack;
            vector<string> path;
            bool cycleFound = false;

            for (auto &nodeEntry : graph)
            {
                if (visited[nodeEntry.first])
                    continue;

                path.clear();
                inStack.clear(); // naya DFS attempt, stack reset karo
                if (findCycleDFS(nodeEntry.first, graph, visited, inStack, path))
                {
                    cancelCycle(path, graph); // cycle mili -> turant cancel karo
                    cycleFound = true;
                    break; // graph badal gaya hai, isliye fresh DFS restart karenge
                }
            }

            if (!cycleFound)
                break; // Ab koi cycle nahi bachi -> loop khatam
        }

        // ---------------------------------------------------------------
        // Ab jo bhi edges bachi hain graph me (acyclic ho chuka hai),
        // wahi hamari FINAL SIMPLIFIED TRANSACTIONS hain
        // ---------------------------------------------------------------
        vector<Transaction> result;
        for (auto &entry : graph)
        {
            string debtor = entry.first;
            for (auto &inner : entry.second)
            {
                string creditor = inner.first;
                double amount = round(inner.second * 100.0) / 100.0;
                if (amount > EPSILON)
                {
                    result.push_back(Transaction(debtor, creditor, amount));
                }
            }
        }

        return result;
    }

    static void printTransactions(const vector<Transaction> &transactions)
    {
        if (transactions.empty())
        {
            cout << "Koi outstanding debt nahi hai. Sab settled hai! ✅" << endl;
            return;
        }
        cout << "\n=========== Cycle Cancellation Transactions ===========" << endl;
        cout << "Total Transactions Required: " << transactions.size() << endl;
        cout << "----------------------------------------------------------" << endl;
        for (const auto &t : transactions)
        {
            cout << t.from << " ---> pays Rs " << fixed << setprecision(2)
                 << t.amount << " ---> " << t.to << endl;
        }
        cout << "============================================================\n"
             << endl;
    }
};

// ---------------------------------------------------------------------------
// MAIN: Do test cases -
//   1. Same tangled example jo pehle dono algorithms me use kiya
//   2. Ek PURE CIRCULAR debt example, taaki cycle-cancellation ka
//      real fayda dikhaya ja sake (yeh case dikhayega ki isme 0
//      transactions honi chahiye!)
// ---------------------------------------------------------------------------
int main()
{
    // ---------------- TEST CASE 1: Same as pehle ----------------
    map<string, map<string, double>> balances1;
    balances1["A"]["B"] = 500; // B owes A
    balances1["C"]["A"] = 300; // A owes C
    balances1["D"]["C"] = 200; // C owes D
    balances1["B"]["D"] = 100; // D owes B

    cout << "=========== TEST 1: Tangled Balances ===========" << endl;
    cout << "B owes A: Rs 500 | A owes C: Rs 300 | C owes D: Rs 200 | D owes B: Rs 100" << endl;

    vector<Transaction> result1 = CycleCancellationSimplifier::simplifyDebts(balances1);
    CycleCancellationSimplifier::printTransactions(result1);

    // ---------------- TEST CASE 2: Pure Circular Debt ----------------
    // A owes B 100, B owes C 100, C owes A 100
    // Yeh perfect cycle hai -> result me ZERO transactions honi chahiye!
    map<string, map<string, double>> balances2;
    balances2["B"]["A"] = 100; // A owes B
    balances2["C"]["B"] = 100; // B owes C
    balances2["A"]["C"] = 100; // C owes A

    cout << "=========== TEST 2: Pure Circular Debt (Should Cancel Out) ===========" << endl;
    cout << "A owes B: Rs 100 | B owes C: Rs 100 | C owes A: Rs 100" << endl;

    vector<Transaction> result2 = CycleCancellationSimplifier::simplifyDebts(balances2);
    CycleCancellationSimplifier::printTransactions(result2);

    return 0;
}