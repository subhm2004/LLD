# Problem Statement: Blinkit / Zepto Inventory Management LLD

Design a low-level inventory and order fulfillment system for a quick-commerce platform (Blinkit/Zepto style).

The system should support:

1. Dark store onboarding with location.
2. Inventory add/remove per store using pluggable `InventoryStore` backends.
3. Replenishment strategies (Threshold, Weekly) using Strategy pattern.
4. User catalog view from all dark stores within **5 KM** radius.
5. Cart and order placement with stock validation.
6. Order splitting across multiple dark stores when one store cannot fulfill full cart.
7. Multiple delivery partners for split fulfillment.
8. Delivery fee calculation and order lifecycle states.

Architecture should remain modular so payment, routing, and persistence can be extended later.
