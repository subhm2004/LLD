// ============================================================================
//  factories/SplitFactory.h  —  FACTORY PATTERN (Creational)
// ----------------------------------------------------------------------------
//  Factory = "object banane ka kaam ek jagah rakh do; client sirf batayega
//             'mujhe ye TYPE chahiye', banane ki jhanjhat factory sambhalegi."
//
//  Ye Strategy pattern ka natural jodidar hai:
//    - Strategy: "kaam karne ke kai tareeke" (EqualSplit / ExactSplit / ...)
//    - Factory : "un tareekon me se sahi wala CHUNNE aur BANANE ka kaam"
//
//  Bina factory ke, har caller ko ye likhna padta:
//      SplitStrategy* s;
//      if      (type == SplitType::EQUAL)      s = new EqualSplit();
//      else if (type == SplitType::EXACT)      s = new ExactSplit();
//      else if (type == SplitType::PERCENTAGE) s = new PercentageSplit();
//
//  Aur ye 3 jagah copy hota (Group::addExpense, Group::editExpense,
//  Splitwise::addIndividualExpense, Splitwise::editIndividualExpense — asal me
//  4 jagah!). Naya split type add karne pe chaaro jagah edit karna padta, aur
//  ek jagah bhoolna = silent bug. Factory se wo switch SIRF EK JAGAH hai. Naya
//  type aaye to bas yahan ek `case` line add karo, baaki poora system untouched.
//
//  Fayda dobara, seedhe shabdon me:
//    1. Concrete class ke naam (EqualSplit, ExactSplit...) client se CHUPE
//       rehte hain. Client sirf enum jaanta hai aur `SplitStrategy*` pakadta hai.
//    2. Creation logic ek jagah -> ek jagah change, sab jagah asar.
//    3. Client `new` se aazaad. Kal ko strategies ko CACHE karna ho (stateless
//       hain, to ek hi object reuse ho sakta hai — bilkul Flyweight ki tarah,
//       L30 yaad karo!) to sirf ye file badalni padegi.
//
//  Ye kaunsa Factory hai?
//    Technically ye "Simple Factory" / "Static Factory" hai — ek static method
//    jo switch chalata hai. Poora GoF "Factory Method" tab hota jab factory
//    khud abstract hoti aur subclasses decide karti ki kya banana hai. LLD
//    interviews me 90% cases me yahi simple wala kaafi hota hai, aur isko hi
//    log "Factory" bolte hain.
// ============================================================================
#ifndef SPLITWISE_LLD_FACTORIES_SPLITFACTORY_H
#define SPLITWISE_LLD_FACTORIES_SPLITFACTORY_H

#include <bits/stdc++.h>

#include "../enums/SplitType.h"
#include "../strategies/SplitStrategy.h"

using namespace std;

namespace splitwise_lld {

class SplitFactory {
public:
    // `static` -> object banane ki zaroorat nahi. Seedha aise call karo:
    //     SplitFactory::getSplitStrategy(SplitType::EQUAL)
    // Kyunki factory ka apna koi STATE nahi hai (koi member variable nahi),
    // to object banane ka koi matlab hi nahi banta.
    //
    // Return type `SplitStrategy*` hai (BASE class ka pointer), `EqualSplit*`
    // nahi. Yahi to poora point hai — caller ko concrete type pata hi nahi
    // chalta. Wo bas `->calculateSplit()` bulata hai aur virtual dispatch se
    // sahi class ka function chal jaata hai.
    static SplitStrategy *getSplitStrategy(SplitType type) {
        switch (type) {
            case SplitType::EQUAL: return new EqualSplit();
            case SplitType::EXACT: return new ExactSplit();
            case SplitType::PERCENTAGE: return new PercentageSplit();

            // Defensive default — enum me kal koi naya value aaya aur yahan case
            // add karna bhool gaye, to crash hone ke bajaye EQUAL pe fallback.
            // (Debatable choice: kuch log yahan `throw` karna pasand karenge,
            //  taaki galti chhupe nahi balki turant pakdi jaaye. Dono theek hain
            //  — bas soch-samajh ke chuno.)
            default: return new EqualSplit();
        }
    }

    // ⚠ MEMORY KA ZIMMA — dhyaan se:
    //   Ye function `new` karta hai, par `delete` NAHI karta. To phir leak?
    //   Nahi — kyunki har caller ise turant `unique_ptr` me daal deta hai:
    //       unique_ptr<SplitStrategy> strategy(SplitFactory::getSplitStrategy(type));
    //   unique_ptr scope khatam hote hi khud delete kar deta hai. Isi liye
    //   SplitStrategy ka destructor virtual hona ZAROORI tha (warna base
    //   pointer se delete karne pe derived ka destructor skip ho jaata).
    //
    //   Aur bhi safe design: yahan se hi `unique_ptr<SplitStrategy>` return kar
    //   dena — tab caller ke paas raw pointer aayega hi nahi, aur leak karne ka
    //   option hi khatam. Ye is code me ek genuine improvement hai.
};

}

#endif
