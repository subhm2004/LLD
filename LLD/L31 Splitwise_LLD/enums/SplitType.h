// ============================================================================
//  enums/SplitType.h  —  Expense kis tarah baantna hai, uska "type tag"
// ----------------------------------------------------------------------------
//  Ye sirf ek enum hai, par LLD me iska role bada important hai: ye CLIENT ki
//  zubaan hai. Client bolta hai "mujhe EQUAL split chahiye" — usse ye nahi pata
//  hona chahiye ki andar `EqualSplit` naam ki koi class hai. Wo bas enum bhejta
//  hai, aur SplitFactory us enum ko dekh ke sahi strategy object bana deti hai.
//
//  Teen tarike:
//    EQUAL      -> total ko members ki count se barabar baant do.
//                  800 rupaye, 4 log -> har banda 200.
//    EXACT      -> caller khud batata hai kaun kitna dega (rupayo me).
//                  700 rupaye -> [200, 300, 200] (khud add hoke 700 hona chahiye)
//    PERCENTAGE -> caller percentage batata hai, amount hum calculate karte hain.
//                  1000 rupaye -> [50, 30, 20] % -> [500, 300, 200] rupaye
//
//  `enum class` kyun, plain `enum` kyun nahi?
//    Plain enum apne naam bahar leak kar deta hai (EQUAL globally dikhne lagta
//    hai) aur chupke se int me convert ho jaata hai — yaani `SplitType::EQUAL`
//    aur koi random `1` ko compiler same maan lega. `enum class` scoped hota
//    hai: naam `SplitType::EQUAL` hi likhna padega, aur galti se int ke saath
//    compare/assign nahi ho sakta. Type-safety free me mil jaati hai.
//
//  Naya split type add karna ho (jaise SHARES / ADJUSTMENT)? Sirf 2 jagah touch:
//    1. yahan enum me ek entry
//    2. SplitStrategy.h me uski class + SplitFactory.h me ek case
//  Baaki poora system (Group, Splitwise, main) chhune ki zaroorat hi nahi —
//  yahi Strategy + Factory ka asli fayda hai.
// ============================================================================
#ifndef SPLITWISE_LLD_ENUMS_SPLITTYPE_H
#define SPLITWISE_LLD_ENUMS_SPLITTYPE_H

#include <bits/stdc++.h>
using namespace std;

namespace splitwise_lld {

// Split ke 3 supported tarike. SplitFactory isi value ko dekh ke decide karta
// hai ki EqualSplit / ExactSplit / PercentageSplit me se kaunsa object banega.
enum class SplitType { EQUAL, EXACT, PERCENTAGE };

}

#endif
