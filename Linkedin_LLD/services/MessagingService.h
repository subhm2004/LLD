// ============================================================================
//  services/MessagingService.h  —  Direct messaging (Requirement R9)
// ----------------------------------------------------------------------------
//  R9 (Messaging): "Direct communication is facilitated through messaging,
//  allowing users to send and receive messages with other professionals."
//
//  Ye service messages ko STORE karti hai aur "inbox" / "conversation" nikaal
//  ke deti hai. Message ID khud generate karti hai (apna counter) — poori
//  messaging self-contained hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Saare messages ek hi list me — filter karke inbox/conversation nikalte│
//  │                                                                          │
//  │  Simple demo model: ek `vector<Message>` me sab messages. Kiske inbox    │
//  │  me kya hai = `to == userId` wale filter kar do. Do logon ki baat-cheet  │
//  │  (conversation) = jinme (from,to) us jodi ke ho.                         │
//  │                                                                          │
//  │  ⚠ Scale pe ye O(N) filter mehnga hoga — asli system me per-user index   │
//  │  (map<userId, vector<Message>>) ya DB hota. Interview me ye trade-off     │
//  │  bolna acchha rehta hai. Yahan clarity > micro-optimization.             │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Notification ka kaam ye service NAHI karti — wo NotificationService ka
//     zimma hai. Yahan bas message store hota hai; "ping" facade bhijwata hai
//     (SRP: messaging aur notification alag concerns).
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_MESSAGINGSERVICE_H
#define LINKEDIN_LLD_SERVICES_MESSAGINGSERVICE_H

#include <string>
#include <vector>

#include "../models/Message.h"

namespace linkedin_lld {

class MessagingService {
public:
    // ---- R9: message bhejo -> store karo, message ID lauta do --------------
    std::string sendMessage(const std::string &fromUserId, const std::string &toUserId,
                            const std::string &content) {
        const std::string messageId = "MSG_" + std::to_string(++messageCounter_);
        messages_.emplace_back(messageId, fromUserId, toUserId, content);
        return messageId;
    }

    // ---- Kisi user ke paas aaye saare messages (inbox: to == userId) -------
    std::vector<Message> getInbox(const std::string &userId) const {
        std::vector<Message> inbox;
        for (const Message &m : messages_) {
            if (m.getToUserId() == userId) {
                inbox.push_back(m);
            }
        }
        return inbox;
    }

    // ---- Do users ki poori baat-cheet (dono direction ke messages) ---------
    std::vector<Message> getConversation(const std::string &userA, const std::string &userB) const {
        std::vector<Message> convo;
        for (const Message &m : messages_) {
            const bool aToB = m.getFromUserId() == userA && m.getToUserId() == userB;
            const bool bToA = m.getFromUserId() == userB && m.getToUserId() == userA;
            if (aToB || bToA) {
                convo.push_back(m);
            }
        }
        return convo;
    }

private:
    std::vector<Message> messages_; // saare messages (upar note: demo-simple storage)
    int messageCounter_ = 0;        // unique message ID ke liye
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_MESSAGINGSERVICE_H
