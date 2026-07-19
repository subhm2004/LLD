// ============================================================================
//  enums/Gender.h — User ka gender: MALE / FEMALE / NON_BINARY / OTHER
// ----------------------------------------------------------------------------
//  Profile me store hota; real Tinder me preference filtering (kisko dikhana
//  hai) me use hota. Inclusive options (NON_BINARY/OTHER) — modern dating
//  apps me zaroori. Type-safe `enum class`.
// ============================================================================
#ifndef TINDER_LLD_ENUMS_GENDER_H
#define TINDER_LLD_ENUMS_GENDER_H

#include <bits/stdc++.h>

using namespace std;

namespace tinder_lld {

enum class Gender { MALE, FEMALE, NON_BINARY, OTHER };

} // namespace tinder_lld

#endif // TINDER_LLD_ENUMS_GENDER_H
