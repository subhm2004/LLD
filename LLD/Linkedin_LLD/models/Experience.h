// ============================================================================
//  models/Experience.h  —  Profile me ek job/work experience (Requirement R1)
// ----------------------------------------------------------------------------
//  Requirement R1 (Profile Customization): "...educational background, WORK
//  EXPERIENCES, achievements, and skills."
//
//  Education jaisa hi ek chhota immutable value object — ek naukri ki details.
//  UserProfile ke andar inki ek list hoti hai (career ka poora safar).
//
//  📌 Education aur Experience ko ALAG classes rakha (ek generic class nahi) —
//     kyunki dono ke fields alag hain (Education me degree, Experience me title/
//     company). Alag rakhna zyada saaf hai aur galat field mix nahi hote.
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_EXPERIENCE_H
#define LINKEDIN_LLD_MODELS_EXPERIENCE_H

#include <string>
#include <utility>

namespace linkedin_lld {

class Experience {
public:
    Experience(std::string title, std::string company, std::string duration)
        : title_(std::move(title)), company_(std::move(company)), duration_(std::move(duration)) {}

    const std::string &getTitle() const { return title_; }
    const std::string &getCompany() const { return company_; }
    const std::string &getDuration() const { return duration_; }

private:
    std::string title_;    // "Senior Backend Engineer"
    std::string company_;  // "Google"
    std::string duration_; // "2021 - Present"
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_EXPERIENCE_H
