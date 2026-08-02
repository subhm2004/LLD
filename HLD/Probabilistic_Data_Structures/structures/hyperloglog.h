// ============================================================================
//  structures/hyperloglog.h  —  HYPERLOGLOG
// ----------------------------------------------------------------------------
//  "Kitne UNIQUE items the?" ka jawab — CONSTANT memory me (16 KB).
//
//  Idea: hash ke shuru me kitne zero bits hain (= "head streak"). Lamba streak
//  dekha matlab bahut saare alag items aaye honge. Kismat ka asar hatane ke
//  liye hash ko m buckets me baanto aur ant me HARMONIC MEAN lo.
//
//  Formula:  estimate = alpha_m * m^2 / sum(2^(-M[j]))
//            standard error = 1.04 / sqrt(m)
//
//  ⭐ MERGE ho sakte hain (registers ka max lo) — isi liye distributed unique
//     counting me ye behtareen hai. Overlap apne aap handle ho jaata hai.
// ============================================================================
#ifndef PDS_STRUCTURES_HYPERLOGLOG_H
#define PDS_STRUCTURES_HYPERLOGLOG_H

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "../pds_common.h"

using namespace std;

namespace pds
{

class HyperLogLog
{
public:
    // precision b: registers m = 2^b. b=14 => 16384 registers = 16 KB (Redis default)
    explicit HyperLogLog(int precision = 14)
        : precision_(precision), m_(1u << precision), registers_(1u << precision, 0)
    {
        if (precision < 4 || precision > 18)
        {
            throw invalid_argument("precision 4 se 18 ke beech hona chahiye");
        }
        alpha_ = alphaFor(m_);
    }

    void add(const string &key)
    {
        uint64_t h = hash64(key);

        // Upar ke `b` bits = bucket ka index
        uint64_t index = h >> (64 - precision_);

        // Bache hue bits me shuru ke kitne ZERO (+1) = "streak"
        uint64_t rest = (h << precision_) | (1ULL << (precision_ - 1));
        uint8_t rank = static_cast<uint8_t>(__builtin_clzll(rest) + 1);

        registers_[index] = max(registers_[index], rank);
    }

    double estimate() const
    {
        double sum = 0.0;
        uint32_t zeroRegisters = 0;
        for (uint8_t value : registers_)
        {
            sum += 1.0 / (1ULL << value); // 2^(-M[j])
            if (value == 0)
            {
                ++zeroRegisters;
            }
        }

        double raw = alpha_ * m_ * m_ / sum;

        // ⚠ SMALL RANGE: chhoti cardinality pe "linear counting" behtar hai —
        //    khaali buckets ki ginti se andaaza lagao.
        if (raw <= 2.5 * m_ && zeroRegisters > 0)
        {
            return m_ * log(static_cast<double>(m_) / zeroRegisters);
        }
        return raw;
        // 64-bit hash ki wajah se "large range correction" ki zaroorat nahi.
    }

    // ⭐ MERGE = registers ka max. Jod nahi — isi liye overlap double nahi ginta.
    void merge(const HyperLogLog &other)
    {
        if (other.precision_ != precision_)
        {
            throw invalid_argument("precision same honi chahiye merge ke liye");
        }
        for (uint32_t i = 0; i < m_; ++i)
        {
            registers_[i] = max(registers_[i], other.registers_[i]);
        }
    }

    double standardError() const { return 1.04 / sqrt(static_cast<double>(m_)); }
    size_t memoryBytes() const { return registers_.size(); } // 1 byte per register
    uint32_t registerCount() const { return m_; }
    int precision() const { return precision_; }

private:
    static double alphaFor(uint32_t m)
    {
        switch (m)
        {
        case 16:
            return 0.673;
        case 32:
            return 0.697;
        case 64:
            return 0.709;
        default:
            return 0.7213 / (1.0 + 1.079 / m);
        }
    }

    int precision_;
    uint32_t m_;
    double alpha_;
    vector<uint8_t> registers_;
};

} // namespace pds

#endif // PDS_STRUCTURES_HYPERLOGLOG_H
