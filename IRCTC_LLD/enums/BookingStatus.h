// ============================================================================
//  enums/BookingStatus.h  —  Ek ticket booking ki state
// ----------------------------------------------------------------------------
//     CONFIRMED -> ticket pakka (seat is segment ke liye reserved)
//     CANCELLED -> cancel ho gaya (seat segment ab dobara bik sakti hai)
//
//  📌 Is LLD me sirf CONFIRMED/CANCELLED hain — koi RAC/Waitlist/PENDING nahi
//     (problem statement me "only CONFIRMED bookings" likha hai). Real IRCTC me
//     RAC, WL, PENDING bhi hote, par interview-scope me ye 2 kaafi.
//
//  ⭐ CANCELLED status ka role SEAT REUSE me: SeatAllocationService overlap
//     check karte waqt sirf CONFIRMED bookings dekhta hai — CANCELLED ko IGNORE
//     karta hai. Isi liye ticket cancel hote hi wo seat-segment fir se available
//     ho jaata hai (booking record delete nahi hota, bas status CANCELLED).
// ============================================================================
#ifndef IRCTC_LLD_ENUMS_BOOKINGSTATUS_H
#define IRCTC_LLD_ENUMS_BOOKINGSTATUS_H

namespace irctc_lld {

enum class BookingStatus { CONFIRMED, CANCELLED };

}  // namespace irctc_lld

#endif  // IRCTC_LLD_ENUMS_BOOKINGSTATUS_H
