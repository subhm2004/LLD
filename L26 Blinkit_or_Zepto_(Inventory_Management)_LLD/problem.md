# Problem Statement: Blinkit-Style Quick Commerce LLD

Design a low-level system for a quick commerce platform where users order grocery items and the system fulfills orders from nearby dark stores.

The system should support:

1. Dark store registration with location.
2. Product catalog and inventory per store.
3. Cart management for a user.
4. Nearby store discovery based on distance.
5. Order placement with stock checks and total calculation.
6. Dynamic delivery fee calculation based on distance and surge.
7. Order state machine for lifecycle transitions.

The architecture should be modular so features like payment, delivery assignment, and replenishment can be added later.
