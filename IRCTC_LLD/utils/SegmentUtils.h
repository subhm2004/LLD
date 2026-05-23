#ifndef IRCTC_LLD_UTILS_SEGMENTUTILS_H
#define IRCTC_LLD_UTILS_SEGMENTUTILS_H

namespace irctc_lld {

/** Half-open interval [fromIdx, toIdx) overlap check for seat segment reuse. */
inline bool segmentsOverlap(int fromA, int toA, int fromB, int toB) {
    return fromA < toB && fromB < toA;
}

}  // namespace irctc_lld

#endif  // IRCTC_LLD_UTILS_SEGMENTUTILS_H
