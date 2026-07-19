# Design Patterns Used in Leave Request System LLD

Leave Request System LLD system me leave approvals routing, processing hierarchies, aur client coordination ko modular rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Chain of Responsibility Design Pattern

### Concept
Chain of Responsibility pattern requests ko processing handlers ki ek sequential chain/link me pass karta hai. Har handler decide karta hai ki request ko process karna hai ya chain me aage next handler ko pass karna hai.

### Implementation
- [LeaveApprovalHandler](file:///Users/shubham/Desktop/LLD/Leave_Request_System_LLD/handlers/LeaveApprovalHandler.h) base handler interface hai jo `setNext` aur `processRequest` methods define karta hai.
- Approvals routing chain is sequence me link ki gayi hai:
  `TeamLeadHandler` $\rightarrow$ `ManagerHandler` $\rightarrow$ `DirectorHandler` $\rightarrow$ `HRHandler`
- Har level ke approval limits set hain (e.g. Team Lead $\le$ 3 days, Manager $\le$ 7 days, etc.). Agar leave days limit limit se zyada hai, toh check automatically next higher handler ko delegate ho jata hai.

---

## 2. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple modules aur services ke dynamic inter-connections ko wrap karke client ke samne ek single simple controller interface deta hai.

### Implementation
- [LeaveRequestSystem](file:///Users/shubham/Desktop/LLD/Leave_Request_System_LLD/core/LeaveRequestSystem.h) class is system ka main **Facade** class hai.
- Client applications direct multiple handlers linking, employee registries lookup, ya leaves balance services check me interface nahi karti. Wo direct Facade class ke standard inputs call karti hai (jaise `submitLeaveRequest`, `addEmployee`).
