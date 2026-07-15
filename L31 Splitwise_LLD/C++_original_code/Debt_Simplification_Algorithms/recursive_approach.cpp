/*
    ===========================================================================
    APPROACH 2: DFS / RECURSIVE GREEDY (Classic GFG-style solution)
    ===========================================================================

    Yeh pehle wale Greedy Algorithm (sorting + two-pointer) ka hi ek
    alternative implementation hai, lekin is baar:
      - Sorting NAHI karte (isliye O(n log n) ki jagah O(n²) ban jata hai)
      - Har round me LINEAR SCAN karke max creditor aur max debtor dhundte
        hain
      - RECURSION use karte hain (function khud ko baar-baar call karta hai
        jab tak saare balances 0 na ho jayein)

    Yeh version un logo ke liye easy hai jo:
      - Sorting ka overhead avoid karna chahte hain (chhote inputs ke liye)
      - Recursive/functional style me sochna prefer karte hain
      - Interview me quickly likhna chahte hain (kam lines of code)

    LOGIC same hai jo pehle wale me tha:
      1. Net amount nikalo har user ka
      2. Max credit wale (+ve) aur max debit wale (-ve) ko dhundo
      3. min(credit, debit) settle karo, ek transaction record karo
      4. Jiska balance 0 ho gaya, use array se "remove" kar do (mark as 0)
      5. Recursively wapas call karo jab tak koi bhi non-zero balance na bache

    Time Complexity : O(n²)   [har recursive call me O(n) scan hota hai,
                               aur worst case me n baar recursion hoga]
    Space Complexity: O(n)    [recursion stack ke liye]
    ===========================================================================
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
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

class DFSDebtSimplifier
{
private:
    static const int NO_INDEX = -1;
    static constexpr double EPSILON = 0.01;

    // -------------------------------------------------------------------
    // Helper function: Array me sabse zyada POSITIVE balance wale ka
    // index dhundhna (yeh "max creditor" hoga — jisko sabse zyada
    // paisa milna hai)
    // -------------------------------------------------------------------
    static int getMaxCreditIndex(const vector<double> &amounts)
    {
        int maxIndex = 0;
        for (int i = 1; i < (int)amounts.size(); i++)
        {
            if (amounts[i] > amounts[maxIndex])
            {
                maxIndex = i;
            }
        }
        return maxIndex;
    }

    // -------------------------------------------------------------------
    // Helper function: Array me sabse zyada NEGATIVE balance wale ka
    // index dhundhna (yeh "max debtor" hoga — jisko sabse zyada
    // paisa dena hai)
    // -------------------------------------------------------------------
    static int getMaxDebitIndex(const vector<double> &amounts)
    {
        int minIndex = 0;
        for (int i = 1; i < (int)amounts.size(); i++)
        {
            if (amounts[i] < amounts[minIndex])
            {
                minIndex = i;
            }
        }
        return minIndex;
    }

    // -------------------------------------------------------------------
    // RECURSIVE FUNCTION: Yahi asli engine hai is algorithm ka.
    //
    // amounts[]: har user ka current net balance (positive = lena hai,
    //            negative = dena hai, 0 = settled)
    // names[]  : amounts[] ke corresponding user names (same index)
    // result   : jaha hum saari transactions record karte jayenge
    // -------------------------------------------------------------------
    static void settleRecursively(vector<double> &amounts,
                                  const vector<string> &names,
                                  vector<Transaction> &result)
    {

        // ---- BASE CASE ----
        // Max credit aur max debit dono nikalo. Agar dono ~0 hain,
        // matlab sabka balance settle ho chuka hai — recursion yahi
        // ruk jayegi.
        int maxCreditIdx = getMaxCreditIndex(amounts);
        int maxDebitIdx = getMaxDebitIndex(amounts);

        // Agar max credit bhi EPSILON se chhota hai aur max debit
        // (magnitude me) bhi EPSILON se chhota hai, to matlab kisi
        // ka bhi outstanding balance nahi bacha — recursion stop.
        if (amounts[maxCreditIdx] < EPSILON &&
            amounts[maxDebitIdx] > -EPSILON)
        {
            return; // Base case hit — saari settlements ho chuki hain
        }

        // ---- RECURSIVE CASE ----
        // Max creditor aur max debtor ke beech jo bhi minimum
        // (magnitude me) hai, utna settle karo is round me.
        double settleAmount = min(amounts[maxCreditIdx],
                                  -amounts[maxDebitIdx]);
        settleAmount = round(settleAmount * 100.0) / 100.0;

        if (settleAmount > 0)
        {
            // Debtor pays Creditor
            result.push_back(Transaction(names[maxDebitIdx],
                                         names[maxCreditIdx],
                                         settleAmount));
        }

        // Dono ke balances update karo — jo settle ho gaya wo minus
        // kar do dono se
        amounts[maxCreditIdx] -= settleAmount;
        amounts[maxDebitIdx] += settleAmount;

        // Recursion: Ab wapas se pura array scan karke agla max
        // creditor/debtor dhundo aur unhe settle karo. Yeh chalte
        // rahega jab tak base case hit na ho.
        settleRecursively(amounts, names, result);
    }

public:
    // -------------------------------------------------------------------
    // Public entry-point: Raw balances map lekar recursion shuru karta hai
    // -------------------------------------------------------------------
    static vector<Transaction> simplifyDebts(
        map<string, map<string, double>> &groupBalances)
    {

        // STEP 1: Net amount nikalna (same logic jo pehle Greedy me tha)
        map<string, double> netAmountMap;
        for (auto &entry : groupBalances)
        {
            netAmountMap[entry.first] += 0;
            for (auto &inner : entry.second)
            {
                netAmountMap[inner.first] += 0;
            }
        }
        for (auto &entry : groupBalances)
        {
            string creditorId = entry.first;
            for (auto &inner : entry.second)
            {
                string debtorId = inner.first;
                double amount = inner.second;
                if (amount > 0)
                {
                    netAmountMap[creditorId] += amount;
                    netAmountMap[debtorId] -= amount;
                }
            }
        }

        // STEP 2: Map ko do parallel vectors me convert karna (names[]
        // aur amounts[]) — kyunki recursion me index-based access
        // karna easy/fast hota hai map ke mukable
        vector<string> names;
        vector<double> amounts;
        for (auto &entry : netAmountMap)
        {
            names.push_back(entry.first);
            amounts.push_back(entry.second);
        }

        // STEP 3: Recursion shuru karo
        vector<Transaction> result;
        settleRecursively(amounts, names, result);

        return result;
    }

    static void printTransactions(const vector<Transaction> &transactions)
    {
        if (transactions.empty())
        {
            cout << "Koi outstanding debt nahi hai. Sab settled hai! ✅" << endl;
            return;
        }
        cout << "\n=========== DFS/Recursive Greedy Transactions ===========" << endl;
        cout << "Total Transactions Required: " << transactions.size() << endl;
        cout << "-----------------------------------------------------------" << endl;
        for (const auto &t : transactions)
        {
            cout << t.from << " ---> pays Rs " << fixed << setprecision(2)
                 << t.amount << " ---> " << t.to << endl;
        }
        cout << "=============================================================\n"
             << endl;
    }
};

// ---------------------------------------------------------------------------
// MAIN: Same test case jo pehle Greedy algorithm me use kiya tha, taaki
// dono ke results compare kiye ja sakein (result same hi aana chahiye,
// kyunki dono hi ek jaisa greedy logic follow karte hain — bas
// implementation style alag hai)
// ---------------------------------------------------------------------------
int main()
{
    map<string, map<string, double>> balances;

    // B owes A 500 => balances["A"]["B"] = 500
    balances["A"]["B"] = 500;
    // A owes C 300 => balances["C"]["A"] = 300
    balances["C"]["A"] = 300;
    // C owes D 200 => balances["D"]["C"] = 200
    balances["D"]["C"] = 200;
    // D owes B 100 => balances["B"]["D"] = 100
    balances["B"]["D"] = 100;

    cout << "=========== Original (Tangled) Balances ===========" << endl;
    cout << "B owes A: Rs 500" << endl;
    cout << "A owes C: Rs 300" << endl;
    cout << "C owes D: Rs 200" << endl;
    cout << "D owes B: Rs 100" << endl;

    vector<Transaction> result = DFSDebtSimplifier::simplifyDebts(balances);
    DFSDebtSimplifier::printTransactions(result);

    return 0;
}