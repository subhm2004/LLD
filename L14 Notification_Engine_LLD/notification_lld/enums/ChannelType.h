// enums/ChannelType.h — Notification kis channel se jaayega: EMAIL / SMS / POPUP.
// Type-safe enum taaki magic strings na use karne padein.
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
