# E-commerce Cart + Checkout LLD

Flipkart/Amazon-style **cart → coupon → inventory hold → payment → order** flow in C++17.

## Quick run

```bash
cd Ecommerce_Cart_Checkout_LLD
./compile.sh
./ecommerce_checkout_app
```

## Flow

```mermaid
sequenceDiagram
    participant U as User
    participant C as CartService
    participant I as InventoryService
    participant P as Pricing/Coupon
    participant Pay as PaymentService
    participant O as OrderService

    U->>C: addToCart()
    U->>C: checkout()
    C->>I: reserveItems()
    C->>P: apply coupon + shipping
    C->>Pay: processPayment()
    alt success
        Pay-->>C: SUCCESS
        C->>I: commitReservation()
        C->>O: save CONFIRMED order
        C->>C: clearCart()
    else failure
        Pay-->>C: FAILED
        C->>I: releaseReservation()
        C->>O: save FAILED order
    end
```

## Interview talking points

- Why reserve inventory **before** payment? → avoid overselling during checkout window.
- Why Strategy for payment? → new rails (wallet, EMI) without changing checkout service.
- Idempotency on `clientRequestId` → safe retries from mobile/web client.
- COD vs prepaid → different `PaymentStatus` while order can still be `CONFIRMED`.

See [`problem_statement.md`](problem_statement.md) for full scope.
