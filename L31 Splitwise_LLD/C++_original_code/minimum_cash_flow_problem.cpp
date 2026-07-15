// ye hai wo debt simplification algorithm jo ki minimum cash flow problem ko solve karta hai.

/*
    ===========================================================================
    DEBT SIMPLIFICATION ALGORITHM (Minimum Cash Flow Problem) (Greedy Approach)
    ===========================================================================

    Kya hai yeh?
    -------------
    Jab ek group me multiple log aapas me paise udhaar lete/dete hain
    (jaise Splitwise app me hota hai), to transactions ka jaal (web) bahut
    complicated ho jata hai. Is algorithm ka kaam hai us jaal ko simplify
    karke minimum possible transactions me convert karna.

    Example:
      A ko B se 100 rupaye lene hain
      B ko C se 100 rupaye lene hain
      C ko A se 100 rupaye lene hain
    Yeh ek CIRCULAR debt hai — asal me kisi ko kisi ko kuch dena hi nahi
    chahiye! Yeh algorithm aise cycles ko automatically cancel kar deta hai.

    Approach: GREEDY ALGORITHM
    ----------------------------
    1. Har person ka "Net Balance" nikalo (total lena - total dena)
    2. Positive net wale logo ko "Creditors" (jinko paisa milna hai) bolte hain
    3. Negative net wale logo ko "Debtors" (jinko paisa dena hai) bolte hain
    4. Har step me sabse BADE creditor aur sabse BADE debtor ko match karo,
       jo bhi minimum amount hai wo settle karo, aur jiska balance 0 ho jaye
       usse list se hata do
    5. Yeh tab tak chalega jab tak saare balances 0 na ho jayein

    Time Complexity : O(n log n)   [sorting ki wajah se]
    Space Complexity: O(n)

    Note: Yeh algorithm TRUE OPTIMAL (minimum possible transactions) guarantee
    nahi karta — wo problem NP-Hard hai (Subset Sum se related). Lekin yeh
    ek bahut hi achha aur fast APPROXIMATION deta hai, isliye real-world
    apps (Splitwise jaisi) isi tarah ka greedy approach use karti hain.
    ===========================================================================
*/

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// ---------------------------------------------------------------------------
// Transaction struct: Ek simplified transaction ko represent karta hai.
// Matlab: "from" person ko "to" person ko "amount" dena hai.
// ---------------------------------------------------------------------------
struct Transaction
{
    string from;   // Jisne paise dene hain (debtor)
    string to;     // Jisko paise milne hain (creditor)
    double amount; // Kitna amount dena hai

    Transaction(string f, string t, double a) : from(f), to(t), amount(a) {}
};

// ---------------------------------------------------------------------------
// DebtSimplifier Class
// ---------------------------------------------------------------------------
// Yeh class group ke raw balances (jisme har user ke multiple entries ho
// sakte hain) ko lekar minimum transactions ki list nikalti hai.
//
// Input format: adjacency map
//   balances[A][B] = 200  ka matlab hai ki "B owes A Rs 200"
//   (matlab A ko milna hai, B ko dena hai)
// ---------------------------------------------------------------------------
class DebtSimplifier
{
public:
    // -----------------------------------------------------------------------
    // Main function: simplifyDebts
    // -----------------------------------------------------------------------
    // Input : groupBalances -> raw balance sheet (multiple entries per user)
    // Output: vector<Transaction> -> minimum transactions ki list jo saare
    //         debts ko settle kar degi
    // -----------------------------------------------------------------------
    static vector<Transaction> simplifyDebts(
        map<string, map<string, double>> &groupBalances)
    {

        // ---------------------------------------------------------------
        // STEP 1: Har user ka NET AMOUNT nikalna
        // ---------------------------------------------------------------
        // netAmount[X] > 0  => X ko total milakar itna paisa milna hai
        // netAmount[X] < 0  => X ko total milakar itna paisa dena hai
        map<string, double> netAmount;

        // Pehle sabhi known users ko 0 se initialize kar dete hain,
        // taaki koi bhi user missed na ho (even if unka net exactly 0 ho)
        for (auto &entry : groupBalances)
        {
            netAmount[entry.first] += 0; // ensures key exists
            for (auto &inner : entry.second)
            {
                netAmount[inner.first] += 0; // ensures key exists
            }
        }

        // Ab actual net calculate karte hain.
        // groupBalances[A][B] = amount  =>  B owes A "amount"
        // Iska matlab: A ka net badhega (+amount), B ka net ghategga (-amount)
        for (auto &entry : groupBalances)
        {
            string creditorId = entry.first;
            for (auto &inner : entry.second)
            {
                string debtorId = inner.first;
                double amount = inner.second;

                // Sirf positive amounts hi count karenge, taaki
                // double-counting na ho (kyunki balances[A][B] aur
                // balances[B][A] dono ho sakte hain, ek positive ek negative)
                if (amount > 0)
                {
                    netAmount[creditorId] += amount;
                    netAmount[debtorId] -= amount;
                }
            }
        }

        // ---------------------------------------------------------------
        // STEP 2: Creditors aur Debtors ko alag-alag list me daalna
        // ---------------------------------------------------------------
        // creditors -> jinka net positive hai (inhe paisa MILNA hai)
        // debtors   -> jinka net negative hai (inhe paisa DENA hai)
        vector<pair<string, double>> creditors;
        vector<pair<string, double>> debtors;

        const double EPSILON = 0.01; // floating point precision error handle
                                     // karne ke liye threshold

        for (auto &entry : netAmount)
        {
            if (entry.second > EPSILON)
            {
                creditors.push_back({entry.first, entry.second});
            }
            else if (entry.second < -EPSILON)
            {
                // debtor ka amount hum positive store karte hain (easy
                // comparison ke liye), isliye -1 se multiply kar diya
                debtors.push_back({entry.first, -entry.second});
            }
            // Agar net ~0 hai, to us user ka koi outstanding balance nahi
            // hai, use kisi list me daalne ki zaroorat nahi.
        }

        // ---------------------------------------------------------------
        // STEP 3: Dono list ko DESCENDING order me sort karna
        // ---------------------------------------------------------------
        // Sabse bade balance wale logo ko pehle settle karna better hai,
        // isse transactions ki total count kam hone ke chances badh jate
        // hain (though guaranteed optimal nahi hai).
        sort(creditors.begin(), creditors.end(),
             [](const pair<string, double> &a, const pair<string, double> &b)
             {
                 return a.second > b.second;
             });

        sort(debtors.begin(), debtors.end(),
             [](const pair<string, double> &a, const pair<string, double> &b)
             {
                 return a.second > b.second;
             });

        // ---------------------------------------------------------------
        // STEP 4: GREEDY TWO-POINTER SETTLEMENT
        // ---------------------------------------------------------------
        // i -> creditors list ka current pointer
        // j -> debtors list ka current pointer
        //
        // Logic: Har iteration me sabse bade creditor aur sabse bade debtor
        // ko match karo. Jo bhi dono me minimum amount hai, utna settle
        // karke ek Transaction bana do. Jiska balance khatam ho jaye,
        // usko aage badha do (pointer increment).
        vector<Transaction> result;
        int i = 0, j = 0;

        while (i < (int)creditors.size() && j < (int)debtors.size())
        {
            string creditorId = creditors[i].first;
            string debtorId = debtors[j].first;
            double creditorAmount = creditors[i].second;
            double debtorAmount = debtors[j].second;

            // Dono me se jo minimum hai, wahi is round me settle hoga
            double settleAmount = min(creditorAmount, debtorAmount);

            // Round karke 2 decimal places tak rakhte hain (currency ke liye)
            settleAmount = round(settleAmount * 100.0) / 100.0;

            if (settleAmount > 0)
            {
                // Debtor -> Creditor transaction record karo
                result.push_back(Transaction(debtorId, creditorId, settleAmount));
            }

            // Dono ke balances me se settle hua amount minus karo
            creditors[i].second -= settleAmount;
            debtors[j].second -= settleAmount;

            // Agar creditor ka balance clear (0 ke bahut paas) ho gaya,
            // to agle creditor par chale jao
            if (creditors[i].second < EPSILON)
            {
                i++;
            }

            // Agar debtor ka balance clear ho gaya, to agle debtor par
            // chale jao
            if (debtors[j].second < EPSILON)
            {
                j++;
            }
        }

        return result;
    }

    // -----------------------------------------------------------------------
    // Helper function: Transactions ko sundar tarike se print karna
    // -----------------------------------------------------------------------
    static void printTransactions(const vector<Transaction> &transactions)
    {
        if (transactions.empty())
        {
            cout << "Koi outstanding debt nahi hai. Sab settled hai! ✅" << endl;
            return;
        }

        cout << "\n=========== Simplified Transactions ===========" << endl;
        cout << "Total Transactions Required: " << transactions.size() << endl;
        cout << "-------------------------------------------------" << endl;

        for (const auto &t : transactions)
        {
            cout << t.from << " ---> pays Rs " << fixed << setprecision(2)
                 << t.amount << " ---> " << t.to << endl;
        }
        cout << "=================================================\n"
             << endl;
    }
};

// ---------------------------------------------------------------------------
// MAIN FUNCTION: Testing / Demo
// ---------------------------------------------------------------------------
// Yaha hum ek example scenario le rahe hain jisme complex, tangled debts
// hain, aur dikhayenge ki algorithm unhe kaise simplify karta hai.
// ---------------------------------------------------------------------------
int main()
{

    // -------------------------------------------------------------------
    // Raw Balances Setup
    // -------------------------------------------------------------------
    // Format: balances[X][Y] = amount  =>  "Y owes X ka amount"
    //
    // Scenario (4 log: A, B, C, D):
    //   B owes A: 500
    //   A owes C: 300
    //   C owes D: 200
    //   D owes B: 100
    //
    // Yeh ek tangled/complicated set hai jisme circular dependency bhi
    // chhupi hui hai. Dekhte hain simplification ke baad kitne transactions
    // bachte hain.
    // -------------------------------------------------------------------
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
    cout << "Total raw entries: 4" << endl;

    // -------------------------------------------------------------------
    // Algorithm Call
    // -------------------------------------------------------------------
    vector<Transaction> simplified = DebtSimplifier::simplifyDebts(balances);

    // -------------------------------------------------------------------
    // Result Print
    // -------------------------------------------------------------------
    DebtSimplifier::printTransactions(simplified);

    // -------------------------------------------------------------------
    // Manual Verification (samajhne ke liye):
    // -------------------------------------------------------------------
    // Net calculation:
    //   A: +500 (from B) -300 (to C) = +200   -> Creditor
    //   B: -500 (to A)   +100 (from D) = -400 -> Debtor
    //   C: +300 (from A) -200 (to D) = +100   -> Creditor
    //   D: +200 (from C) -100 (to B) = +100   -> Creditor
    //
    // Creditors: A(+200), C(+100), D(+100)
    // Debtors:   B(-400)
    //
    // Greedy matching:
    //   B pays A: 200   (A settled, B remaining = 200)
    //   B pays C: 100   (C settled, B remaining = 100)
    //   B pays D: 100   (D settled, B settled)
    //
    // Result: 4 original entries -> sirf 3 transactions me simplify ho gaye!
    // -------------------------------------------------------------------

    return 0;
}