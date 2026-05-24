#ifndef CRICBUZZ_LLD_COMMENTARY_I_COMMENTARY_GENERATOR_H
#define CRICBUZZ_LLD_COMMENTARY_I_COMMENTARY_GENERATOR_H

#include <string>

#include "../enums/BallOutcome.h"

namespace cricbuzz_lld {

class ICommentaryGenerator {
public:
    virtual ~ICommentaryGenerator() = default;
    virtual std::string generate(const std::string& batsmanName, const std::string& bowlerName,
                                 BallOutcome outcome, int runs, bool isWicket) const = 0;
};

}  // namespace cricbuzz_lld

#endif
