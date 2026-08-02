// ============================================================================
//  pds_common.h  —  Saare probabilistic structures ka COMMON base
// ----------------------------------------------------------------------------
//  In saari structures ki jaan ek hi cheez me hai: ek ACHHA HASH FUNCTION.
//  Agar hash bikhra hua na ho, to Bloom filter ka false-positive rate theory se
//  bahut zyada aayega, Count-Min ka error phat jaayega, aur HyperLogLog ka
//  estimate bekaar ho jaayega.
//
//  ⭐ In sabki theory ek hi baat maan kar chalti hai: "hash output UNIFORMLY
//     RANDOM hai". Isliye pehle hash theek karo, tabhi formula match karega.
//     Ye demos wahi verify karte hain — aur match karte hain, matlab hash theek hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ 64-BIT HASH KYUN (32-bit nahi)                                        │
//  │                                                                          │
//  │  HyperLogLog me hum crore-arab unique items gin te hain. 32-bit hash me   │
//  │  sirf ~4.3 arab alag values ho sakti hain — to bade counts pe COLLISIONS  │
//  │  shuru ho jaate hain aur estimate girne lagta hai (isi liye original HLL  │
//  │  paper me "large range correction" lagani padti thi).                     │
//  │                                                                          │
//  │  64-bit hash me wo problem practically hoti hi nahi, aur correction ki    │
//  │  zaroorat khatam. Isliye yahan sab jagah 64-bit use kiya hai.             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 FNV-1a se bits mila kar phir `splitmix64` finalizer lagaya hai — wahi
//     kaam jo pehle wale folders me `fmix32` kar raha tha, bas 64-bit me.
// ============================================================================
#ifndef PROBABILISTIC_DATA_STRUCTURES_PDS_COMMON_H
#define PROBABILISTIC_DATA_STRUCTURES_PDS_COMMON_H

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace pds
{

// ---- splitmix64 finalizer — bits ko achhe se "hilata" hai -----------------
inline uint64_t mix64(uint64_t x)
{
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

// ---- FNV-1a 64-bit + finalizer -------------------------------------------
// `seed` isliye taaki ek hi string ke kai ALAG-ALAG hash mil sakein
// (Count-Min Sketch ko har row ke liye alag hash chahiye).
inline uint64_t hash64(const string &key, uint64_t seed = 0)
{
    uint64_t h = 1469598103934665603ULL ^ seed; // FNV offset basis
    for (unsigned char c : key)
    {
        h ^= c;
        h *= 1099511628211ULL; // FNV prime
    }
    return mix64(h);
}

// ============================================================================
//  ⭐ KIRSCH-MITZENMACHER TRICK — k hashes, par sirf 2 hash calculate karo
// ----------------------------------------------------------------------------
//  Bloom filter ko `k` independent hash chahiye (aksar k=7 ya 10). Har item pe
//  10 alag hash functions chalana MEHNGA hai.
//
//  1998 me sabit hua ki sirf DO hash se kaam chal jaata hai:
//
//        h_i(x) = h1(x) + i * h2(x)      (i = 0, 1, 2, ... k-1)
//
//  Isse false-positive rate practically utna hi rehta hai jitna k asli
//  independent hashes se hota. Yaani 10 hash ka kaam 2 me. ⭐
//
//  ⚠ `h2` ko VISHAM (odd) banaya jaata hai taaki wo modulo ke saath achhe se
//     ghoome (warna kuch buckets kabhi hit hi nahi honge).
// ============================================================================
inline void doubleHash(const string &key, uint64_t &h1, uint64_t &h2)
{
    h1 = hash64(key, 0);
    h2 = hash64(key, 0x9e3779b9ULL) | 1ULL; // `| 1` = hamesha odd
}

// ---- Test data banane ke liye --------------------------------------------
inline vector<string> makeKeys(const string &prefix, int count)
{
    vector<string> keys;
    keys.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        keys.push_back(prefix + to_string(i));
    }
    return keys;
}

// ---- Bytes ko padhne-layak roop me ---------------------------------------
inline string humanBytes(double bytes)
{
    const char *units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    while (bytes >= 1024.0 && unit < 3)
    {
        bytes /= 1024.0;
        ++unit;
    }
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f %s", bytes, units[unit]);
    return string(buffer);
}

// ---- "theory vs measured" ko ek line me dikhane ke liye -------------------
//  Yahi is poore folder ka dil hai: har structure ka ek FORMULA hai, aur hum
//  har baar naap kar dekhte hain ki formula sach bolta hai ya nahi.
inline void printTheoryVsMeasured(const string &label, double theory, double measured,
                                  const string &unit = "%")
{
    double error = (theory > 0.0) ? (100.0 * fabs(measured - theory) / theory) : 0.0;
    cout << "    " << left << setw(24) << label << right << " theory " << setw(8) << fixed
         << setprecision(4) << theory << unit << "  |  measured " << setw(8) << measured << unit
         << "  |  farak " << setprecision(1) << error << "%\n"
         << left;
    cout << right;
}

} // namespace pds

#endif // PROBABILISTIC_DATA_STRUCTURES_PDS_COMMON_H
