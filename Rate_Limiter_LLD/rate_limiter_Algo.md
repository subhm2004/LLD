# Detailed Explanation: Rate Limiter & Algorithms

## Rate Limiter Kya Hota Hai?

Rate Limiter ek control mechanism hai jo ye decide karta hai ki ek user ya client ek specific time period me kitni requests bhej sakta hai.

**Asli Zindagi ka Example:**
Jaise LeetCode par agar aap "Run" button baar-baar click karte ho, toh ek point ke baad message aata hai "You are submitting too frequently." Ye Rate Limiter hi hai jo server ko spamming aur overload se bachata hai.

---

## 4 Main Algorithms ki Detail

### 1. Token Bucket Algorithm

Ye sabse zyada use hone wala algorithm hai. Isme ek "bucket" hoti hai jisme tokens bhare jaate hain.

- **Kaise kaam karta hai:** Har request ke liye bucket se 1 token nikalna padta hai. Agar bucket khali hai, toh request block ho jati hai.
- **Refill Rate:** Ek fixed interval (jaise har 4 ghante) ke baad bucket me naye tokens add hote rehte hain.
- **Fayda:** Ye "burst traffic" (ek saath aayi bohot saari requests) ko handle kar sakta hai jab tak bucket me tokens hain.

### 2. Fixed Window Counter

Isme time ko fixed slots (windows) me baant diya jata hai (jaise 1:00 to 1:01).

- **Kaise kaam karta hai:** Har window ke liye ek counter hota hai. Agar counter limit cross kar jaye, toh us window me baaki requests block ho jati hain.
- **Nuksan:** Window ke edges par problem hoti hai. Agar koi 1:00:59 par 10 request kare aur 1:01:01 par phir 10, toh sirf 2 seconds me 20 requests chali jayengi, jo server par load badha sakta hai.

### 3. Sliding Window Log

Ye Fixed Window ki "edge problem" ko solve karta hai.

- **Kaise kaam karta hai:** Ye har request ka exact **timestamp** store karta hai (usually ek Queue ya Sorted Set me). Jab nayi request aati hai, ye purani requests (jo window ke bahar hain) ko remove kar deta hai aur bachi hui requests count karta hai.
- **Fayda:** Bohot accurate hai.
- **Nuksan:** Bohot memory consume karta hai kyunki ise har request ka timestamp save karna padta hai.

### 4. Sliding Window Counter

Ye Fixed Window aur Sliding Window Log ka hybrid version hai.

- **Kaise kaam karta hai:** Ye poora log store karne ki jagah pichli window ke count aur current window ke weightage ko use karke ek approximation nikalta hai.
- **Fayda:** Memory kam leta hai aur "edge problem" ko bhi kafi had tak handle kar leta hai.

---

## Implementation Tips for LLD

1.  **Factory Pattern:** Alag-alag algorithms (Token Bucket, Fixed Window) ko create karne ke liye Factory pattern ka use karein.
2.  **Strategy Pattern:** Runtime par algorithm switch karne ke liye Strategy pattern best hai.
3.  **Thread Safety:** Java me `ConcurrentHashMap` aur C++ me `std::mutex` ka use karein takki simultaneous requests sahi se handle hon.
