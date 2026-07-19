// ============================================================================
//  enums/PaymentChannel.h — Exit pe payment KAHAN karein (requirement #9) [NAYA]
// ----------------------------------------------------------------------------
//  Requirements ke hisaab se exit pe do payment options:
//    AUTOMATED_MACHINE -> automated machine pe khud pay karo
//    AGENT             -> parking agent (banda) ko pay karo
//
//  Ye enum PaymentProcessorFactory ko batata hai kaunsa payment processor
//  (Strategy) banana hai. PaymentMethod (card/cash/upi) se alag — ye "KAHAN"
//  hai, wo "KAISE" hai.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTCHANNEL_H
#define PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTCHANNEL_H

namespace parking_lot_lld {

enum class PaymentChannel { AUTOMATED_MACHINE, AGENT };

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTCHANNEL_H
