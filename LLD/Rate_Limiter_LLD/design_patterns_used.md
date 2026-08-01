# Design Patterns Used in Rate Limiter LLD

Rate Limiter LLD project me extensible algorithms support aur clean decoupling ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Strategy Design Pattern

### Concept
Strategy design pattern ek family of algorithms ko define karta hai, har algorithm ko encapsulates karta hai, aur runtime par unhe interchange karne ki flexibility deta hai.

### Implementation
- [RateLimiter](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/limiter/RateLimiter.h) class ek base interface context ki tarah kaam karti hai jo abstract core validation `allowRequest` method declare karti hai.
- Teen alag concrete rate limiting algorithms ko alag strategies ki tarah implement kiya gaya hai:
  - [TokenBucketRateLimiter](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/limiter/TokenBucketRateLimiter.h)
  - [FixedWindowRateLimiter](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/limiter/FixedWindowRateLimiter.h)
  - [SlidingWindowLogRateLimiter](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/limiter/SlidingWindowLogRateLimiter.h)
- Is pattern ke chalte hum bina client logic ko target kiye runtime par user tiers ke according strategies badal sakte hain.

---

## 2. Factory Design Pattern (Factory Method)

### Concept
Factory Method pattern dynamic object creation logic ko abstract karta hai. Client code ko details objects initialization step explicitly pass nahi karne hote.

### Implementation
- [RateLimiterFactory](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/factory/RateLimiterFactory.h) class static creation method `createRateLimiter` provide karti hai.
- Factory parameter dynamic type `RateLimitType` aur setup options `RateLimitConfig` receive karti hai aur corresponding subclass (`std::unique_ptr<RateLimiter>`) runtime par instantiate karke return karti hai.

---

## 3. Facade Design Pattern

### Concept
Facade pattern sub-systems ko wrap karke client application ke samne ek single standard API point expose karta hai.

### Implementation
- [RateLimiterService](file:///Users/shubham/Desktop/LLD/Rate_Limiter_LLD/service/RateLimiterService.h) class client UI layer ya gatekeeper router ke samne Facade interface ki tarah khadi hai.
- Client ko internal factory instantiation ya concrete limit checking process lookup me involve nahi hona padta. Client direct simple method `rateLimiterService.allowRequest(user)` call karta hai aur answer evaluate karta hai.
