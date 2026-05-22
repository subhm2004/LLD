#include <bits/stdc++.h>
#include <memory>

#include "core/PubSubSystem.h"
#include "subscribers/AnalyticsSubscriber.h"
#include "subscribers/EmailAlertSubscriber.h"
#include "subscribers/LoggingSubscriber.h"

using namespace std;
using namespace pub_sub_lld;

int main() {
    PubSubSystem broker;

    auto logger = make_shared<LoggingSubscriber>("log-1");
    auto email = make_shared<EmailAlertSubscriber>("email-1", "ops@company.com");
    auto analytics = make_shared<AnalyticsSubscriber>("analytics-1");

    broker.registerSubscriber(logger);
    broker.registerSubscriber(email);
    broker.registerSubscriber(analytics);

    broker.createTopic("order.created");
    broker.createTopic("payment.failed");

    broker.subscribe("order.created", "log-1");
    broker.subscribe("order.created", "email-1");
    broker.subscribe("order.created", "analytics-1");

    broker.subscribe("payment.failed", "log-1");
    broker.subscribe("payment.failed", "email-1");

    cout << "\n=== Publish order.created ===\n";
    broker.publish("order.created", R"({"orderId":"ORD-101","amount":2499})");

    cout << "\n=== Publish payment.failed ===\n";
    broker.publish("payment.failed", R"({"orderId":"ORD-102","reason":"timeout"})");

    cout << "\n=== Topic info ===\n";
    broker.printTopicInfo("order.created");

    cout << "\n=== Unsubscribe email from order.created ===\n";
    broker.unsubscribe("order.created", "email-1");

    cout << "\n=== Publish order.created again (email should not get it) ===\n";
    broker.publish("order.created", R"({"orderId":"ORD-103","amount":999})");

    cout << "\n=== Publish to unknown topic ===\n";
    try {
        broker.publish("inventory.updated", "payload");
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n=== Duplicate subscribe ===\n";
    try {
        broker.subscribe("order.created", "log-1");
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\nAnalytics events on order.created: " << analytics->getCount("order.created") << "\n";

    cout << "\nPub_Sub_System_LLD demo completed.\n";
    return 0;
}
