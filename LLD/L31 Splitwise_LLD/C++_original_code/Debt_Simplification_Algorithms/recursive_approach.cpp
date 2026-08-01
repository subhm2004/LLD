/*
    ===========================================================================
    APPROACH 2: DFS / RECURSIVE GREEDY (Classic GFG-style solution)
    ===========================================================================

    Yeh `minimum_cash_flow_problem.cpp` wale Greedy Algorithm (sorting +
    two-pointer) ka hi ALTERNATIVE implementation hai. LOGIC bilkul same hai,
    bas likhne ka tareeka alag:

      | Cheez            | Greedy (sort+2ptr)      | Ye wala (recursive)        |
      |------------------|-------------------------|----------------------------|
      | Max kaise milta  | ek baar SORT kar lo      | har round me LINEAR SCAN   |
      | Loop kaise       | while + two pointers     | RECURSION (khud ko call)   |
      | Complexity       | O(n log n)               | O(n^2)                     |
      | Result           | ------- DONO KA SAME -------                       |

    Dono ka natija ek jaisa aata hai kyunki soch ek hi hai:
      "sabse bade creditor ko sabse bade debtor se milao, jitna ho sake settle
       karo, jiska balance khatam ho jaye use hata do — dohrao."

    LOGIC (steps):
      1. Net amount nikalo har user ka (kitna lena / kitna dena)
      2. Max credit wale (+ve) aur max debit wale (-ve) ko dhundo
      3. min(credit, debit) settle karo, ek transaction record karo
      4. Dono ke balances update karo
      5. Recursively wapas call karo jab tak koi meaningful balance na bache

    Time Complexity : O(n^2)  [har recursive call me O(n) scan, aur worst case
                               me n baar recursion]
    Space Complexity: O(n)    [recursion stack]

    ⚠ Ye TRUE OPTIMAL nahi hai — minimum-transactions problem NP-Hard hai.
      Ye ek achha, fast APPROXIMATION hai (hamesha n-1 se kam transactions).

    ---------------------------------------------------------------------------
    KAB YE USE KARO, KAB GREEDY?
    ---------------------------------------------------------------------------
      Production / bade groups -> `minimum_cash_flow_problem.cpp` (O(n log n))
      Sirf seekhne / interview  -> ye wala (recursion ka soch samajhne ke liye)

    Dono ka OUTPUT same hota hai, isliye speed hi asli farak hai.

    ---------------------------------------------------------------------------
    ⭐ IS FILE ME 2 CRASH BUG THE — dono fix ho chuke hain (neeche detail me):
       1. INFINITE RECURSION -> stack overflow (sub-paisa amounts pe)
       2. Khaali input -> out-of-bounds read -> segfault
       Interesting baat: greedy version me ye dono bug NAHI the. Kyun? Kyunki
       wo shuru me hi chhote (< EPSILON) amounts ko list se filter kar deta hai,
       aur khaali list pe uska `while` loop bas chalta hi nahi. Ye wala raw
       array pe kaam karta hai — isliye usko khud guard karna padta hai.
       📌 Sabak: "same logic, alag implementation" ka matlab "same bugs" nahi.
          Har implementation ke apne edge cases hote hain.
    ===========================================================================
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>    // round(), fabs()
#include <iomanip>  // fixed, setprecision — currency format ke liye
#include <algorithm> // min()

using namespace std;

// ---------------------------------------------------------------------------
// Transaction: ek final settlement — "from" ko "to" ko "amount" dena hai.
// ---------------------------------------------------------------------------
struct Transaction
{
    string from;   // jisne paise DENE hain (debtor)
    string to;     // jisko paise MILNE hain (creditor)
    double amount; // kitna

    // Initializer list se seedha members initialize (body me assign karne se
    // behtar — ek extra step bach jaata hai)
    Transaction(string f, string t, double a) : from(f), to(t), amount(a) {}
};

// ---------------------------------------------------------------------------
// DFSDebtSimplifier
// ---------------------------------------------------------------------------
// Poori class `static` functions se bani hai — object banane ki zaroorat nahi
// (`DFSDebtSimplifier::simplifyDebts(...)` seedha call karo). Class ke paas
// apna koi state (member variable) nahi hai — ye sirf ek utility hai.
// ---------------------------------------------------------------------------
class DFSDebtSimplifier
{
private:
    // EPSILON = "itne se kam bacha to settled maan lo".
    //
    // Kyun chahiye? double me precision ki dikkat hoti hai — 0.1 + 0.2 exactly
    // 0.3 nahi hota, 0.30000000000000004 hota hai. Isliye `net == 0` check
    // karne ke bajaye `abs(net) < EPSILON` check karte hain, warna rounding ka
    // kachra hamesha "unsettled" dikhta rahega.
    static constexpr double EPSILON = 0.01; // 1 paisa

    // (Pehle yahan `static const int NO_INDEX = -1;` bhi tha — par wo poori
    //  file me kahin USE hi nahi hota tha. Dead code tha, isliye hata diya.)

    // -------------------------------------------------------------------
    // Helper: sabse zyada POSITIVE balance wale ka index dhundo
    // (yeh "max creditor" hai — jisko sabse zyada paisa MILNA hai)
    //
    // Ye sirf ek linear scan hai — O(n). Greedy version isi kaam ke liye
    // ek baar sort kar leta hai, isliye wo aage jaake tez padta hai.
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
    // Helper: sabse zyada NEGATIVE balance wale ka index dhundo
    // (yeh "max debtor" hai — jisko sabse zyada paisa DENA hai)
    //
    // Note: "max debtor" ka matlab sabse CHHOTA number (jaise -450), kyunki
    // dena hai to balance negative hota hai. Isliye yahan `<` compare hai.
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
    // RECURSIVE FUNCTION — yahi asli engine hai
    // -------------------------------------------------------------------
    // amounts[] : har user ka current net balance
    //             (+ = lena hai, - = dena hai, ~0 = settled)
    // names[]   : amounts[] ke corresponding naam (SAME index pe)
    //             -> ye "parallel arrays" hain: amounts[3] ka naam names[3]
    // result    : jaha saari transactions record hoti jaayengi
    //
    // `amounts` non-const reference hai — kyunki har round me hum ise MODIFY
    // karte hain (settle hua amount ghata dete hain). `names` const hai — wo
    // kabhi badalta nahi.
    // -------------------------------------------------------------------
    static void settleRecursively(vector<double> &amounts,
                                  const vector<string> &names,
                                  vector<Transaction> &result)
    {
        // ---- ✅ GUARD (BUG FIX #2): khaali list ----
        //
        // 🐛 Pehle ye check tha hi nahi. Agar `amounts` khaali hota (jaise
        // khaali groupBalances pass karne pe), to `getMaxCreditIndex()` bas
        // `maxIndex = 0` return kar deta (uska loop `i=1` se shuru hota hai,
        // to khaali vector pe chalta hi nahi). Phir neeche `amounts[0]` padha
        // jaata — ek KHAALI vector pe! Ye out-of-bounds read hai -> SEGFAULT.
        // (AddressSanitizer se test karke confirm kiya tha: SEGV on 0x0.)
        //
        // Ab pehle hi nikal jaate hain — kuch hai hi nahi to settle kya karein.
        if (amounts.empty())
        {
            return;
        }

        // Max credit aur max debit dono nikalo (har baar poora scan — O(n))
        int maxCreditIdx = getMaxCreditIndex(amounts);
        int maxDebitIdx = getMaxDebitIndex(amounts);

        // ---- ✅ BASE CASE (BUG FIX #1): recursion kab rukegi ----
        //
        // 🐛 Pehle yahan `&&` tha:
        //       if (amounts[maxCreditIdx] < EPSILON &&
        //           amounts[maxDebitIdx] > -EPSILON) return;
        //    Matlab: "rukо sirf tab jab DONO taraf kuch na bacha ho."
        //
        //    Ye INFINITE RECURSION -> STACK OVERFLOW de deta tha. Kaise?
        //    Ek asli case (test karke confirm kiya, segfault mila):
        //
        //      A, B, C ko 0.004-0.004-0.004 lena hai; D ko 0.012 dena hai
        //      -> amounts = [+0.004, +0.004, +0.004, -0.012]   (sum = 0 ✓)
        //
        //      maxCredit = 0.004  -> 0.004 < 0.01 ✓ (ye taraf settled hai)
        //      maxDebit  = -0.012 -> -0.012 > -0.01 ✗ (ye taraf nahi)
        //      `&&` -> false -> base case HIT NAHI hua -> aage badh gaya
        //
        //      settleAmount = min(0.004, 0.012) = 0.004
        //      round(0.004 * 100) / 100 = round(0.4)/100 = 0/100 = 0.00 !!
        //
        //      -> settleAmount = 0 -> koi transaction nahi
        //      -> `amounts` me se 0 ghataya -> STATE BILKUL WAHI KA WAHI
        //      -> phir se recursion -> phir wahi -> ANANT (infinite) 🔁💥
        //
        // ✅ Ab `||` hai: "ruko jaise hi KISI EK taraf kuch meaningful na bache."
        //    Ye logically bhi sahi hai — transaction ke liye creditor AUR
        //    debtor DONO chahiye. Ek bhi taraf sirf dust (< 1 paisa) bacha ho,
        //    to koi kaam ka settlement ho hi nahi sakta. Ab upar wale case me
        //    maxCredit(0.004) < 0.01 -> turant return -> koi crash nahi.
        //
        // 📌 Isse ye bilkul greedy version jaisa behave karta hai — wo bhi
        //    shuru me hi `> EPSILON` wale filter se aise dust ko list me aane
        //    hi nahi deta. Dono ka natija ab identical (test karke confirm).
        if (amounts[maxCreditIdx] < EPSILON ||
            amounts[maxDebitIdx] > -EPSILON)
        {
            return; // Base case — settle karne layak kuch nahi bacha
        }

        // ---- RECURSIVE CASE ----
        //
        // Dono me se jo MINIMUM hai, utna hi settle ho sakta hai.
        // Kyun minimum? Agar creditor ko sirf 200 lene hain par debtor ko 500
        // dene hain, to is transaction me sirf 200 hi ja sakte hain — creditor
        // ka hisaab poora ho jaayega, aur debtor ka 300 bacha rahega jo agle
        // creditor ko jaayega.
        //
        // `-amounts[maxDebitIdx]` -> debit negative hota hai (-450), usko
        // positive (450) bana ke compare karte hain, warna min() galat nikalta.
        double settleAmount = min(amounts[maxCreditIdx],
                                  -amounts[maxDebitIdx]);

        // Paise tak round karo (currency me aadha paisa nahi hota).
        // x*100 -> paise me lao, round -> nearest paisa, /100 -> wapas rupaye.
        // Isse 199.99999999 jaisa float kachra saaf hoke 200.00 ban jaata hai.
        settleAmount = round(settleAmount * 100.0) / 100.0;

        if (settleAmount > 0)
        {
            // Debtor -> Creditor ko pay karega
            result.push_back(Transaction(names[maxDebitIdx],
                                         names[maxCreditIdx],
                                         settleAmount));
        }

        // Dono ke balances update karo:
        //   creditor ka lena kam hua   -> minus
        //   debtor ka dena kam hua     -> plus (kyunki wo negative tha,
        //                                 plus karne se 0 ki taraf jaata hai)
        amounts[maxCreditIdx] -= settleAmount;
        amounts[maxDebitIdx] += settleAmount;

        // ---- RECURSION ----
        // Ab dobara poora array scan karke agla max creditor/debtar dhundo.
        // Har call me kam se kam ek banda (ya dono) 0 ke kareeb pahunch jaata
        // hai, isliye recursion aage badhti hai aur base case tak pahunchti hai.
        //
        // 💡 Ye ek "tail call" hai (function ka aakhri kaam) — compiler isko
        // aksar loop me badal deta hai (-O2 pe), to stack depth ki chinta
        // aam taur pe nahi. Phir bhi, -O0 (debug build) pe ye asli recursion
        // hoti hai — n logon ke liye zyada se zyada n-1 gehri.
        settleRecursively(amounts, names, result);
    }

public:
    // -------------------------------------------------------------------
    // Public entry-point: raw balances map lekar recursion shuru karta hai
    // -------------------------------------------------------------------
    // Input format (adjacency map):
    //     groupBalances[A][B] = 200   ka matlab "B owes A Rs 200"
    //     (outer key = creditor, inner key = debtor)
    //
    // ✅ `const &` — hum map ko sirf PADH rahe hain, badal nahi rahe.
    //    (Pehle plain `&` tha, isliye `const map` ya temporary pass hi nahi
    //     kar sakte the. Const lagane se compiler galti se modify karne se
    //     bhi rok deta hai.)
    // -------------------------------------------------------------------
    static vector<Transaction> simplifyDebts(
        const map<string, map<string, double>> &groupBalances)
    {
        // ---- STEP 1: Har user ka NET amount nikalna ----
        // netAmountMap[X] > 0 -> X ko kul milakar itna MILNA hai
        // netAmountMap[X] < 0 -> X ko kul milakar itna DENA hai
        map<string, double> netAmountMap;

        // Pehle sabhi naamon ki entry bana do (0 se), taaki koi banda chhoot
        // na jaye — chahe uska net exactly 0 hi kyun na ho.
        //
        // map me `[]` lagate hi, agar key nahi hai to wo apne aap 0.0 ke saath
        // ban jaati hai — bas isi ka fayda utha rahe hain.
        // (Pehle `netAmountMap[...] += 0;` likha tha — `+= 0` bekaar tha,
        //  kyunki `[]` khud hi 0 se entry bana deta hai.)
        for (const auto &entry : groupBalances)
        {
            netAmountMap[entry.first];
            for (const auto &inner : entry.second)
            {
                netAmountMap[inner.first];
            }
        }

        // Ab asli net calculate karo.
        // groupBalances[A][B] = amount  =>  B owes A
        //   -> A ka net BADHEGA  (+amount)
        //   -> B ka net GHATEGA  (-amount)
        for (const auto &entry : groupBalances)
        {
            const string &creditorId = entry.first;

            for (const auto &inner : entry.second)
            {
                const string &debtorId = inner.first;
                double amount = inner.second;

                // Sirf POSITIVE entries count karo — warna double counting ho
                // jaayegi (kyunki balances[A][B] aur balances[B][A] dono ho
                // sakte hain, ek positive ek negative).
                //
                // ⚠ Ye ek ASSUMPTION hai: har karza positive form me likha
                // hoga (balances[creditor][debtor] = +amount). Agar koi SIRF
                // negative form likhe (balances[A][B] = -50) bina mirror ke,
                // to wo debt chup-chaap gayab ho jaayegi.
                if (amount > 0)
                {
                    netAmountMap[creditorId] += amount;
                    netAmountMap[debtorId] -= amount;
                }
            }
        }

        // ---- STEP 2: Map ko do PARALLEL VECTORS me badalna ----
        //
        // Kyun? Kyunki recursion me baar-baar index se access karna hota hai
        // (amounts[maxCreditIdx]), aur vector me index access O(1) hota hai —
        // map me dhoondhna O(log n) padta.
        //
        // "Parallel arrays" ka matlab: names[i] aur amounts[i] EK HI bande ke
        // hain. Dono ka order kabhi mismatch nahi hona chahiye.
        vector<string> names;
        vector<double> amounts;
        for (const auto &entry : netAmountMap)
        {
            names.push_back(entry.first);
            amounts.push_back(entry.second);
        }

        // ---- STEP 3: Recursion shuru ----
        vector<Transaction> result;
        settleRecursively(amounts, names, result);

        return result;
    }

    // -------------------------------------------------------------------
    // Result ko readable format me print karna
    // -------------------------------------------------------------------
    static void printTransactions(const vector<Transaction> &transactions)
    {
        // Khaali result ka matlab: sab pehle se settled tha, ya saare debts
        // aapas me cancel ho gaye (jaise circular debt me hota hai).
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
            // `fixed << setprecision(2)` -> hamesha 2 decimal (paise) dikhao,
            // warna C++ "1e+03" jaisa kuch bhi print kar sakta hai.
            cout << t.from << " ---> pays Rs " << fixed << setprecision(2)
                 << t.amount << " ---> " << t.to << endl;
        }
        cout << "=============================================================\n"
             << endl;
    }
};

// ---------------------------------------------------------------------------
// MAIN: Wahi test case jo Greedy algorithm me use hua tha, taaki dono ke
// results compare kiye ja sakein.
//
// Result DONO ka same aana chahiye — kyunki logic ek hi hai, bas
// implementation style alag hai (sort+2ptr vs recursion).
// ---------------------------------------------------------------------------
int main()
{
    map<string, map<string, double>> balances;

    // Format: balances[Creditor][Debtor] = Amount
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

    // Haath se verify karne ke liye (net nikalte hain):
    //   A: +500 (B se lena) -300 (C ko dena)  = +200
    //   B: -500 (A ko dena) +100 (D se lena)  = -400
    //   C: +300 (A se lena) -200 (D ko dena)  = +100
    //   D: +200 (C se lena) -100 (B ko dena)  = +100
    //   Check: 200 - 400 + 100 + 100 = 0 ✓ (sum hamesha 0 hona chahiye)
    //
    //   Creditors: A(+200), C(+100), D(+100)
    //   Debtor   : B(-400)
    //   -> 4 tangled entries se sirf 3 transactions bachte hain.

    vector<Transaction> result = DFSDebtSimplifier::simplifyDebts(balances);
    DFSDebtSimplifier::printTransactions(result);

    return 0;
}
