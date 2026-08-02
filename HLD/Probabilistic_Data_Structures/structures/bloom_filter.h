// ============================================================================
//  structures/bloom_filter.h  —  BLOOM FILTER
// ----------------------------------------------------------------------------
//  "Ye item pehle dekha tha kya?" ka sasta jawab.
//     "PAKKA NAHI HAI" -> 100% bharosemand
//     "SHAYAD HAI"     -> thoda jhooth bol sakta hai (false positive)
//
//  Formula:
//      false positive rate  p = (1 - e^(-k*n/m))^k
//      best size            m = -(n * ln p) / (ln 2)^2
//      best hash count      k = (m/n) * ln 2
// ============================================================================
#ifndef PDS_STRUCTURES_BLOOM_FILTER_H
#define PDS_STRUCTURES_BLOOM_FILTER_H

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "../pds_common.h"

using namespace std;

namespace pds
{

class BloomFilter
{
public:
    BloomFilter(size_t bits, int hashCount)
        : bits_(bits), hashCount_(hashCount), data_((bits + 63) / 64, 0ULL)
    {
        if (bits == 0 || hashCount <= 0)
        {
            throw invalid_argument("bits aur hashCount 0 se bade hone chahiye");
        }
    }

    // ⭐ Practical banane wala constructor: "n items, p false-positive chahiye"
    static BloomFilter forCapacity(size_t expectedItems, double falsePositiveRate)
    {
        double m = -(static_cast<double>(expectedItems) * log(falsePositiveRate)) / (M_LN2 * M_LN2);
        double k = (m / expectedItems) * M_LN2;
        return BloomFilter(static_cast<size_t>(ceil(m)), max(1, static_cast<int>(round(k))));
    }

    void add(const string &key)
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < hashCount_; ++i)
        {
            setBit((h1 + static_cast<uint64_t>(i) * h2) % bits_);
        }
    }

    // false = PAKKA nahi hai | true = SHAYAD hai
    bool mightContain(const string &key) const
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < hashCount_; ++i)
        {
            // Ek bhi bit 0 => item add hua hi nahi tha. Turant nikal jao.
            if (!getBit((h1 + static_cast<uint64_t>(i) * h2) % bits_))
            {
                return false;
            }
        }
        return true;
    }

    double theoreticalFpRate(size_t itemsAdded) const
    {
        double exponent = -static_cast<double>(hashCount_) * itemsAdded / bits_;
        return pow(1.0 - exp(exponent), hashCount_);
    }

    // Kitni bits abhi 1 hain — optimal pe ye ~50% hota hai
    double fillRatio() const
    {
        size_t ones = 0;
        for (uint64_t word : data_)
        {
            ones += static_cast<size_t>(__builtin_popcountll(word));
        }
        return static_cast<double>(ones) / bits_;
    }

    size_t bitCount() const { return bits_; }
    int hashCount() const { return hashCount_; }
    size_t memoryBytes() const { return data_.size() * sizeof(uint64_t); }

private:
    void setBit(uint64_t index) { data_[index >> 6] |= (1ULL << (index & 63)); }
    bool getBit(uint64_t index) const { return (data_[index >> 6] >> (index & 63)) & 1ULL; }

    size_t bits_;
    int hashCount_;
    vector<uint64_t> data_; // bits ko 64-64 ke packet me
};

} // namespace pds

#endif // PDS_STRUCTURES_BLOOM_FILTER_H
