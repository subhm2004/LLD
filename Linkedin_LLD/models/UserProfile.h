// ============================================================================
//  models/UserProfile.h  —  Ek user ki poori professional pehchaan
// ----------------------------------------------------------------------------
//  Requirement R1 (Profile Customization): "Users can enrich their profiles with
//  educational background, work experiences, achievements, and skills, offering
//  a comprehensive professional summary."
//
//  Requirement R5 (Insights) ka ek hissa bhi yahin: har profile apne
//  "profile views" aur "search appearances" ka count rakhti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PURANA UserProfile sirf 3 field ka tha (userId, name, headline)       │
//  │                                                                          │
//  │  Wo 3-arg constructor ABHI BHI hai — purana code (main.cpp ka             │
//  │  `UserProfile("U1","Shubham","Backend Engineer")`) bilkul waise hi        │
//  │  chalta hai. Backward compatible. Bas ab uske UPAR education, experience, │
//  │  skills, achievements JODNE ki suvidha aa gayi hai.                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FLUENT / BUILDER-STYLE add* methods — `UserProfile&` lautate hain:
//
//      UserProfile("U1", "Shubham", "Backend Engineer")
//          .addEducation({"IIT Delhi", "B.Tech CSE", "2021"})
//          .addExperience({"SDE", "Google", "2021-Present"})
//          .addSkill("C++").addSkill("System Design")
//          .addAchievement("Published a paper on LLD");
//
//  Ye "method chaining" hai — har add apna hi reference wapas deta hai, to agla
//  add usi pe lag jaata hai. R1 ke bahut saare optional fields ko ek saaf line
//  me set karne ka tareeka (Builder pattern ka halka roop).
//
//  📌 Education/Experience alag classes hain (Education.h, Experience.h) —
//     skills aur achievements sirf strings hain, to unhe `vector<string>` me
//     rakha. Jitni cheez me structure chahiye utni hi banao, zyada nahi.
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_USERPROFILE_H
#define LINKEDIN_LLD_MODELS_USERPROFILE_H

#include <string>
#include <utility>
#include <vector>

#include "Education.h"
#include "Experience.h"

namespace linkedin_lld {

class UserProfile {
public:
    // ---- Purana 3-arg constructor — backward compatible ---------------------
    UserProfile(std::string userId, std::string name, std::string headline)
        : userId_(std::move(userId)), name_(std::move(name)), headline_(std::move(headline)) {}

    // ---- Core getters (existing) -------------------------------------------
    const std::string &getUserId() const { return userId_; }
    const std::string &getName() const { return name_; }
    const std::string &getHeadline() const { return headline_; }

    // ========================================================================
    //  R1 — profile enrich karne wale fluent setters (chaining ke liye &return)
    // ========================================================================
    UserProfile &addEducation(const Education &edu) {
        educations_.push_back(edu);
        return *this;
    }
    UserProfile &addExperience(const Experience &exp) {
        experiences_.push_back(exp);
        return *this;
    }
    UserProfile &addSkill(const std::string &skill) {
        skills_.push_back(skill);
        return *this;
    }
    UserProfile &addAchievement(const std::string &achievement) {
        achievements_.push_back(achievement);
        return *this;
    }

    // ---- R1 getters ---------------------------------------------------------
    const std::vector<Education> &getEducations() const { return educations_; }
    const std::vector<Experience> &getExperiences() const { return experiences_; }
    const std::vector<std::string> &getSkills() const { return skills_; }
    const std::vector<std::string> &getAchievements() const { return achievements_; }

    // ========================================================================
    //  R5 — analytics counters (profile ke saath rehte hain)
    // ========================================================================
    //  Ye counters AnalyticsService padhta/badhata hai. Yahan sirf store hote
    //  hain — "kaun dekhne pe kab badhega" ka faisla service karti hai (SRP).
    void incrementProfileViews() { ++profileViews_; }
    void incrementSearchAppearances() { ++searchAppearances_; }
    int getProfileViews() const { return profileViews_; }
    int getSearchAppearances() const { return searchAppearances_; }

private:
    // Core identity
    std::string userId_;   // "U1" — unique
    std::string name_;     // "Shubham"
    std::string headline_; // "Backend Engineer @ XYZ"

    // R1 — rich profile
    std::vector<Education> educations_;
    std::vector<Experience> experiences_;
    std::vector<std::string> skills_;
    std::vector<std::string> achievements_;

    // R5 — analytics (in-class init = har naya profile 0 se shuru)
    int profileViews_ = 0;
    int searchAppearances_ = 0;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_USERPROFILE_H
