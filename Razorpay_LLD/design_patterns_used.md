# Design Patterns Used in Razorpay LLD

Razorpay LLD system me payment checkout flows, input validations, aur entities management ko modular rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Template Method Design Pattern

### Concept
Template Method pattern ek base class algorithm ke execution steps ka structure definition (skeleton) define karta hai, aur subclasses ko steps override karne ki opportunity deta hai bina main algorithm structure ko alter kiye.

### Implementation
- [CheckoutFlowTemplate](file:///Users/shubham/Desktop/LLD/Razorpay_LLD/flows/CheckoutFlowTemplate.h) is pattern ka main implementation hai.
- `runCheckout` method checkout execution sequence ka standard step structure define karti hai:
  1. `validateInputStep` (Input validations check)
  2. `createOrderStep` (Order configuration creation)
  3. `initiatePaymentStep` (Payment gateway transaction trigger)
  4. `capturePaymentStep` (Capturing the successfully authorized payment)
  5. `onSuccess` / `onPaymentFailed` hooks
- Subclasses in virtual step functions ko custom billing structures ke according override kar sakte hain.

---

## 2. Facade Design Pattern

### Concept
Facade pattern subsystems ki complex APIs ke samne ek single simple entry point class provide karta hai taaki client integration straightforward aur easy ho sake.

### Implementation
- [RazorpayPaymentSystem](file:///Users/shubham/Desktop/LLD/Razorpay_LLD/core/RazorpayPaymentSystem.h) class main facade interface hai.
- Client applications direct multiple billing, ordering, ya idempotency services se talk nahi karti. Wo sirf `RazorpayPaymentSystem` ke endpoints (jaise `checkout`, `refund`, `verifyWebhook`) call karti hain.

---

## 3. Strategy Design Pattern

### Concept
Strategy pattern algorithms ko unke specific behaviors ke relative interfaces me dynamically divide aur select karne me use hota hai.

### Implementation
- `IPaymentMethodValidator` strategy interface define karta hai.
- Alag-alag method types (jaise NetBanking, Card details, ya UPI) validation strategies ko encapsulate karte hain aur runtime par dynamically validate hote hain (Polymorphic validation).

---

## 4. Factory Design Pattern

### Concept
Factory pattern objects creation and allocation mechanisms ko single class me decouple karne ke liye use hota hai.

### Implementation
- [PaymentMethodValidatorFactory](file:///Users/shubham/Desktop/LLD/Razorpay_LLD/factories/PaymentMethodValidatorFactory.h) method type (e.g. UPI, CARD) ke parameters receive karke right validator object create karke dynamic return karta hai.
- `EntityIdFactory` system me order IDs, payment IDs, aur refund IDs generator details instantiate karta hai.
