// ============================================================================
//  models/Recommendation.h  —  Ek professional recommendation (Requirement R6)
// ----------------------------------------------------------------------------
//  Requirement R6 (Recommendations): "Users can request and provide professional
//  recommendations, enhancing the credibility and value of their professional
//  network."
//
//  Ye ek "tareef" hai jo ek user doosre ko deta hai:
//     author  -> jo recommendation LIKH raha hai
//     target  -> jiske BAARE me likhi ja rahi hai (jiske profile pe dikhegi)
//
//  Example: author "U2" (manager) target "U1" ke liye likhta hai —
//  "Shubham ek zabardast backend engineer hai, deadlines pe deliver karta hai."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ R6 me DO cheezein hain: "request" aur "provide"                       │
//  │                                                                          │
//  │    provide  -> koi seedha recommendation likh de (ye class + service)     │
//  │    request  -> "mujhe recommend karo" ki guzaarish bheji jaaye            │
//  │                                                                          │
//  │  Is model me hum ek `requested_` flag rakhte hain: agar recommendation   │
//  │  kisi request ke jawab me aayi, to text baad me bharega. Simple demo ke  │
//  │  liye hum mostly "provide" flow dikhate hain. RecommendationService is    │
//  │  poore lifecycle ko handle karti hai.                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_RECOMMENDATION_H
#define LINKEDIN_LLD_MODELS_RECOMMENDATION_H

#include <ctime>
#include <string>
#include <utility>

namespace linkedin_lld {

class Recommendation {
public:
    Recommendation(std::string recommendationId, std::string authorId, std::string targetId, std::string text)
        : recommendationId_(std::move(recommendationId)),
          authorId_(std::move(authorId)),
          targetId_(std::move(targetId)),
          text_(std::move(text)),
          createdAt_(std::time(nullptr)) {}

    const std::string &getRecommendationId() const { return recommendationId_; }
    const std::string &getAuthorId() const { return authorId_; } // kisne likhi
    const std::string &getTargetId() const { return targetId_; } // kiske liye
    const std::string &getText() const { return text_; }
    long long getCreatedAt() const { return createdAt_; }

private:
    std::string recommendationId_;
    std::string authorId_;
    std::string targetId_;
    std::string text_;
    long long createdAt_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_RECOMMENDATION_H
