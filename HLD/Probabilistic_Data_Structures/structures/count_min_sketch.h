// ============================================================================
//  structures/count_min_sketch.h  —  COUNT-MIN SKETCH
// ----------------------------------------------------------------------------
//  "Ye item kitni baar aaya?" ka sasta jawab.
//
//  `d` rows x `w` columns ka table, har row ka apna hash:
//      add(x)      -> har row me table[i][h_i(x)] += count
//      estimate(x) -> saare rows me se SABSE CHHOTA value
//
//  ⭐ MIN isliye ki har cell me doosre items ki ginti bhi mili ho sakti hai,
//     to har row ka jawab asli se >= hota hai. Sabse chhota = sach ke kareeb.
//     Nateeja: kabhi KAM nahi batata, sirf ZYADA bata sakta hai.
//
//  Theory:  w = ceil(e/ε), d = ceil(ln(1/δ))
//           estimate <= asli + ε*N   (probability 1-δ ke saath)
//           jahan N = poore stream ka total count
// ============================================================================
#ifndef PDS_STRUCTURES_COUNT_MIN_SKETCH_H
#define PDS_STRUCTURES_COUNT_MIN_SKETCH_H

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "../pds_common.h"

using namespace std;

namespace pds
{

class CountMinSketch
{
public:
    CountMinSketch(size_t width, size_t depth)
        : width_(width), depth_(depth), table_(depth, vector<uint64_t>(width, 0))
    {
        if (width == 0 || depth == 0)
        {
            throw invalid_argument("width aur depth 0 se bade hone chahiye");
        }
        for (size_t i = 0; i < depth_; ++i)
        {
            seeds_.push_back(mix64(0x517cc1b727220a95ULL + i)); // har row ka apna hash
        }
    }

    // ⭐ Requirement se size: "ε error chahiye, δ probability ke saath"
    static CountMinSketch forError(double epsilon, double delta)
    {
        size_t width = static_cast<size_t>(ceil(M_E / epsilon));
        size_t depth = static_cast<size_t>(ceil(log(1.0 / delta)));
        return CountMinSketch(width, max<size_t>(1, depth));
    }

    void add(const string &key, uint64_t count = 1)
    {
        for (size_t i = 0; i < depth_; ++i)
        {
            table_[i][hash64(key, seeds_[i]) % width_] += count;
        }
        total_ += count;
    }

    uint64_t estimate(const string &key) const
    {
        uint64_t best = UINT64_MAX;
        for (size_t i = 0; i < depth_; ++i)
        {
            best = min(best, table_[i][hash64(key, seeds_[i]) % width_]);
        }
        return best;
    }

    // Theory ka vaada — error is se zyada nahi hona chahiye
    double errorBound() const { return (M_E / width_) * static_cast<double>(total_); }

    double epsilon() const { return M_E / width_; }
    uint64_t totalCount() const { return total_; }
    size_t memoryBytes() const { return depth_ * width_ * sizeof(uint64_t); }
    size_t width() const { return width_; }
    size_t depth() const { return depth_; }

private:
    size_t width_, depth_;
    vector<vector<uint64_t>> table_;
    vector<uint64_t> seeds_;
    uint64_t total_ = 0;
};

} // namespace pds

#endif // PDS_STRUCTURES_COUNT_MIN_SKETCH_H
