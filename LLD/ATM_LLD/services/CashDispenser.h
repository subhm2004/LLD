// ============================================================================
//  services/CashDispenser.h  —  ATM cash vault + notes nikaalne ka algorithm
//                               (Requirement #4 "sufficient notes", #5 "dispense
//                                by denomination")
// ----------------------------------------------------------------------------
//  Iska kaam: ATM ke andar kaunse denomination ke kitne notes hain, ye track
//  karna, aur jab paisa nikalna ho to sahi notes chun ke dena.
//
//  ============================================================================
//   ⭐⭐ YAHAN EK BUG THA — GREEDY se BACKTRACKING pe shift kiya
//  ----------------------------------------------------------------------------
//   PURANA CODE GREEDY tha: "hamesha sabse bada note pehle, jitna zyada ho sake".
//
//   🐛 GREEDY KYUN GALAT HAI (limited notes ke saath):
//
//      Maano vault me:  500 x1,  200 x3,  100 x0
//      User maange:     600
//
//      GREEDY sochega: "600 me se ek 500 note nikaal do" -> remaining 100.
//                      Ab 100 ka note chahiye -> vault me 100 hai hi nahi (x0).
//                      -> "notes available nahi" bol ke MANA kar dega. ❌
//
//      Par asli me: 200 + 200 + 200 = 600 aaram se ban sakta tha! Greedy ne
//      pehla 500 uthaake khud ko phasa liya. Ye greedy ki classic galti hai —
//      wo "abhi ka best" leta hai, "poore ka best" nahi dekhta.
//
//   ✅ FIX — BACKTRACKING: har denomination pe "kitne notes lein" ke saare
//      options try karo (zyada se kam), aur agar ek choice se aage kaam na bane
//      to PEECHE aake kam notes wali choice try karo. Isse agar koi bhi valid
//      combination possible hai to wo GUARANTEED mil jaayegi.
//
//      Upar wale example me: 500 ka 1 note try -> fail -> "500 ka 0 note" try ->
//      phir 200 ke 3 notes -> 600 ban gaya. ✅
//
//   📌 Ye greedy-vs-backtracking wala farak coin/note-change problems me classic
//      interview topic hai. High-to-low order rakha hai taaki (jahan possible ho)
//      kam-se-kam notes wala hal pehle mile — user ko mote notes milte hain.
//  ============================================================================
#ifndef ATM_LLD_SERVICES_CASHDISPENSER_H
#define ATM_LLD_SERVICES_CASHDISPENSER_H

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

using namespace std;

namespace atm_lld {

class CashDispenser {
public:
    // Vault initialize — default notes inventory.
    CashDispenser() {
        notes_[500] = 20; // 500 x20 = 10,000 INR
        notes_[200] = 30; // 200 x30 = 6,000 INR
        notes_[100] = 50; // 100 x50 = 5,000 INR
    }

    // ---- Kya `amount` diye gaye notes se ban sakta hai? (paisa nikale bina) --
    bool canDispense(int amount) const {
        // Quick reject: negative ya sabse chhote note (100) ka multiple nahi.
        // (100 se kam ka change ye ATM de hi nahi sakta.)
        if (amount <= 0 || amount % 100 != 0) {
            return false;
        }
        // Denominations descending order me (map greater<int> se already sorted).
        vector<pair<int, int>> denoms(notes_.begin(), notes_.end());
        map<int, int> plan;
        // Bas check karna hai (vault ko haath nahi lagana) — isi liye COPY nahi,
        // computePlan sirf padhta hai aur ek throwaway plan bharta hai.
        return computePlan(denoms, 0, amount, plan);
    }

    // ---- Asli me notes nikaalo: plan banao, vault se ghatao, plan lauta do ---
    map<int, int> dispense(int amount) {
        vector<pair<int, int>> denoms(notes_.begin(), notes_.end());
        map<int, int> plan;
        if (!computePlan(denoms, 0, amount, plan)) {
            // Koi valid combination nahi bani (ab backtracking ke baad ye SIRF
            // tab hoga jab sach me notes kam hain — greedy wali jhooti "na" nahi).
            throw runtime_error("ATM ke paas is amount ke liye sahi notes ka combination nahi hai.");
        }
        // Plan mil gaya -> vault se wo notes ghata do (asli deduction).
        for (const auto &entry : plan) {
            notes_[entry.first] -= entry.second;
        }
        return plan; // {100: x, 200: y, 500: z} — kitne kaunse note diye
    }

private:
    // ========================================================================
    //  BACKTRACKING core — kya denoms[idx..] se `remaining` ban sakta hai?
    // ========================================================================
    //  Har denomination pe: "kitne notes use karein" ke options ZYADA se KAM
    //  try karo. Ek option se aage poora amount ban gaya -> true (plan bhara hua).
    //  Nahi bana -> KAM notes wala option try karo (backtrack). Sab fail -> false.
    //
    //  `plan` reference se bharta jaata hai; fail wale raaste apni entry saaf
    //  kar dete hain taaki plan me kachra na rahe.
    static bool computePlan(const vector<pair<int, int>> &denoms, size_t idx, int remaining,
                            map<int, int> &plan) {
        if (remaining == 0) {
            return true; // poora amount ban gaya — plan taiyaar
        }
        if (idx == denoms.size()) {
            return false; // notes khatam par amount bacha — ye raasta fail
        }

        const int denom = denoms[idx].first;      // abhi kaunsa note (500/200/100)
        const int available = denoms[idx].second; // itne available hain
        // Is note ke max kitne use ho sakte: jitne chahiye ya jitne hain (jo kam ho)
        const int maxUse = min(remaining / denom, available);

        // ZYADA se KAM try karo (mote notes pehle — kam-notes wala hal aage aata)
        for (int use = maxUse; use >= 0; --use) {
            if (use > 0) {
                plan[denom] = use;
            } else {
                plan.erase(denom); // 0 use -> is note ki entry hi mat rakho
            }
            // Bache hue amount ko agle (chhote) denominations se banane ki koshish
            if (computePlan(denoms, idx + 1, remaining - use * denom, plan)) {
                return true; // aage kaam ban gaya!
            }
            // Nahi bana -> loop kam `use` try karega (BACKTRACK)
        }
        plan.erase(denom); // is denom se kuch nahi bana — apni entry saaf karo
        return false;
    }

    // Notes inventory: denomination -> count. `greater<int>` se descending order
    // (500 pehle, phir 200, phir 100) — mote note pehle try karne ke liye handy.
    map<int, int, greater<int>> notes_;
};

} // namespace atm_lld

#endif // ATM_LLD_SERVICES_CASHDISPENSER_H
