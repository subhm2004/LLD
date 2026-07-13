// ============================================================================
//  utils/DebtSimplifier.h  —  Splitwise ka HEADLINE FEATURE
//  "Simplify Debts": kaun-kisko-kitna dena hai, use MINIMUM transactions me
//  badal do — bina kisi ke net hisaab ko chhede.
// ----------------------------------------------------------------------------
//  IDEA (ek line me):
//    Kisi ko is baat se farak nahi padta ki paisa KISKO dena hai — bas itna
//    matter karta hai ki uska NET (kul mila ke) kitna dena/lena bacha. To hum
//    saare pairwise karze ko bhool ke sirf har banda ka NET nikaalte hain, aur
//    phir naye sire se sabse kam transfers bana dete hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  CLASSIC EXAMPLE — kyun ye feature itna kaam ka hai:                     │
//  │                                                                          │
//  │   PEHLE (2 transactions):        Aditya ──100──> Rohit                   │
//  │                                  Rohit  ──100──> Manish                  │
//  │                                                                          │
//  │   Net nikaalo:  Aditya = -100  |  Rohit = 0  |  Manish = +100           │
//  │                 (Rohit ne 100 diya bhi, 100 liya bhi -> uska hisaab      │
//  │                  barabar! Wo beech me se HAT sakta hai.)                 │
//  │                                                                          │
//  │   BAAD ME (1 transaction):       Aditya ──100──> Manish                  │
//  │                                                                          │
//  │   Rohit ko ab kuch karna hi nahi. 2 transactions -> 1. Sabka net same.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ALGORITHM — 3 saaf steps:
//
//    STEP 1: NET nikaalo
//            Har banda ka ek hi number: (mera kul lena) − (mera kul dena)
//              net > 0  ->  CREDITOR  (lene wala — logon ne isko dena hai)
//              net < 0  ->  DEBTOR    (dene wala — isne logon ko dena hai)
//              net = 0  ->  free!     (hisaab barabar, ise chhod do)
//            NOTE: sabhi nets ka total hamesha 0 hota hai — jitna paisa kisi ka
//                  lena hai, utna hi kisi ka dena hai. Ye ek accha sanity check.
//
//    STEP 2: Dono lists ko BADE SE CHHOTA sort karo
//
//    STEP 3: GREEDY two-pointer matching
//            Sabse bade creditor ko sabse bade debtor se milao. Jitna ho sake
//            utna settle karo (min of dono). Jiska amount khatam ho gaya, use
//            list se aage badha do. Repeat.
//
//  Kyun GREEDY (sabse bada vs sabse bada)?
//    Kyunki har transaction me hum kam se kam EK banda ko poori tarah "khatam"
//    (settle) kar dete hain — uska net 0 ho jaata hai aur wo hamesha ke liye
//    list se nikal jaata hai. Toh n logon ke liye zyada se zyada n−1
//    transactions lagenge. Bina simplify kiye ye N×N (har pair ka apna) tak
//    ja sakta tha. Bahut bada fark.
//
//  Sach bolein to — kya ye HAMESHA absolute minimum deta hai?
//    Nahi. "Minimum number of transactions" wala problem NP-hard hai (subset-sum
//    se reduce hota hai — agar kisi creditor ka amount kuch debtors ke sum se
//    exactly match kar jaaye to us subset ko pehle match karna better hota).
//    Ye greedy ek HEURISTIC hai: hamesha VALID answer deta hai, hamesha n−1 se
//    kam-ya-barabar, aur practically almost hamesha optimal ke aas-paas.
//    Interview me exactly yahi bolna hai — "greedy hai, n−1 bound guarantee hai,
//    theoretical minimum NP-hard hai". Ye jaanna hi tumhe alag dikhata hai.
// ============================================================================
#ifndef SPLITWISE_LLD_UTILS_DEBTSIMPLIFIER_H
#define SPLITWISE_LLD_UTILS_DEBTSIMPLIFIER_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {

class DebtSimplifier {
public:
    // Pure function hai: balance matrix andar, NAYA (simplified) matrix bahar.
    // Koi state nahi, koi side-effect nahi -> isi liye `static` hai aur test
    // karna bhi bahut aasan (input do, output check karo, bas).
    //
    // Dhyaan do: parameter BY VALUE hai (`map<...> groupBalances`, `&` nahi).
    // Yaani ek copy banti hai. Yahan wo theek hai — hum original ko chhedna
    // nahi chahte, aur caller (Group::simplifyGroupDebts) return value ko
    // apne matrix pe assign kar deta hai. `const &` lena thoda efficient hota,
    // par is size pe farak nahi padta.
    static map<string, map<string, double>> simplifyDebts(map<string, map<string, double>> groupBalances) {

        // ====================================================================
        //  STEP 1 — har banda ka NET nikaalo
        // ====================================================================

        // Pehle sabko 0 se initialize karo. Ye isliye zaroori hai taaki wo log
        // bhi map me aayein jinka koi lena-dena hai hi nahi (net = 0) — warna
        // wo silently gayab ho jaate.
        map<string, double> netAmounts;
        for (const auto &ub : groupBalances) netAmounts[ub.first] = 0;

        // Ab matrix ghumo aur nets bharo.
        for (const auto &ub : groupBalances) {
            string creditorId = ub.first;             // matrix ki row = ek banda
            for (const auto &b : ub.second) {         // uski row ke andar ke entries
                string debtorId = b.first;
                double amount = b.second;

                // ⚠ SIRF POSITIVE entries dekho — ye chhota sa `if` bahut
                // important hai. Yaad karo: matrix MIRRORED hai —
                //     groupBalances[A][B] = +100   (B ko A ko 100 dene hain)
                //     groupBalances[B][A] = -100   (wahi baat, ulti taraf se)
                // Yaani har karza matrix me DO BAAR likha hai. Agar hum dono
                // padh lete, to har amount DOUBLE count ho jaata. Sirf positive
                // side padhne se har karza THEEK EK BAAR ginta hai.
                if (amount > 0) {
                    netAmounts[creditorId] += amount;  // iska lena badha
                    netAmounts[debtorId] -= amount;    // uska dena badha
                }
            }
        }

        // ====================================================================
        //  STEP 2 — do camps me baanto, aur bade se chhota sort karo
        // ====================================================================
        vector<pair<string, double>> creditors, debtors;
        for (const auto &net : netAmounts) {
            // 0.01 ka epsilon: floating-point ka kachra (0.0000001 type) ko
            // "settled" maan ke chhod do. Aur exact 0 wale bhi apne aap yahan
            // chhut jaate hain — dono list me nahi jaate, kyunki unhe kuch
            // karna hi nahi hai (upar wala Rohit yaad hai? Wo yahan drop hota hai).
            if (net.second > 0.01) creditors.push_back({net.first, net.second});

            // Debtors ka amount POSITIVE bana ke store karte hain (`-net.second`).
            // Kyun? Taaki aage min() / subtract ka logic dono lists pe bilkul
            // same chale — bar bar sign ka dhyaan na rakhna pade. Chhoti trick,
            // par loop ka code kaafi saaf ho jaata hai.
            else if (net.second < -0.01) debtors.push_back({net.first, -net.second});
        }

        // Descending sort (bada pehle) — greedy ka dil yahi hai. Lambda me
        // `a.second > b.second` -> bade amount wala pehle aayega.
        sort(creditors.begin(), creditors.end(), [](const auto &a, const auto &b){ return a.second > b.second; });
        sort(debtors.begin(), debtors.end(), [](const auto &a, const auto &b){ return a.second > b.second; });

        // ====================================================================
        //  STEP 3 — GREEDY two-pointer matching
        // ====================================================================

        // Naya khaali matrix. Purana poora BHOOL jaate hain — ab sirf nets se
        // fresh transfers banayenge. Har member ki khaali row pehle se daal do
        // taaki jo log settled hain (net = 0) wo bhi matrix me dikhein (bas
        // khaali row ke saath) — showGroupBalances unke liye "No outstanding
        // balances" print kar sake.
        map<string, map<string, double>> simplified;
        for (const auto &ub : groupBalances) simplified[ub.first] = {};

        size_t i = 0, j = 0;  // i -> creditors me, j -> debtors me
        while (i < creditors.size() && j < debtors.size()) {
            string creditorId = creditors[i].first, debtorId = debtors[j].first;

            // Jitna ho sake utna settle karo — dono me se JO CHHOTA hai.
            // Kyun min()? Kyunki:
            //   - creditor se zyada nahi de sakte (usko utna lena hi nahi)
            //   - debtor se zyada nahi le sakte (uske paas utna dena hi nahi)
            // Aur min() lene se DONO me se KAM SE KAM EK ka amount 0 ho jaata
            // hai -> wo list se nikal jaata hai -> loop guaranteed aage badhta
            // hai (infinite loop nahi ho sakta).
            double settle = min(creditors[i].second, debtors[j].second);

            // Naya transfer likho — dono taraf (mirrored), same convention:
            //   simplified[creditor][debtor] = +settle  (debtor ko dena hai)
            //   simplified[debtor][creditor] = -settle  (debtor ke upar karza)
            simplified[creditorId][debtorId] = settle;
            simplified[debtorId][creditorId] = -settle;

            // Dono ka bacha hua amount kam karo
            creditors[i].second -= settle;
            debtors[j].second -= settle;

            // Jiska khatam ho gaya (1 paise se kam bacha), use chhod ke aage badho.
            // Note: dono ek saath bhi khatam ho sakte hain (jab amounts barabar
            // the) — tab dono pointer aage badhenge. Isi liye ye do ALAG `if`
            // hain, `if-else` nahi. Ye ek classic bug ki jagah hai.
            if (creditors[i].second < 0.01) i++;
            if (debtors[j].second < 0.01) j++;
        }
        // Loop khatam = ek list poori ho gayi. Aur kyunki saare nets ka sum 0
        // hota hai, ek list khatam hone ka matlab DONO khatam ho gayi. Kuch bhi
        // "bacha hua" nahi rehta. (Agar rehta, to hisaab me kahin gadbad hai.)

        return simplified;
    }
};

}

#endif
