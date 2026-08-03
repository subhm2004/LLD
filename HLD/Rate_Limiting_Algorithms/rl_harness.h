// ============================================================================
//  rl_harness.h  —  Demos ke liye chhota sa test harness (VIRTUAL TIME)
// ----------------------------------------------------------------------------
//  ⚠ Ye file `rate_limiter/` walon ka hissa NAHI hai. Wo saare headers aapke
//     apne repo (github.com/subhm2004/Rate_Limiter) se JYON KE TYON copy kiye
//     gaye hain — unme ek line bhi nahi badli. Ye harness sirf demos ko
//     chalane ke liye upar se lagaya gaya hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PROBLEM: `allow()` ASLI GHADI use karta hai                           │
//  │                                                                          │
//  │  `RateLimiter::allow()` andar `now_seconds()` bulata hai — yaani asli     │
//  │  steady_clock. Production ke liye bilkul sahi, par DEMO ke liye bekaar:   │
//  │                                                                          │
//  │    * "window boundary pe kya hota hai" dikhane ke liye 10 second SO-NA    │
//  │      padega (`sleep`) — har demo minute-bhar lega.                       │
//  │    * Aur result har machine pe thoda alag aayega (timing jitter) —        │
//  │      yaani demo bharosemand hi nahi rahega.                              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ HAL: `check()` ko seedha bulao, apna time de kar                      │
//  │                                                                          │
//  │  Aapke design me ek bahut achhi baat hai (shayad aapne jaan-bujh ke ki    │
//  │  thi): asli logic `check(key, now, cost)` me hai, aur `now` BAHAR se      │
//  │  aata hai. `allow()` sirf ek patla wrapper hai jo ghadi padh ke `check`   │
//  │  ko de deta hai.                                                         │
//  │                                                                          │
//  │  Iska matlab: hum `check` ko apna NAKLI time de kar bula sakte hain, aur  │
//  │  1 microsecond me 10 ghante ka traffic simulate kar sakte hain. ⭐        │
//  │                                                                          │
//  │  Isi ko DEPENDENCY INJECTION kehte hain — time ko hard-code karne ki      │
//  │  jagah bahar se lena. Isi wajah se aapka code TESTABLE hai.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ `check` PROTECTED hai (bilkul sahi — bahar wale ko time inject nahi karne
//     dena chahiye). Par ek DERIVED class use bula sakti hai. Isi liye neeche
//     `Probe<T>` hai: wo aapke algorithm se inherit karta hai aur `check` ko
//     ek public `at()` ke through khol deta hai — sirf test ke liye.
//
//  ⚠ Dhyan: `at()` mutex NAHI leta (`allow()` leta hai). Demos single-threaded
//     hain isliye theek hai, par production me kabhi `check` ko seedha mat bulao.
// ============================================================================
#ifndef RATE_LIMITING_ALGORITHMS_RL_HARNESS_H
#define RATE_LIMITING_ALGORITHMS_RL_HARNESS_H

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "rate_limiter/base.h"

using namespace std;

namespace rlx {

// ============================================================================
//  Probe<Algo> — kisi bhi rate limiter ko "virtual time" ke saath chalao
// ----------------------------------------------------------------------------
//  Use: rlx::Probe<rl::TokenBucket> tb(5, 1);  // wahi constructor args
//       tb.at("user1", 0.0);                   // t=0 second pe request
//       tb.at("user1", 3.5);                   // t=3.5 second pe request
// ============================================================================
template <typename Algo>
class Probe : public Algo {
public:
    // Algo ke saare constructors jaise ke taise mil jaate hain.
    using Algo::Algo;

    // `now` humare haath me — asli ghadi ka koi role nahi.
    rl::Decision at(const string &key, double now, int cost = 1) {
        return this->check(key, now, cost);
    }
};

// ---- Ek decision ko ek line me print karo --------------------------------
inline void printDecision(double now, const rl::Decision &d, const string &note = "")
{
    cout << "    t=" << fixed << setprecision(2) << setw(6) << now << "s  "
         << (d.allowed ? "✅ ALLOW" : "❌ DENY ") << "  used=" << setw(5) << setprecision(2)
         << d.used << "/" << setprecision(0) << d.limit;

    if (!d.allowed) {
        cout << "  retry_after=" << setprecision(2) << d.retry_after << "s";
    }
    if (!note.empty()) {
        cout << "   " << note;
    }
    cout << "\n";
}

// ---- Ek burst bhejo aur ginti wapas karo ---------------------------------
//  `count` requests, sab EK HI waqt pe (`now`) — yaani ek instant burst.
template <typename Probe_>
inline int sendBurst(Probe_ &limiter, const string &key, double now, int count,
                     bool verbose = false)
{
    int allowed = 0;
    for (int i = 0; i < count; ++i) {
        rl::Decision d = limiter.at(key, now);
        if (d.allowed) {
            ++allowed;
        }
        if (verbose) {
            printDecision(now, d);
        }
    }
    return allowed;
}

// ---- Timeline: har tick pe ek request, allow/deny ka pattern dikhao -------
//  Output kuch aisa: ✅✅✅✅✅❌❌❌❌❌
template <typename Probe_>
inline void printTimeline(Probe_ &limiter, const string &key, double start, double step,
                          int steps)
{
    // Koi indent nahi — caller apna label pehle print karta hai.
    for (int i = 0; i < steps; ++i) {
        cout << (limiter.at(key, start + i * step).allowed ? "✅" : "❌");
    }
    cout << "\n";
}

} // namespace rlx


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

#endif // RATE_LIMITING_ALGORITHMS_RL_HARNESS_H
