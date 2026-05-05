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
