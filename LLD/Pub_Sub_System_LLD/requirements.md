# Requirements — Pub-Sub System

## Entities

| Entity | Description |
|--------|-------------|
| `Message` | messageId, topic, payload, publishedAtEpoch |
| `Topic` | string name (logical channel) |
| `ISubscriber` | Observer — `onMessage(Message)` |
| `Publisher` | Sends messages via broker |

## API (Facade — `PubSubSystem`)

| Method | Description |
|--------|-------------|
| `registerSubscriber(shared_ptr<ISubscriber>)` | Register consumer identity |
| `createTopic(topic)` | Create topic bucket |
| `subscribe(topic, subscriberId)` | Link subscriber to topic |
| `unsubscribe(topic, subscriberId)` | Remove link |
| `publish(topic, payload)` | Fan-out to all subscribers |
| `printTopicInfo(topic)` | Debug subscriber list |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Observer** | `ISubscriber::onMessage` |
| **Facade** | `PubSubSystem` |
| **Mediator / Broker** | `DispatchService` + `SubscriptionService` |
| **Strategy** (future) | DeliveryMode SYNC / ASYNC |

## Rules

- Cannot publish to non-existent topic.
- Cannot subscribe twice with same subscriber on same topic.
- Subscriber must be registered before subscribe.

## Production mapping

| This LLD | Real system |
|----------|-------------|
| `PubSubSystem` | Kafka cluster / SNS |
| `Topic` | Kafka topic / SNS topic |
| `ISubscriber` | Consumer group worker |
| `publish` | Producer `send()` |
