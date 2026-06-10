# Requirements: Blinkit / Zepto Inventory Management LLD

## Functional Requirements

1. **Inventory management**
   - System should support adding and removing inventory items (SKU + quantity) per dark store.

2. **Replenishment strategies (Strategy pattern)**
   - System should support replenishment strategies (e.g. Threshold-based, Weekly).
   - Strategy should be swappable per dark store and scalable for new strategies.

3. **Multiple inventory store backends**
   - System should support multiple `InventoryStore` implementations (e.g. in-memory, DB-backed simulation).
   - New store types should be addable without changing dark-store business logic.

4. **5 KM catalog visibility**
   - User should be able to see products available from all dark stores within a **5 KM** service radius.

5. **Order placement with stock validation**
   - User should be able to add products to cart and place order.
   - System should validate stock before fulfillment.

6. **Order splitting across dark stores**
   - If one dark store cannot fulfill full order, system should split order across nearby stores.
   - Each store slice should be assigned to a separate delivery partner/agent.

7. **Unfulfilled reporting**
   - If some SKUs remain unavailable after split, system should print unfulfilled SKU quantities.

8. **Delivery fee**
   - System should calculate dynamic delivery fee using distance and surge multiplier.

9. **Order lifecycle**
   - System should support order state transitions:
     - PLACED
     - CONFIRMED
     - PACKING
     - OUT_FOR_DELIVERY
     - DELIVERED
     - CANCELLED

## Non-Functional Requirements

1. Inventory operations should be fast for in-memory usage.
2. Components should be modular and easy to extend (Strategy + pluggable store).
3. Product creation should be centralized (factory).
4. Design should be deterministic and interview-friendly.

## Assumptions

1. Payment and live GPS tracking are out of scope.
2. Real DB integration is out of scope (DB store is simulated via interface).
3. Single-process execution model is sufficient.

## Implementation map (modular project)

| Requirement | Module |
| ----------- | ------ |
| Inventory add/remove | `inventory/Inventory.h` |
| Replenishment strategies | `inventory/ReplenishStrategy.h` |
| Store backends | `inventory/Inventory.h` (`InventoryStoreFactory`) |
| 5 KM catalog | `core/BlinkitSystem.h` (`DarkStoreManager::showCatalogWithinRadius`) |
| Split order + partners | `core/BlinkitSystem.h` (`OrderManager::placeOrder`) |

> **Note:** `C++ Code/ZeptoClone.cpp` is preserved as legacy monolithic reference and is not modified.
