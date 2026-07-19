// ============================================================================
//  enums/ChannelType.h — Notification kis channel se: EMAIL / SMS / POPUP
// ----------------------------------------------------------------------------
//  Recipient ka preferred channel batata hai. `enum class` (scoped) —
//  ChannelType::EMAIL likhna padta hai, aur int me chupke se convert nahi
//  hota (type-safe). Magic strings ("email") se behtar — typo compile-time
//  pe pakda jayega.
// ============================================================================
#ifndef CHANNEL_TYPE_H
#define CHANNEL_TYPE_H

namespace notification_lld {

enum class ChannelType {
    EMAIL,
    SMS,
    POPUP
};

}  // namespace notification_lld

#endif
