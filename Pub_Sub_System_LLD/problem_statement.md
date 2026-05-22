# Problem Statement: Pub-Sub System (Message Broker)

Design an in-memory **Publish–Subscribe** messaging system where publishers send messages to **topics** and subscribers receive only messages from topics they subscribed to.

## Functional requirements

1. **Topics** — named channels (e.g. `order.created`).
2. **Publish** — producer sends payload to a topic.
3. **Subscribe / Unsubscribe** — consumer registers interest in a topic.
4. **Fan-out** — one publish delivers to **all** subscribers of that topic.
5. **Multiple subscriber types** — logging, email alert, analytics (pluggable).
6. **Delivery** — synchronous dispatch (async queue as extension).

## Non-functional

- Decouple publishers from subscribers (no direct references).
- Extensible for Kafka/RabbitMQ-style persistence later.
- C++17, header-based modular layout.

## Out of scope

- Real Kafka cluster, partitions, consumer groups
- Message persistence / replay (extension)
- Network transport
