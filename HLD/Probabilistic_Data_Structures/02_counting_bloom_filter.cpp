// ============================================================================
//  #2 — 02_counting_bloom_filter.cpp
//  COUNTING BLOOM FILTER — delete karne ki keemat
// ----------------------------------------------------------------------------
//  Bloom filter ki sabse badi kami: aap kuch DELETE nahi kar sakte.
//
//  ⭐ Kyun nahi? Sochiye "apple" ne bits 3,17,42 set ki. "mango" ne 17,58,91.
//     Ab "apple" delete karne ke liye 3,17,42 ko 0 kar do — par bit 17 to
//     "mango" ki bhi thi! Ab "mango" ka check FALSE NEGATIVE dega.
//     Aur false negative Bloom filter ki EK guarantee thi, wahi toot gayi. 💥
//
//  ⭐ ILAAJ: bit ki jagah ek CHHOTA COUNTER rakho (aksar 4 bits).
//        add    -> counter++
//        remove -> counter--
//        check  -> saare counters > 0 hain?
//
//     Ab "apple" hatane pe bit 17 ka counter 2 se 1 hoga, 0 nahi.
//     "mango" abhi bhi safe hai. ✅
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ KEEMAT: 4x MEMORY                                                     │
//  │                                                                          │
//  │  1 bit ki jagah 4 bits. Yaani wahi false-positive rate paane ke liye      │
//  │  CHAAR GUNA memory. Delete karne ki ye seedhi keemat hai.                 │
//  │                                                                          │
//  │  Isi liye jahan delete ki zaroorat na ho, wahan normal Bloom hi lagao.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ COUNTER OVERFLOW — wo baat jo log bhool jaate hain                   │
//  │                                                                          │
//  │  4 bits me sirf 0-15 aata hai. Agar koi counter 15 pe pahunch gaya aur    │
//  │  ek aur item usko hit kare, to wo aage nahi badh sakta.                   │
//  │                                                                          │
//  │  Tab hum use 15 pe HI ROK dete hain (saturate). Par ab wo counter kabhi   │
//  │  theek se ghata nahi ja sakta — kyunki uska asli count 15 se zyada tha.   │
//  │  Isi liye us bucket ko HAMESHA ke liye 15 pe chhod dete hain (usko kabhi  │
//  │  0 nahi hone dete), warna false negative ka khatra aa jaata.              │
//  │                                                                          │
//  │  ⭐ Ye demo overflow ko naap ke dikhayega — aur wo tab hota hai jab       │
//  │     filter apni capacity se zyada bhar diya jaaye.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ✅ Faayde  : delete kar sakte ho, baaki sab Bloom jaisa
//  ❌ Nuksan  : 4x memory, counter overflow ka dhyan rakhna padta
//  📌 Kab use : jab set BADALTA rehta ho (cache eviction, active session list,
//               CDN pe kaunsa content abhi maujood hai)
//
//  Compile: g++ -std=c++17 -Wall -Wextra 02_counting_bloom_filter.cpp -o counting_bloom_demo
// ============================================================================
#include <bits/stdc++.h>

#include "structures/counting_bloom_filter.h"

using namespace std;
using namespace pds;

// ---- Comparison ke liye: simple Bloom (bina counting ke) -----------------
class PlainBloom
{
public:
    PlainBloom(size_t bits, int hashCount) : bits_(bits), k_(hashCount), data_((bits + 63) / 64, 0) {}
    void add(const string &key)
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < k_; ++i)
        {
            uint64_t idx = (h1 + static_cast<uint64_t>(i) * h2) % bits_;
            data_[idx >> 6] |= (1ULL << (idx & 63));
        }
    }
    // ⚠ Ye GALAT tareeka hai — sirf dikhane ke liye ki kyun galat hai
    void unsafeRemove(const string &key)
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < k_; ++i)
        {
            uint64_t idx = (h1 + static_cast<uint64_t>(i) * h2) % bits_;
            data_[idx >> 6] &= ~(1ULL << (idx & 63));
        }
    }
    bool mightContain(const string &key) const
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < k_; ++i)
        {
            uint64_t idx = (h1 + static_cast<uint64_t>(i) * h2) % bits_;
            if (!((data_[idx >> 6] >> (idx & 63)) & 1ULL))
            {
                return false;
            }
        }
        return true;
    }

private:
    size_t bits_;
    int k_;
    vector<uint64_t> data_;
};

int main()
{
    cout << "=========================================================\n";
    cout << " #2 : COUNTING BLOOM FILTER\n";
    cout << "=========================================================\n\n";

    // ---- Part A: ⭐ pehle dikhao ki normal Bloom me delete KYUN nahi ----
    cout << "[A] ⭐ Pehle samjho normal Bloom me delete kyun MANA hai:\n\n";
    {
        PlainBloom bloom(10000, 7);
        vector<string> keys = makeKeys("item:", 2000);
        for (const string &key : keys)
        {
            bloom.add(key);
        }

        // Ab aadhe items "delete" kar dete hain (bits 0 karke)
        for (int i = 0; i < 1000; ++i)
        {
            bloom.unsafeRemove("item:" + to_string(i));
        }

        // Jo items ABHI BHI andar hone chahiye (1000-1999), unhe check karo
        int falseNegatives = 0;
        for (int i = 1000; i < 2000; ++i)
        {
            if (!bloom.mightContain("item:" + to_string(i)))
            {
                ++falseNegatives;
            }
        }

        cout << "    2000 items add kiye, phir pehle 1000 ko 'bits 0 karke' hataya.\n";
        cout << "    Ab bache hue 1000 items check kiye:\n\n";
        cout << "    FALSE NEGATIVES: " << falseNegatives << " out of 1000  💥\n";
        cout << "\n    ⭐ Yaani " << (100.0 * falseNegatives / 1000)
             << "% items 'gayab' ho gaye jabki wo andar hain!\n";
        cout << "       Bloom filter ki EKLAUTI guarantee (false negative kabhi nahi)\n";
        cout << "       toot gayi. Isi liye normal Bloom me delete BILKUL MANA hai.\n\n";
    }

    // ---- Part B: counting bloom sahi se delete karta hai ----------------
    cout << "[B] Ab wahi test COUNTING Bloom filter pe:\n\n";
    {
        CountingBloomFilter filter = CountingBloomFilter::forCapacity(2000, 0.01);
        for (int i = 0; i < 2000; ++i)
        {
            filter.add("item:" + to_string(i));
        }
        for (int i = 0; i < 1000; ++i)
        {
            filter.remove("item:" + to_string(i));
        }

        int falseNegatives = 0;
        for (int i = 1000; i < 2000; ++i)
        {
            if (!filter.mightContain("item:" + to_string(i)))
            {
                ++falseNegatives;
            }
        }
        int stillPresent = 0;
        for (int i = 0; i < 1000; ++i)
        {
            if (filter.mightContain("item:" + to_string(i)))
            {
                ++stillPresent;
            }
        }

        cout << "    Bache hue items ke false negatives : " << falseNegatives
             << " out of 1000  <-- 0 hona chahiye ✅\n";
        cout << "    Hataye gaye items jo abhi bhi 'dikh' rahe : " << stillPresent
             << " out of 1000\n";
        cout << "      (ye normal false positives hain — delete ki galti nahi)\n";
        cout << "\n    ⭐ Delete ke baad bhi guarantee bachi rahi. Yahi counters ka faayda.\n\n";
    }

    // ---- Part C: add/remove cycle — state saaf rehta hai? --------------
    cout << "[C] Bar-bar add/remove karne pe state kharab to nahi hoti?\n\n";
    {
        CountingBloomFilter filter = CountingBloomFilter::forCapacity(10000, 0.01);

        // 50 round: har round me 1000 add, phir wahi 1000 remove
        for (int round = 0; round < 50; ++round)
        {
            for (int i = 0; i < 1000; ++i)
            {
                filter.add("temp:" + to_string(round) + ":" + to_string(i));
            }
            for (int i = 0; i < 1000; ++i)
            {
                filter.remove("temp:" + to_string(round) + ":" + to_string(i));
            }
        }

        // Ab filter lagbhag khaali hona chahiye
        int ghosts = 0;
        for (int i = 0; i < 10000; ++i)
        {
            if (filter.mightContain("check:" + to_string(i)))
            {
                ++ghosts;
            }
        }
        cout << "    50 round x (1000 add + 1000 remove) = 50,000 add aur 50,000 remove\n";
        cout << "    Ab 10,000 naye items check kiye -> " << ghosts << " ne 'haan' kaha\n";
        cout << "    Overflow hue counters: " << filter.overflowCount() << "\n";
        cout << "\n    ⭐ Filter wapas lagbhag khaali ho gaya — counters theek se\n";
        cout << "       badhe aur ghate. Ye 'cache eviction' jaise use case ke liye\n";
        cout << "       zaroori hai, jahan set lagatar badalta rehta hai.\n\n";
    }

    // ---- Part D: ⚠ overflow — jab capacity se zyada bhar do -------------
    cout << "[D] ⚠ Ab galat use karke dikhate hain — capacity se zyada bharo:\n\n";
    {
        cout << "    Filter 10,000 items ke liye banaya, par usme zyada thusenge:\n\n";
        cout << "    items daale | overflow hue counters | ye kyun bura hai\n";
        cout << "    ------------+-----------------------+------------------\n";

        for (int items : {10000, 50000, 100000, 200000})
        {
            CountingBloomFilter filter = CountingBloomFilter::forCapacity(10000, 0.01);
            for (int i = 0; i < items; ++i)
            {
                filter.add("x:" + to_string(i));
            }
            cout << "    " << setw(11) << items << " | " << setw(21) << filter.overflowCount()
                 << " | " << (filter.overflowCount() == 0 ? "theek hai" : "in slots ko ab ghata nahi sakte")
                 << "\n";
        }
        cout << "\n    ⭐ Capacity ke andar raho to overflow hota hi nahi (pehli row).\n";
        cout << "       Zyada bharoge to counters 15 pe atak jaate hain, aur wo slots\n";
        cout << "       hamesha ke liye 'bhare hue' reh jaate hain — filter dheere-dheere\n";
        cout << "       bekaar hone lagta hai. Isliye capacity ka andaaza sahi lagao.\n\n";
    }

    // ---- Part E: memory ki keemat ----------------------------------------
    cout << "[E] Delete karne ki KEEMAT — memory:\n\n";
    {
        cout << "    items      | plain Bloom | counting Bloom | kitna zyada\n";
        cout << "    -----------+-------------+----------------+------------\n";
        for (long long n : {100000LL, 1000000LL, 10000000LL})
        {
            double m = -(static_cast<double>(n) * log(0.01)) / (M_LN2 * M_LN2);
            double plainBytes = ceil(m / 8.0);
            double countingBytes = ceil(m / 2.0); // 4 bits per slot

            cout << "    " << setw(10) << n << " | " << setw(11) << humanBytes(plainBytes) << " | "
                 << setw(14) << humanBytes(countingBytes) << " | " << setw(6) << fixed
                 << setprecision(0) << (countingBytes / plainBytes) << "x\n";
        }
        cout << "\n    ⭐ Theek 4x — kyunki 1 bit ki jagah 4 bits. Delete ka feature\n";
        cout << "       muft nahi milta. Zaroorat na ho to plain Bloom hi behtar hai.\n";
    }

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA:\n";
    cout << "  ✅ Delete kar sakte ho, aur false-negative guarantee bachi rehti hai.\n";
    cout << "  ✅ Bar-bar add/remove karne pe bhi state saaf rehti hai.\n";
    cout << "  ❌ 4x memory (1 bit -> 4 bits).\n";
    cout << "  ❌ Counter overflow: capacity se zyada bharoge to slots 15 pe atak\n";
    cout << "     jaate hain aur filter dheere-dheere kharab hota jaata hai.\n";
    cout << "  ⚠ Normal Bloom me bits 0 karke delete karna = false negatives\n";
    cout << "     (part A me naap ke dekha). Kabhi mat karna.\n";
    cout << "\n  Ab tak sirf 'hai ya nahi' tha. GINTI chahiye? -> COUNT-MIN SKETCH (file 03)\n";
    cout << "---------------------------------------------------------\n";
    return 0;
}
