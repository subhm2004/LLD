#ifndef RAZORPAY_LLD_FACTORIES_ENTITY_ID_FACTORY_H
#define RAZORPAY_LLD_FACTORIES_ENTITY_ID_FACTORY_H

#include <string>

namespace razorpay_lld {

class EntityIdFactory {
public:
    static std::string nextOrderId() { return "order_" + std::to_string(++orderSeq_); }
    static std::string nextPaymentId() { return "pay_" + std::to_string(++paymentSeq_); }
    static std::string nextRefundId() { return "rfnd_" + std::to_string(++refundSeq_); }

private:
    inline static int orderSeq_{0};
    inline static int paymentSeq_{0};
    inline static int refundSeq_{0};
};

}  // namespace razorpay_lld

#endif
