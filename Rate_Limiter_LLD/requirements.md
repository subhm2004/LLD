# Requirements Gathering: Rate Limiter Design (LLD)

Is document me Rate Limiter system ki functional aur non-functional requirements ko detail me samjhaya gaya hai.

## 1. Functional Requirements

Ye requirements batati hain ki system ko kaam kya karna hai.

- **User-Based Rate Limiting:**
  - System ko har user ko unki **User ID** ke basis par limit karna chahiye.
  - Limits user ke **Tier** (Subscription Plan) par depend karengi:
    - **Free Tier:** Tight limits (Example: 2 requests per 10 seconds).
    - **Premium Tier:** Loose limits (Example: 10 requests per 10 seconds).
- **Multiple Algorithm Support:**
  - System ko kam se kam ye 4 algorithms support karne chahiye:
    1.  **Token Bucket**
    2.  **Fixed Window**
    3.  **Sliding Window Log**
    4.  **Sliding Window Counter**
- **Extensibility:**
  - Design aisa hona chahiye ki future me naye algorithms (jaise 5th type) bina purane code ko modify kiye add kiye ja sakein (Open-Closed Principle).
- **Action on Limit Exceeded:**
  - Agar user limit cross karta hai, toh system ko request block karni chahiye aur **HTTP 429 (Too Many Requests)** error return karna chahiye.

## 2. Non-Functional Requirements

Ye requirements system ki quality aur performance ko define karti hain.

- **High Efficiency & Low Latency:**
  - Rate limiter ko bohot fast hona chahiye. Agar actual API 200ms leti hai aur limiter 2s le raha hai, toh ye bura user experience hai. Iska overhead negligible hona chahiye.
- **Thread Safety:**
  - System ko multi-threaded environment me accurate hona chahiye. Agar ek hi user ek saath (simultaneously) multiple requests bheje, toh race conditions nahi honi chahiye aur counting sahi honi chahiye.
- **Accuracy:**
  - Rate limiting precision ke saath honi chahiye takki koi user defined limits ko bypass na kar sake.

## 3. Design Assumptions & Scale

- System memory-efficient hona chahiye kyunki har user ka data store karna padega.
- `ConcurrentHashMap` aur `Atomic` variables ka use karke concurrency handle ki jayegi.
