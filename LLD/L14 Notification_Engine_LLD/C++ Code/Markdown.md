# Notification System: Advanced Design Patterns Documentation

Bhai, ye system ek real-world production engine jaisa hai jo multiple design patterns ko combine karke banaya gaya hai. Niche iska detailed breakdown hai:

---

## 1. High-Level Architecture
Is project mein humne ek modular notification system banaya hai. Iska main flow ye hai:
1. **Singleton** se service access hoti hai.
2. **Decorator** se message ko customize kiya jata hai (timestamp, signature).
3. **Observer** pattern se multiple components ko notify kiya jata hai.
4. **Strategy** pattern se decide hota hai ki message Email, SMS, ya Popup se jayega.

---

## 2. Design Patterns Explained

### A. Singleton Pattern
**Class:** `NotificationService`
- **Logic:** Pura system sirf ek single instance use karta hai taaki notification history aur observable state consistent rahe.
- **Why?** Taaki alag-alag modules alag-alag service create na karein, sab ek hi point par interact karein.

### B. Decorator Pattern ("Is-A" & "Has-A")
**Classes:** `INotification`, `SimpleNotification`, `TimestampDecorator`, `SignatureDecorator`
- **Concept:** Hum base notification (`SimpleNotification`) ko dynamically wrap karte hain.
- **Workflow:** - `TimestampDecorator` adds time.
    - `SignatureDecorator` adds a custom footer.
- **Benefit:** Bina base code chhede hum unlimited combinations bana sakte hain.

### C. Observer Pattern
**Classes:** `IObservable`, `NotificationObservable`, `IObserver`, `Logger`, `NotificationEngine`
- **Logic:** `NotificationObservable` "Subject" hai. Jab bhi koi naya notification aata hai, wo apne saare registered `IObservers` ko `update()` call karke bata deta hai.
- **Observers:**
    - **Logger:** Notification ko console/file mein log karta hai.
    - **NotificationEngine:** Notification ko actual delivery ke liye process karta hai.

### D. Strategy Pattern
**Classes:** `INotificationStrategy`, `EmailStrategy`, `SMSStrategy`, `PopUpStrategy`
- **Logic:** `NotificationEngine` ke paas multiple strategies ho sakti hain. Hum runtime par decide karte hain ki message kis medium se bhejna hai.
- **Why?** Kal ko agar "WhatsAppStrategy" add karni ho, toh humein purana code touch bhi nahi karna padega (Open-Closed Principle).

---

## 3. Detailed Workflow (Step-by-Step)

1. **Initialization:** `NotificationService` ka instance initialize hota hai aur `Logger` aur `NotificationEngine` ko register kiya jata hai.
2. **Creation:** Client ek `SimpleNotification` banata hai.
3. **Decoration:** Us notification ko `TimestampDecorator` aur `SignatureDecorator` ke andar wrap kiya jata hai.
4. **Trigger:** `notificationService->sendNotification()` call hota hai.
5. **Notification Flow:** - `NotificationObservable` apne list ke sabhi observers ko notify karta hai.
    - `Logger` text ko print karta hai.
    - `NotificationEngine` apni strategies (Email, SMS, etc.) loop karke content deliver kar deta hai.

---

## 4. Class Hierarchy & Structure

| Pattern | Component | Responsibility |
| :--- | :--- | :--- |
| **Singleton** | `NotificationService` | Global Access Point & History Management. |
| **Decorator** | `INotificationDecorator` | Adds extra info (Time/Sign) to the raw message. |
| **Observer** | `NotificationObservable` | Maintains list of observers and notifies changes. |
| **Strategy** | `INotificationStrategy` | Decouples the delivery logic (Email vs SMS). |

---

## 5. Implementation Benefits

* **Scalability:** Naye notification type ya naye delivery channel add karna bahut easy hai.
* **Loose Coupling:** `Logger` ko pata bhi nahi ki message Email se ja raha hai ya SMS se. Sab independently kaam kar rahe hain.
* **Memory Safety:** `INotificationDecorator` ka destructor recursive deletion handle karta hai, jisse memory leaks nahi hote.

---