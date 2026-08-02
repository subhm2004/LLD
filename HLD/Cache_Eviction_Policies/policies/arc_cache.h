// ============================================================================
//  policies/arc_cache.h  —  ARC (Adaptive Replacement Cache)
// ----------------------------------------------------------------------------
//  LRU aur LFU dono ki apni-apni kamzori hai:
//     LRU -> sequential scan pe mar jaata hai (naya data purane kaam ke data ko
//            bahaa deta hai)
//     LFU -> popularity badalne pe mar jaata hai (purani hit wali cheez chipak
//            jaati hai)
//
//  ⭐ ARC ka jawab: dono rakho, aur WORKLOAD KHUD DEKH KAR faisla karo ki abhi
//     kis pe zyada bharosa karna hai. Ye IBM ne 2003 me banaya tha.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CHAAR LIST — do asli, do "ghost"                                      │
//  │                                                                          │
//  │    T1 : jo cheezein EK BAAR dekhi (recency wali) — ASLI cache             │
//  │    T2 : jo cheezein DO+ BAAR dekhi (frequency wali) — ASLI cache          │
//  │    B1 : T1 se nikali gayi cheezon ke sirf NAAM (ghost — data nahi)        │
//  │    B2 : T2 se nikali gayi cheezon ke sirf NAAM (ghost — data nahi)        │
//  │                                                                          │
//  │  Asli cache = T1 + T2 (capacity `c` tak). B1/B2 me sirf keys hain, koi    │
//  │  data nahi — isliye wo lagbhag muft hain.                                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ GHOST LISTS HI POORA JAADU HAIN — ye samajh gaye to ARC samajh gaye │
//  │                                                                          │
//  │  Ghost list ka matlab hai: "ye cheez maine haal hi me NIKAALI thi."       │
//  │  Ab agar wahi cheez turant WAPAS maangi jaaye, to ye ek SABOOT hai ki     │
//  │  maine galat cheez nikaali thi — aur ARC us galti se SEEKHTA hai:         │
//  │                                                                          │
//  │    B1 me hit mila? => "maine recency wali cheez galti se nikaali"         │
//  │                       => p badhao (T1 ko zyada jagah do)                  │
//  │                                                                          │
//  │    B2 me hit mila? => "maine frequency wali cheez galti se nikaali"       │
//  │                       => p ghatao (T2 ko zyada jagah do)                  │
//  │                                                                          │
//  │  `p` = T1 ka target size. Yahi ek number poore cache ka "mood" batata     │
//  │  hai: p bada = abhi recency important hai, p chhota = frequency important.│
//  │                                                                          │
//  │  ⭐ Aur ye p HAR ACCESS pe apne aap adjust hota rehta hai. Kisi ne tune   │
//  │     nahi kiya, koi config nahi — workload khud bata deta hai.             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SCAN-RESISTANT KYUN HAI: scan me har key PEHLI baar aati hai, to wo sirf
//     T1 me jaati hai — T2 (jahan sach me kaam ki, baar-baar maangi jaane wali
//     cheezein hain) ko wo chhoo hi nahi paati. Isi liye scan poora cache nahi
//     bahaa pata. Yahi LRU pe iski sabse badi jeet hai.
//
//  ❌ Keemat: 4 lists ka hisaab, ~2x metadata (ghost keys), aur code kaafi
//     lamba. Aur IBM ka ispe patent tha (isi liye kai open-source projects ne
//     ARC ki jagah apne alternatives banaye).
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_ARC_CACHE_H
#define CACHE_EVICTION_POLICIES_ARC_CACHE_H

#include <algorithm>
#include <list>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class ArcCache : public CachePolicy
{
public:
    explicit ArcCache(size_t capacity) : capacity_(capacity)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
    }

    const char *name() const override { return "ARC"; }

    bool access(const string &key) override
    {
        auto found = locate_.find(key);

        if (found != locate_.end())
        {
            Which where = found->second.which;

            // ---- CASE I: asli cache me hai (T1 ya T2) => HIT --------------
            if (where == Which::T1 || where == Which::T2)
            {
                // Dono soorat me wo ab "do baar dekhi gayi" hai -> T2 ke top pe
                list<string> &from = (where == Which::T1) ? t1_ : t2_;
                t2_.splice(t2_.begin(), from, found->second.it);
                found->second.which = Which::T2;
                found->second.it = t2_.begin();
                return true;
            }

            // ---- CASE II: B1 me hit — "recency wali galti se nikali thi" --
            if (where == Which::B1)
            {
                double delta = (b1_.size() >= b2_.size())
                                   ? 1.0
                                   : static_cast<double>(b2_.size()) / b1_.size();
                p_ = min(static_cast<double>(capacity_), p_ + max(1.0, delta)); // T1 ko badhao

                replace(key);
                t2_.splice(t2_.begin(), b1_, found->second.it);
                found->second.which = Which::T2;
                found->second.it = t2_.begin();
                return false; // ghost hit = phir bhi MISS (data to tha hi nahi)
            }

            // ---- CASE III: B2 me hit — "frequency wali galti se nikali" ---
            double delta = (b2_.size() >= b1_.size())
                               ? 1.0
                               : static_cast<double>(b1_.size()) / b2_.size();
            p_ = max(0.0, p_ - max(1.0, delta)); // T1 ko ghatao (T2 ko jagah do)

            replace(key);
            t2_.splice(t2_.begin(), b2_, found->second.it);
            found->second.which = Which::T2;
            found->second.it = t2_.begin();
            return false;
        }

        // ---- CASE IV: bilkul nayi key (kahin bhi nahi) => MISS ------------
        size_t l1 = t1_.size() + b1_.size();
        size_t total = t1_.size() + t2_.size() + b1_.size() + b2_.size();

        if (l1 == capacity_)
        {
            if (t1_.size() < capacity_)
            {
                dropLru(b1_, Which::B1); // sabse purana ghost hatao
                replace(key);
            }
            else
            {
                dropLru(t1_, Which::T1); // B1 khaali hai — seedha T1 se nikalo
            }
        }
        else if (l1 < capacity_ && total >= capacity_)
        {
            if (total >= 2 * capacity_)
            {
                dropLru(b2_, Which::B2);
            }
            replace(key);
        }

        // Nayi key hamesha T1 me jaati hai ("abhi ek baar dekhi")
        t1_.push_front(key);
        locate_[key] = {Which::T1, t1_.begin()};
        return false;
    }

    void clear() override
    {
        t1_.clear();
        t2_.clear();
        b1_.clear();
        b2_.clear();
        locate_.clear();
        p_ = 0.0;
    }

    size_t capacity() const override { return capacity_; }

    // ---- Demo ke liye: abhi ARC ka "mood" kya hai ------------------------
    double targetT1() const { return p_; }
    size_t t1Size() const { return t1_.size(); }
    size_t t2Size() const { return t2_.size(); }

private:
    enum class Which
    {
        T1,
        T2,
        B1,
        B2
    };

    struct Location
    {
        Which which;
        list<string>::iterator it;
    };

    // ---- ⭐ REPLACE — asli cache se ek entry nikaal kar ghost me daalo ----
    //  Faisla: T1 apne target `p` se bada hai? To T1 se nikalo, warna T2 se.
    void replace(const string &key)
    {
        auto found = locate_.find(key);
        bool keyInB2 = (found != locate_.end() && found->second.which == Which::B2);

        if (!t1_.empty() &&
            (t1_.size() > p_ || (keyInB2 && t1_.size() == static_cast<size_t>(p_))))
        {
            // T1 ka sabse purana -> B1 (ghost) me
            const string victim = t1_.back();
            b1_.splice(b1_.begin(), t1_, prev(t1_.end()));
            locate_[victim] = {Which::B1, b1_.begin()};
        }
        else if (!t2_.empty())
        {
            // T2 ka sabse purana -> B2 (ghost) me
            const string victim = t2_.back();
            b2_.splice(b2_.begin(), t2_, prev(t2_.end()));
            locate_[victim] = {Which::B2, b2_.begin()};
        }
    }

    void dropLru(list<string> &from, Which which)
    {
        (void)which;
        if (from.empty())
        {
            return;
        }
        locate_.erase(from.back());
        from.pop_back();
    }

    size_t capacity_;
    double p_ = 0.0; // ⭐ T1 ka target size — yahi "mood" hai

    list<string> t1_, t2_; // asli cache
    list<string> b1_, b2_; // ghost (sirf keys, koi data nahi)
    unordered_map<string, Location> locate_;
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_ARC_CACHE_H
