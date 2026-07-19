// ============================================================================
//  enums/SwipeAction.h — Swipe types: LEFT / RIGHT / SUPER_LIKE
// ----------------------------------------------------------------------------
//  LEFT       = reject (na pasand — match nahi banega)
//  RIGHT      = like (pasand — mutual ho to match)
//  SUPER_LIKE = extra like (RIGHT jaisa match banata, par limited daily —
//               2 per day; normal swipe 10 per day). Tinder ka premium feel.
//  System match tabhi banata jab dono ne RIGHT ya SUPER_LIKE kiya ho.
//  Type-safe `enum class`.
// ============================================================================
#ifndef TINDER_LLD_ENUMS_SWIPEACTION_H
#define TINDER_LLD_ENUMS_SWIPEACTION_H

#include <bits/stdc++.h>

using namespace std;

namespace tinder_lld {

enum class SwipeAction { LEFT, RIGHT, SUPER_LIKE };

} // namespace tinder_lld

#endif // TINDER_LLD_ENUMS_SWIPEACTION_H
