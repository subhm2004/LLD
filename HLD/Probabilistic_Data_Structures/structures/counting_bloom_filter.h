// ============================================================================
//  structures/counting_bloom_filter.h  —  COUNTING BLOOM FILTER
// ----------------------------------------------------------------------------
//  Bloom filter + DELETE. Bit ki jagah 4-bit counter:
//      add    -> counter++
//      remove -> counter--
//      check  -> saare counters > 0?
//
//  Keemat: 4x memory (1 bit -> 4 bits).
//
//  ⚠ Counter 15 pe SATURATE ho jaata hai. Saturated counter ko kabhi ghatate
//    nahi — uska asli count 15 se zyada ho sakta tha, ghatane se false
//    negative aa jaayega (aur wahi ek guarantee thi jo bachani hai).
// ============================================================================
#ifndef PDS_STRUCTURES_COUNTING_BLOOM_FILTER_H
#define PDS_STRUCTURES_COUNTING_BLOOM_FILTER_H

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "../pds_common.h"

using namespace std;

namespace pds
{

class CountingBloomFilter
{
public:
    CountingBloomFilter(size_t slots, int hashCount)
        : slots_(slots), hashCount_(hashCount), counters_((slots + 1) / 2, 0)
    {
        // Do 4-bit counters ek byte me pack kiye hain -> slots/2 bytes
        if (slots == 0 || hashCount <= 0)
        {
            throw invalid_argument("slots aur hashCount 0 se bade hone chahiye");
        }
    }

    static CountingBloomFilter forCapacity(size_t expectedItems, double falsePositiveRate)
    {
        double m = -(static_cast<double>(expectedItems) * log(falsePositiveRate)) / (M_LN2 * M_LN2);
        double k = (m / expectedItems) * M_LN2;
        return CountingBloomFilter(static_cast<size_t>(ceil(m)), max(1, static_cast<int>(round(k))));
    }

    void add(const string &key)
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < hashCount_; ++i)
        {
            size_t index = (h1 + static_cast<uint64_t>(i) * h2) % slots_;
            uint8_t value = get(index);
            if (value < MAX_COUNT)
            {
                set(index, static_cast<uint8_t>(value + 1));
            }
            else
            {
                ++overflows_; // 15 pe atak gaya — ise ab kabhi ghata nahi sakte
            }
        }
    }

    bool remove(const string &key)
    {
        if (!mightContain(key))
        {
            return false; // tha hi nahi — ghatana khatarnak hoga
        }
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < hashCount_; ++i)
        {
            size_t index = (h1 + static_cast<uint64_t>(i) * h2) % slots_;
            uint8_t value = get(index);
            // ⚠ Saturated (15) ko kabhi mat ghatao
            if (value > 0 && value < MAX_COUNT)
            {
                set(index, static_cast<uint8_t>(value - 1));
            }
        }
        return true;
    }

    bool mightContain(const string &key) const
    {
        uint64_t h1, h2;
        doubleHash(key, h1, h2);
        for (int i = 0; i < hashCount_; ++i)
        {
            if (get((h1 + static_cast<uint64_t>(i) * h2) % slots_) == 0)
            {
                return false;
            }
        }
        return true;
    }

    size_t memoryBytes() const { return counters_.size(); }
    long long overflowCount() const { return overflows_; }
    size_t slotCount() const { return slots_; }
    int hashCount() const { return hashCount_; }

private:
    static const uint8_t MAX_COUNT = 15; // 4 bits

    uint8_t get(size_t index) const
    {
        uint8_t byte = counters_[index >> 1];
        return (index & 1) ? (byte >> 4) : (byte & 0x0F);
    }
    void set(size_t index, uint8_t value)
    {
        uint8_t &byte = counters_[index >> 1];
        if (index & 1)
        {
            byte = static_cast<uint8_t>((byte & 0x0F) | (value << 4));
        }
        else
        {
            byte = static_cast<uint8_t>((byte & 0xF0) | value);
        }
    }

    size_t slots_;
    int hashCount_;
    vector<uint8_t> counters_;
    long long overflows_ = 0;
};

} // namespace pds

#endif // PDS_STRUCTURES_COUNTING_BLOOM_FILTER_H
