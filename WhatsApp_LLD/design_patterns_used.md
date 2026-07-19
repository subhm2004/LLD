# Design Patterns Used in WhatsApp LLD

WhatsApp LLD system me message notifications processing, dynamic dispatch actions, format decorators, aur observers coordination ko highly modular aur clean rakhne ke liye niche diye gaye design patterns ka use kiya gaya hai:

---

## 1. Observer Design Pattern

### Concept
Observer pattern objects ke bich one-to-many dependency define karta hai, jahan ek object (Subject/Observable) me changes hone par sabhi dependencies (Observers) ko automatically update notification milti hai.

### Implementation
- **Observable**: [NotificationObservable](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L74) (Subject) register, remove, aur notify methods handle karta hai.
- **Observer Interface**: [IObserver](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L60) standard update method state callback represent karta hai.
- **Concrete Observers**: [LoggerObserver](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L158) aur [NotificationEngine](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L175) is details ko subscribe karte hain. Nayi notification set hone par loggers aur dispatcher engines automatically updates retrieve karte hain.

---

## 2. Decorator Design Pattern

### Concept
Decorator pattern standard code layers ko inherit kiye bina (composition ke through) runtime par objects me dynamic responsibilities and features attach karne me use hota hai.

### Implementation
- **Base Component**: [INotification](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L14) base component interface define karta hai.
- **Concrete Component**: [SimpleNotification](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L20) text value hold karta hai.
- **Decorators**: [TimestampDecorator](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L39) (message me timestamp prepend karta hai) aur [SignatureDecorator](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L49) (message ke aage signature append karta hai) dynamically notification content modify karte hain.

---

## 3. Strategy Design Pattern

### Concept
Strategy design pattern context class se independent algorithms aur message dispatch channels ko dynamic abstractions me wrap karta hai.

### Implementation
- [INotificationStrategy](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L102) dispatch logic interface define karta hai.
- Multiple concrete strategies standard algorithms implement karti hain:
  - [EmailStrategy](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L108) (Email notification dispatcher)
  - [SMSStrategy](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L119) (SMS text message dispatcher)
  - [PopUpStrategy](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L130) (In-app popup alert simulation)

---

## 4. Singleton Design Pattern

### Concept
Meyers thread-safe static global instance creation pattern.

### Implementation
- [NotificationHub](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/services/NotificationEngine.h#L137) central notifier class ko singleton banaya gaya hai. Client `NotificationHub::getInstance()` call karke single shared broker use karte hain.

---

## 5. Facade Design Pattern

### Concept
Subsystems and modules coordinator pattern.

### Implementation
- [WhatsAppSystem](file:///Users/shubham/Desktop/LLD/WhatsApp_LLD/core/WhatsAppSystem.h) class main facade framework behave karti hai, jo chat records creation, encryption controls (`EncryptionService`), deletion steps (`MessageDeletionService`), aur notification triggers ko simplified APIs me manage karti hai.
