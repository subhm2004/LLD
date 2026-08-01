// ============================================================================
//  main.cpp  —  NOTIFICATION ENGINE ka demo driver (L14 modular version)
// ----------------------------------------------------------------------------
//  System kya karta hai: ek notification banao, use DECORATE karo (timestamp
//  + signature chipkao), aur ek saath KAI channels (Email/SMS/Popup) pe
//  bhej do — plus har notification ko Logger bhi log kare.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  EK sendNotification() call ke PEECHE kya hota hai:                     │
//  │                                                                          │
//  │   1. NotificationMessage banta hai (title/body/priority)               │
//  │   2. SimpleNotification -> TimestampDecorator -> SignatureDecorator     │
//  │      (DECORATOR: layer pe layer content sajta hai)                      │
//  │   3. NotificationHub (SINGLETON) ka observable set hota hai             │
//  │   4. Observable sabhi OBSERVERS ko notify karta hai:                    │
//  │        - LoggerObserver -> log print                                    │
//  │        - NotificationEngine -> har STRATEGY (Email/SMS/Popup) pe send   │
//  │                                                                          │
//  │  Output me EK message 4 baar dikhta hai (Logger + 3 channels) —         │
//  │  yahi Observer + Strategy ka combined power!                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS PROJECT ME 4 PATTERNS: Observer + Strategy + Decorator + Singleton.
//  Detail: ../design_patterns_used.md padho!
// ============================================================================
#include "core/NotificationSystem.h"
#include <iostream>

using namespace notification_lld;

int main() {
    NotificationSystem system;
    system.setupDefaultEngine();

    system.registerRecipient("u1", "Shubham", "shubham@example.com", ChannelType::EMAIL);
    system.registerRecipient("u2", "Rahul", "+91-9876543210", ChannelType::SMS);

    system.sendNotification("u1", "n1", "Order Update", "Your order has been shipped", NotificationPriority::HIGH);
    system.sendNotification("u2", "n2", "OTP Alert", "Your OTP is 445566", NotificationPriority::MEDIUM);

    std::cout << "Notification LLD demo executed." << std::endl;
    return 0;
}
