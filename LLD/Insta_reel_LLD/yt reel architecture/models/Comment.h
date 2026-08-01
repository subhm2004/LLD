// ============================================================================
//  models/Comment.h  —  Ek reel pe kiya gaya comment
// ----------------------------------------------------------------------------
//  Teen cheezein: kisne kiya (userId), kya likha (text), aur kab (timestamp).
//
//  📌 `struct` (class nahi) — ye ek plain data holder hai (koi logic/invariant
//     nahi jise chhupana pade). Comments `Reel` ke andar `vector<Comment>` me
//     rehte hain (Reel unhe OWN karta — composition).
//
//  💡 Comment ka apna `commentId` nahi hai (LinkedIn LLD ke ulat) — kyunki yahan
//     comment pe koi operation nahi (edit/delete/react). Sirf add + count. Agar
//     aage "comment delete/like" chahiye to id add karni padegi.
// ============================================================================
#ifndef INSTA_REEL_LLD_MODELS_COMMENT_H
#define INSTA_REEL_LLD_MODELS_COMMENT_H

#include <string>

using namespace std;

namespace reel_platform_lld {

struct Comment {
    string userId;       // kisne comment kiya
    string text;         // comment ka content
    long long timestamp; // kab (epoch seconds)
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_MODELS_COMMENT_H
