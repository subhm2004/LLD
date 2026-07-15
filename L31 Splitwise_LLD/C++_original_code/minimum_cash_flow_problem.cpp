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

// ---------------------------------------------------------------------------
// HEADER FILES — inki zaroorat kyu hai, ek-ek karke samajhte hain:
// ---------------------------------------------------------------------------
#include <iostream>  // cin, cout, endl jaise basic input/output ke liye
#include <vector>    // vector<T> dynamic array ke liye — humein Transaction
                     // aur creditors/debtors ki list store karni hai
#include <map>       // map<key, value> ordered key-value store ke liye —
                     // groupBalances aur netAmount store karne ke liye use hoga
#include <algorithm> // sort(), min() jaise ready-made algorithms ke liye
#include <iomanip>   // setprecision(), fixed — output ko 2 decimal places
                     // (currency format) me print karne ke liye
#include <cmath>     // round() function ke liye — floating point rounding

using namespace std; // baar baar std:: likhne se bachne ke liye

// ---------------------------------------------------------------------------
// Transaction struct: Ek simplified transaction ko represent karta hai.
// Matlab: "from" person ko "to" person ko "amount" dena hai.
// ---------------------------------------------------------------------------
// Yeh ek chhota sa data structure hai jisme sirf 3 cheeze store hoti hain:
//   1. from   -> kaun paisa de raha hai (debtor)
//   2. to     -> kise paisa mil raha hai (creditor)
//   3. amount -> kitna paisa
// Har final settlement (jo humara algorithm output karega) ek Transaction
// object ke roop me store hoga.
// ---------------------------------------------------------------------------
struct Transaction
{
    string from;   // Jisne paise dene hain (debtor) — is person ka naam/ID
    string to;     // Jisko paise milne hain (creditor) — is person ka naam/ID
    double amount; // Kitna amount dena hai — decimal isliye taaki paise
                   // (rupaye-paise / cents) precisely handle ho sake

    // Constructor: jab bhi hum "Transaction(f, t, a)" likhenge, ye teeno
    // values seedha struct ke members me copy ho jayengi (initializer list
    // ka use karke — ye C++ me efficient tareeka hai directly initialize
    // karne ka, bina extra assignment ke)
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
//
// Poori class "static" functions se bani hai — matlab humein is class ka
// koi object banane ki zaroorat nahi (jaise DebtSimplifier obj; nahi
// chahiye). Hum seedha DebtSimplifier::functionName() likh ke call kar
// sakte hain. Ye is liye kiya gaya hai kyunki is class ke paas apna koi
// "state" (member variables) nahi hai — ye sirf ek utility/helper class hai.
// ---------------------------------------------------------------------------
class DebtSimplifier
{
public:
    // -----------------------------------------------------------------------
    // Main function: simplifyDebts
    // -----------------------------------------------------------------------
    // Input : groupBalances -> raw balance sheet (multiple entries per user)
    //         (reference se pass kiya hai "&" ke through, taaki poora map
    //         copy na ho — bade group me ye copy costly ho sakti hai)
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
        //
        // Hum ek naya map bana rahe hain jisme key hai person ka naam,
        // aur value hai uska "net" balance (saare transactions ka jodh-ghata
        // karke jo final number bachta hai).
        map<string, double> netAmount;

        // -----------------------------------------------------------------
        // Pehle sabhi known users ko 0 se initialize kar dete hain,
        // taaki koi bhi user missed na ho (even if unka net exactly 0 ho).
        //
        // Yeh isliye zaroori hai kyunki agar hum directly "STEP ka doosra
        // loop" chala de, aur kisi bande ka balance coincidentally +100
        // aur -100 ho ke net 0 ban jaye, to bhi humein us bande ka entry
        // netAmount map me chahiye (taaki agar zaroorat pade to pata chale
        // ki wo "settled" hai, "missing" nahi).
        // -----------------------------------------------------------------
        for (auto &entry : groupBalances)
        {
            // entry.first  -> outer map ki key, matlab ek "creditor-ID"
            // entry.second -> ek inner map<string,double>, jisme uske
            //                 saare debtors aur unke amounts listed hain
            netAmount[entry.first] += 0; // ensures key exists
                                         // (map me agar key already nahi
                                         // hai, to ye automatically 0.0
                                         // value ke saath create ho jayegi)

            for (auto &inner : entry.second)
            {
                // inner.first  -> is creditor ka ek debtor ka naam
                // inner.second -> us debtor par kitna amount due hai
                netAmount[inner.first] += 0; // ensures key exists
            }
        }

        // -----------------------------------------------------------------
        // Ab actual net calculate karte hain.
        // groupBalances[A][B] = amount  =>  B owes A "amount"
        // Iska matlab: A ka net badhega (+amount), B ka net ghategga (-amount)
        // -----------------------------------------------------------------
        for (auto &entry : groupBalances)
        {
            string creditorId = entry.first; // outer key = jisko paisa milna hai

            for (auto &inner : entry.second)
            {
                string debtorId = inner.first; // inner key = jisko paisa dena hai
                double amount = inner.second;  // kitna amount

                // -------------------------------------------------------
                // Sirf positive amounts hi count karenge, taaki
                // double-counting na ho (kyunki balances[A][B] aur
                // balances[B][A] dono ho sakte hain, ek positive ek negative)
                //
                // Yani agar kisi galti se balances[A][B] = -50 jaisi negative
                // entry aa jaye, to us se skip kar denge — kyunki wo already
                // balances[B][A] = 50 ke roop me kahi represent ho chuki
                // hogi (ya honi chahiye). Ye ek safety check hai.
                // -------------------------------------------------------
                if (amount > 0)
                {
                    netAmount[creditorId] += amount; // creditor ka net badhega
                    netAmount[debtorId] -= amount;   // debtor ka net ghatega
                }
            }
        }

        // ---------------------------------------------------------------
        // STEP 2: Creditors aur Debtors ko alag-alag list me daalna
        // ---------------------------------------------------------------
        // creditors -> jinka net positive hai (inhe paisa MILNA hai)
        // debtors   -> jinka net negative hai (inhe paisa DENA hai)
        //
        // Hum yaha pair<string, double> ka vector bana rahe hain —
        // pair.first = person ka naam, pair.second = uska (absolute) amount.
        vector<pair<string, double>> creditors;
        vector<pair<string, double>> debtors;

        // EPSILON ek chhota sa threshold value hai. Floating point numbers
        // (double) me precision ki dikkat hoti hai — jaise 0.1 + 0.2 exactly
        // 0.3 nahi hota, balki 0.30000000000000004 jaisa kuch hota hai.
        // Isliye hum "net == 0" check karne ke bajaye "abs(net) < EPSILON"
        // check karte hain, taaki chhoti-moti rounding errors ki wajah se
        // koi settled person galti se creditor/debtor list me na chala jaye.
        const double EPSILON = 0.01; // floating point precision error handle
                                     // karne ke liye threshold

        for (auto &entry : netAmount)
        {
            if (entry.second > EPSILON)
            {
                // Positive net -> ye banda Creditor hai (paisa lena hai)
                creditors.push_back({entry.first, entry.second});
            }
            else if (entry.second < -EPSILON)
            {
                // debtor ka amount hum positive store karte hain (easy
                // comparison ke liye), isliye -1 se multiply kar diya
                // (jaise agar net = -400 hai, to hum store karenge +400,
                // taaki aage "min()" jaisi comparisons seedhe positive
                // numbers ke saath ho sake, negative signs ka jhanjhat na ho)
                debtors.push_back({entry.first, -entry.second});
            }
            // Agar net ~0 hai, to us user ka koi outstanding balance nahi
            // hai, use kisi list me daalne ki zaroorat nahi.
            // (yani ye person already "settled" hai, isko creditor ya
            // debtor kisi bhi list me nahi jodenge)
        }

        // ---------------------------------------------------------------
        // STEP 3: Dono list ko DESCENDING order me sort karna
        // ---------------------------------------------------------------
        // Sabse bade balance wale logo ko pehle settle karna better hai,
        // isse transactions ki total count kam hone ke chances badh jate
        // hain (though guaranteed optimal nahi hai).
        //
        // sort() function ek "comparator" (lambda function) leta hai jo
        // batata hai ki kaunsa element pehle aana chahiye. Yaha hum keh
        // rahe hain: "a.second > b.second" -> matlab bada amount pehle.
        // ---------------------------------------------------------------
        sort(creditors.begin(), creditors.end(),
             [](const pair<string, double> &a, const pair<string, double> &b)
             {
                 // Lambda function: do pairs (a aur b) compare karta hai
                 // aur true return karta hai agar 'a' ka amount 'b' se
                 // zyada hai — isse sort() list ko DESCENDING order me
                 // arrange karega (sabse bada creditor sabse pehle)
                 return a.second > b.second;
             });

        sort(debtors.begin(), debtors.end(),
             [](const pair<string, double> &a, const pair<string, double> &b)
             {
                 // Same logic debtors list ke liye — sabse bada debtor
                 // (jisko sabse zyada paisa dena hai) list me sabse pehle
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
        //
        // Ye ek classic "two-pointer" technique hai — dono sorted lists
        // par ek-ek pointer chalate hain, aur har step me kaam karke pointer
        // aage badhate hain. Isse humein poori list dobara-dobara scan nahi
        // karni padti, bas ek hi pass me kaam ho jata hai (O(n) is step ke
        // liye, overall O(n log n) sorting ki wajah se).
        // ---------------------------------------------------------------
        vector<Transaction> result; // final answer yaha store hoga
        int i = 0, j = 0;           // creditors aur debtors ke liye pointers

        // Loop tab tak chalega jab tak dono list me kuch bacha hai.
        // (Agar ek list khatam ho gayi, matlab baaki sab settled hai —
        // kyunki total creditors ka sum == total debtors ka sum hamesha
        // hota hai, ye ek mathematical guarantee hai is problem ki)
        while (i < (int)creditors.size() && j < (int)debtors.size())
        {
            string creditorId = creditors[i].first;      // current creditor ka naam
            string debtorId = debtors[j].first;          // current debtor ka naam
            double creditorAmount = creditors[i].second; // creditor ko kitna abhi lena baaki hai
            double debtorAmount = debtors[j].second;     // debtor ko kitna abhi dena baaki hai

            // -------------------------------------------------------
            // Dono me se jo minimum hai, wahi is round me settle hoga.
            //
            // Kyun minimum? Kyunki agar creditor ko sirf 200 lene hain
            // aur debtor ko 500 dene hain, to hum sirf 200 hi is
            // transaction me settle kar sakte hain (creditor ko usse
            // zyada nahi de sakte, uska poora balance clear ho jayega
            // aur debtor ka kuch baaki reh jayega jo agle creditor ko
            // milega).
            // -------------------------------------------------------
            double settleAmount = min(creditorAmount, debtorAmount);

            // Round karke 2 decimal places tak rakhte hain (currency ke liye)
            // (settleAmount * 100.0) -> paise ko "cents/paisa" unit me le
            // aate hain, round() se nearest integer banate hain, phir wapas
            // 100 se divide karke 2-decimal precision restore kar dete hain.
            // Isse floating point ki chhoti garbage decimals (jaise
            // 199.99999999) clean ho ke 200.00 ban jate hain.
            settleAmount = round(settleAmount * 100.0) / 100.0;

            if (settleAmount > 0)
            {
                // Debtor -> Creditor transaction record karo
                // (matlab: debtorId is amount ko creditorId ko pay karega)
                result.push_back(Transaction(debtorId, creditorId, settleAmount));
            }

            // Dono ke balances me se settle hua amount minus karo —
            // taaki agli iteration me pata rahe ki inka kitna balance
            // ab bhi baaki hai
            creditors[i].second -= settleAmount;
            debtors[j].second -= settleAmount;

            // Agar creditor ka balance clear (0 ke bahut paas) ho gaya,
            // to agle creditor par chale jao
            // (yaha bhi EPSILON use kiya hai, exact 0.0 check karne ke
            // bajaye, floating point safety ke liye)
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

            // Note: dono conditions "if" hain, "else if" nahi — kyunki
            // ek hi step me EK SAATH dono ka balance bhi 0 ho sakta hai
            // (jab creditorAmount == debtorAmount ho), aise case me dono
            // pointers ek saath aage badhne chahiye.
        }

        return result; // saari settled transactions ki list wapas bhej do
    }

    // -----------------------------------------------------------------------
    // Helper function: Net balances ko print karna (debug/verification ke liye)
    // -----------------------------------------------------------------------
    // Ye function sirf "dikhane" ke kaam aata hai — kisi ka net balance
    // (settlement se pehle) kya hai, ye clearly print kar deta hai. Isse
    // hum apne haath se calculation verify kar sakte hain ki algorithm
    // sahi kaam kar raha hai ya nahi.
    // -----------------------------------------------------------------------
    static void printNetBalances(map<string, map<string, double>> &groupBalances)
    {
        // Yaha bhi wahi net-calculation logic repeat ho rahi hai jo
        // simplifyDebts() ke STEP 1 me thi — bas is baar hum result ko
        // return nahi kar rahe, seedha print kar rahe hain.
        map<string, double> netAmount;

        // Sabhi users ko pehle 0 se initialize karo (koi miss na ho)
        for (auto &entry : groupBalances)
        {
            netAmount[entry.first] += 0;
            for (auto &inner : entry.second)
                netAmount[inner.first] += 0;
        }

        // Ab actual net nikalo
        for (auto &entry : groupBalances)
        {
            for (auto &inner : entry.second)
            {
                if (inner.second > 0)
                {
                    netAmount[entry.first] += inner.second;
                    netAmount[inner.first] -= inner.second;
                }
            }
        }

        // Ab sundar tarike se print karte hain
        cout << "\n--------------- Net Balances ---------------" << endl;
        for (auto &entry : netAmount)
        {
            cout << entry.first << " : ";

            if (entry.second > 0.01)
                // Positive net -> isko paisa milna hai
                cout << "+" << fixed << setprecision(2) << entry.second << "  (paisa milna hai)";
            else if (entry.second < -0.01)
                // Negative net -> isko paisa dena hai
                // (yaha "fixed" ki wajah se negative sign apne aap
                // number ke saath print ho jayega, jaise -230.00)
                cout << fixed << setprecision(2) << entry.second << "  (paisa dena hai)";
            else
                // Net lagbhag 0 -> is person ka koi outstanding balance nahi
                cout << "0.00  (settled)";

            cout << endl;
        }
        cout << "----------------------------------------------\n"
             << endl;
    }

    // -----------------------------------------------------------------------
    // Helper function: Transactions ko sundar tarike se print karna
    // -----------------------------------------------------------------------
    // Ye function final result (simplifyDebts() se aayi hui list) ko
    // ek readable, formatted tarike se terminal par print karta hai.
    // -----------------------------------------------------------------------
    static void printTransactions(const vector<Transaction> &transactions)
    {
        // Agar koi transaction hi nahi bachi, matlab sab pehle se settled
        // tha (ya sab cancel ho gaya cycles ki wajah se)
        if (transactions.empty())
        {
            cout << "Koi outstanding debt nahi hai. Sab settled hai! ✅" << endl;
            return; // function yahi khatam, aage kuch print nahi karna
        }

        cout << "\n=========== Simplified Transactions ===========" << endl;
        cout << "Total Transactions Required: " << transactions.size() << endl;
        cout << "-------------------------------------------------" << endl;

        // Har transaction ko loop karke, ek readable line me print karo:
        // "X ---> pays Rs amount ---> Y"
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
// Ab is baar 6 logo (A, B, C, D, E, F) ka bada aur tangled hua group liya
// hai, jisme kai-kai jagah do-tarfa entries (dono taraf se udhaar) bhi
// hain — bilkul real Splitwise group jaisa messy data.
// ---------------------------------------------------------------------------
int main()
{

    // -------------------------------------------------------------------
    // Raw Balances Setup — 6 log: A, B, C, D, E, F
    // -------------------------------------------------------------------
    // Format: balances[X][Y] = amount  =>  "Y owes X ka amount"
    //
    // Data structure: map<string, map<string,double>>
    //   outer key   -> creditor (jisko paisa milna hai)
    //   inner key   -> debtor   (jisko paisa dena hai)
    //   inner value -> amount
    //
    // Scenario:
    //   B owes A : 500
    //   A owes C : 300
    //   C owes D : 200
    //   D owes B : 100
    //   E owes A : 250
    //   B owes E : 150   (dono taraf udhaar — A/E group aur B/E group)
    //   F owes C : 400
    //   E owes F : 180
    //   D owes F : 90
    //   F owes B : 320
    //   C owes E : 60    (extra tangled entry)
    // -------------------------------------------------------------------
    map<string, map<string, double>> balances; // yahi hamara "raw data" hai,
                                               // jo real app me database se
                                               // aata (jaise Splitwise me)

    // Har line ka format: balances[Creditor][Debtor] = Amount
    balances["A"]["B"] = 500; // B owes A 500  -> A ko 500 milne hain B se
    balances["C"]["A"] = 300; // A owes C 300  -> C ko 300 milne hain A se
    balances["D"]["C"] = 200; // C owes D 200  -> D ko 200 milne hain C se
    balances["B"]["D"] = 100; // D owes B 100  -> B ko 100 milne hain D se
    balances["A"]["E"] = 250; // E owes A 250  -> A ko 250 milne hain E se
    balances["E"]["B"] = 150; // B owes E 150  -> E ko 150 milne hain B se
    balances["C"]["F"] = 400; // F owes C 400  -> C ko 400 milne hain F se
    balances["F"]["E"] = 180; // E owes F 180  -> F ko 180 milne hain E se
    balances["F"]["D"] = 90;  // D owes F 90   -> F ko 90 milne hain D se
    balances["B"]["F"] = 320; // F owes B 320  -> B ko 320 milne hain F se
    balances["E"]["C"] = 60;  // C owes E 60   -> E ko 60 milne hain C se

    // Ab raw (unsimplified) balances ko screen par print kar dete hain,
    // taaki user ko pehle "before" state dikhe, phir "after" (simplified)
    // state ke saath compare kar sake.
    cout << "=========== Original (Tangled) Balances — 6 Logo Ka Group ===========" << endl;
    cout << "B owes A : Rs 500" << endl;
    cout << "A owes C : Rs 300" << endl;
    cout << "C owes D : Rs 200" << endl;
    cout << "D owes B : Rs 100" << endl;
    cout << "E owes A : Rs 250" << endl;
    cout << "B owes E : Rs 150" << endl;
    cout << "F owes C : Rs 400" << endl;
    cout << "E owes F : Rs 180" << endl;
    cout << "D owes F : Rs 90" << endl;
    cout << "F owes B : Rs 320" << endl;
    cout << "C owes E : Rs 60" << endl;
    cout << "Total raw entries: 11" << endl; // yaha humne manually count
                                             // kiya hai kitni entries daali —
                                             // real app me ye balances.size()
                                             // ke through bhi nikal sakte hain

    // Net balances dikha dete hain taaki verify karna aasan ho
    // (ye function humein batayega ki settlement se PEHLE har ek ka
    // final net kya banta hai — creditor hai, debtor hai, ya settled hai)
    DebtSimplifier::printNetBalances(balances);

    // -------------------------------------------------------------------
    // Algorithm Call
    // -------------------------------------------------------------------
    // Ab hum apna main greedy algorithm call karte hain, jo poore
    // "tangled" balances ko lekar minimum transactions ki list return
    // karega.
    // -------------------------------------------------------------------
    vector<Transaction> simplified = DebtSimplifier::simplifyDebts(balances);

    // -------------------------------------------------------------------
    // Result Print
    // -------------------------------------------------------------------
    // Ab final simplified transactions ko readable format me print
    // karte hain, taaki dekh sake ki 11 raw entries se sirf kitni
    // transactions me kaam chal gaya.
    // -------------------------------------------------------------------
    DebtSimplifier::printTransactions(simplified);

    // -------------------------------------------------------------------
    // Manual Verification (samajhne ke liye):
    // -------------------------------------------------------------------
    // Yaha hum haath se (pen-paper style) calculate karke dikha rahe hain
    // ki har person ka net balance kaise nikla — isse confidence milta hai
    // ki upar wala algorithm sahi answer de raha hai.
    //
    //   A: +500 (B) -300 (C) +250 (E)               = +450
    //   B: -500 (A) +100 (D) -150 (E) +320 (F)      = -230
    //   C: +300 (A) -200 (D) +400 (F) -60 (E)       = +440
    //   D: +200 (C) -100 (B) -90 (F)                = +10
    //   E: -250 (A) +150 (B) -180 (F) +60 (C)       = -220
    //   F: -400 (C) +180 (E) +90 (D) -320 (B)       = -450
    //
    //   Check: 450 -230 +440 +10 -220 -450 = 0  ✅
    //   (Sabka total hamesha 0 hona chahiye — kyunki jitna kisi ko
    //   milna hai, utna hi kisi na kisi ko dena bhi hai. Ye ek zaroori
    //   "sanity check" hai poore system ke liye.)
    //
    //   Creditors: A(+450), C(+440), D(+10)
    //   Debtors  : F(-450), B(-230), E(-220)
    //
    // 11 raw entries -> algorithm inhe sirf 4 transactions me simplify kar
    // deta hai (run karke terminal me confirm kar sakte ho):
    //   F pays A: 450  |  B pays C: 230  |  E pays C: 210  |  E pays D: 10
    // -------------------------------------------------------------------

    return 0; // program successfully khatam — 0 return karna convention
              // hai C++ me "sab kuch theek raha" batane ke liye
}