# Design Patterns Used in OTP Generation System LLD

OTP Generation LLD system me dynamic algorithms generate karne, multiple notification distribution channels coordinate karne, aur clients interactions manage karne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Strategy Design Pattern (Double Strategy Implementation)

### Concept
Strategy design pattern interchangeable classes interfaces define karta hai taaki runtime par requirements ke according algorithms select kiye ja sakein.

### Implementation
Is system me do separate strategy interfaces use kiye gaye hain:
1. **OTP Generation Strategy**:
   - [IOTPGeneratorStrategy](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/strategies/IOTPGeneratorStrategy.h) abstract base generator strategy define karta hai.
   - [NumericOTPGenerator](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/strategies/NumericOTPGenerator.h) (only numbers OTP) aur [AlphanumericOTPGenerator](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/strategies/AlphanumericOTPGenerator.h) (numbers + alphabets OTP) concrete algorithm implementations hain.
2. **Notification Delivery Channel Strategy**:
   - [INotificationChannel](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/channels/INotificationChannel.h) delivery channels specification strategy interface define karta hai.
   - [SMSChannel](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/channels/SMSChannel.h) aur [EmailChannel](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/channels/EmailChannel.h) concrete message dispatcher strategies hain.

---

## 2. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple modular classes ko wrap karke client controllers ke samne single unified API entrance provide karta hai.

### Implementation
- [OTPGenerationSystem](file:///Users/shubham/Desktop/LLD/OTP_Generation_System_LLD/core/OTPGenerationSystem.h) class main facade implementation hai.
- Client application database states check, generators switching, or channel selection manually manage nahi karti. Wo direct facade ke simplified methods call karti hai:
  - `generateOTP(...)`
  - `verifyOTP(...)`
