// ============================================================================
//  hash_util.h  —  Saare demos ka COMMON hash function
// ----------------------------------------------------------------------------
//  Consistent hashing ka poora khel ek hi cheez pe tika hai: ek achha
//  **hash function**. Usse do cheezein chahiye:
//
//    1. DETERMINISTIC — same input -> hamesha same output (har machine pe,
//       har run me). Warna ek server key ko "A" pe dhundhega, doosra "B" pe.
//    2. UNIFORM (avalanche) — input me 1 bit badle to output ke ~aadhe bits
//       badal jaayein. Isse "user_1", "user_2", "user_3" ring pe door-door
//       bikhrenge, ek jagah cluster nahi honge.
//
//  ⚠ `std::hash<string>` yahan JAAN-BUJH KE nahi use kiya:
//     - Standard guarantee nahi deta ki value process/library ke beech same
//       rahegi (libstdc++ vs libc++ me alag aa sakti hai).
//     - Distributed system me har node ko BILKUL same number chahiye.
//     Isliye apna khud ka, fixed, portable hash likha hai.
//
//  ⭐ FNV-1a (32-bit) + murmur3 ka `fmix32` finalizer:
//     FNV-1a khud simple aur tez hai, par chhoti-chhoti milti-julti strings
//     ("key_1" vs "key_2") pe uske lower bits kam bikharte hain. Isliye end me
//     fmix32 (murmur3 ka avalanche step) laga diya — ab har bit theek se mix
//     ho jaata hai. Production me log MurmurHash3 / xxHash / MD5 use karte hain
//     (Cassandra Murmur3Partitioner use karta, Memcached clients MD5/CRC32).
//
//  📌 CRYPTO ka kaam nahi hai ye — security nahi, sirf DISTRIBUTION chahiye.
//     Isliye fast non-crypto hash hi sahi choice hai (MD5/SHA slow hain).
// ============================================================================
#ifndef CONSISTENT_HASHING_HASH_UTIL_H
#define CONSISTENT_HASHING_HASH_UTIL_H

#include <iostream>
#include <cmath>
#include <cstdint>
#include <string>

using namespace std;

namespace consistent_hashing {

// ---- murmur3 ka final avalanche mix — bits ko achhe se "hilata" hai --------
// Ye sirf bits ko phaila raha hai, koi nayi information add nahi kar raha.
inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// ---- FNV-1a 32-bit + finalizer --------------------------------------------
// Return: 0 se 2^32-1 ke beech ek number = RING pe position.
// Yahi function keys ke liye bhi chalta hai aur nodes ke liye bhi — dono ek hi
// ring space me map hone chahiye, tabhi "clockwise nearest node" ka matlab banta.
inline uint32_t hashKey(const string &key) {
    uint32_t h = 2166136261u;          // FNV offset basis
    for (unsigned char c : key) {
        h ^= c;                        // pehle XOR (isliye "1a" variant)
        h *= 16777619u;                // phir FNV prime se multiply
    }
    return fmix32(h);                  // aur ant me achha-khasa avalanche
}

// ---- Ring position ko chhota, padhne-layak roop me dikhane ke liye ---------
// 2^32 ki poori range console pe padhna mushkil hai, isliye demos me hum
// position ko 0-99 (percent) me dikha dete hain — "ring ke kis hisse me hai".
inline double ringPercent(uint32_t position) {
    return (100.0 * position) / 4294967296.0; // 2^32
}

} // namespace consistent_hashing


// ============================================================================
//  ⭐ VERIFICATION — demo ko sirf REPORT nahi, VERIFY bhi karna chahiye
// ----------------------------------------------------------------------------
//  Pehle ye demos numbers print karte the aur hamesha `return 0` dete the.
//  Matlab agar kal koi implementation tod deta, to demo GALAT number print
//  karta aur phir bhi "pass" dikhta — kisi ko pata hi nahi chalta.
//
//  Ab har zaroori invariant `demo::check(...)` se guzarta hai, aur `main()` ke
//  ant me `demo::report()` failure count ke hisaab se EXIT CODE deta hai.
//  Isi wajah se CI in demos ko regression test ki tarah chala sakta hai.
//
//  ⚠ Ye helper har folder ke apne common header me hai (shared file nahi) —
//     taaki har folder standalone rahe aur akela copy kiya ja sake.
// ============================================================================
namespace demo
{

inline int failureCount = 0;

// Ek shart check karo. Fail ho to loud print + ginti badhao.
inline void check(bool condition, const std::string &what)
{
    if (!condition)
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "\n";
    }
}

// Do value barabar honi chahiye
template <typename T, typename U>
inline void checkEqual(T actual, U expected, const std::string &what)
{
    if (!(actual == static_cast<T>(expected)))
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "  (mila " << actual
                  << ", chahiye tha " << expected << ")\n";
    }
}

// Value expected ke aas-paas honi chahiye — theory vs measurement ke liye
inline void checkNear(double actual, double expected, double tolerance,
                      const std::string &what)
{
    if (std::fabs(actual - expected) > tolerance)
    {
        ++failureCount;
        std::cout << "    ❌ VERIFY FAIL: " << what << "  (mila " << actual
                  << ", chahiye tha " << expected << " +/- " << tolerance << ")\n";
    }
}

// main() ke ant me: exit 0 = sab theek, exit 1 = kuch toota
inline int report()
{
    if (failureCount == 0)
    {
        std::cout << "\n✅ VERIFY: saare invariants theek hain.\n";
        return 0;
    }
    std::cout << "\n❌ VERIFY: " << failureCount << " invariant TOOT gaye.\n";
    return 1;
}

} // namespace demo

#endif // CONSISTENT_HASHING_HASH_UTIL_H
