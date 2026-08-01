// ============================================================================
//  models/Education.h  —  Profile me ek padhai ka entry (Requirement R1)
// ----------------------------------------------------------------------------
//  Requirement R1 (Profile Customization): "Users can enrich their profiles with
//  EDUCATIONAL BACKGROUND, work experiences, achievements, and skills."
//
//  Ye ek chhota "value object" hai — sirf data (school, degree, saal). Koi
//  behavior nahi, koi logic nahi. UserProfile ke andar inki ek list hoti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IMMUTABLE by design — koi setter nahi                                 │
//  │  Ek baar Education ban gaya, uske fields badalte nahi. Badalna ho to      │
//  │  purana hata ke naya add karo. Value objects ko immutable rakhna safe     │
//  │  hota hai — copy/share sab bina dar ke.                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_EDUCATION_H
#define LINKEDIN_LLD_MODELS_EDUCATION_H

#include <string>
#include <utility>

namespace linkedin_lld {

class Education {
public:
    Education(std::string school, std::string degree, std::string year)
        : school_(std::move(school)), degree_(std::move(degree)), year_(std::move(year)) {}

    const std::string &getSchool() const { return school_; }
    const std::string &getDegree() const { return degree_; }
    const std::string &getYear() const { return year_; }

private:
    std::string school_; // "IIT Delhi"
    std::string degree_; // "B.Tech, Computer Science"
    std::string year_;   // "2021"
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_EDUCATION_H
