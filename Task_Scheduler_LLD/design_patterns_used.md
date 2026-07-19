# Design Patterns Used in Task Scheduler LLD

Task Scheduler LLD system me dynamic jobs scheduling, status updates observers, aur jobs lifecycle coordination ko decoupled rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Strategy Design Pattern

### Concept
Strategy pattern algorithms collection ko separate interface me abstract aur group karta hai taaki runtime par requirements ke base par strategy logic interchange ho sake.

### Implementation
- [ISchedulingStrategy](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/strategies/ISchedulingStrategy.h) abstract base strategy interface hai.
- Do concrete scheduling strategies is interface ko implement karti hain:
  - [FifoSchedulingStrategy](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/strategies/FifoSchedulingStrategy.h) (First-In, First-Out sequence based execution)
  - [PrioritySchedulingStrategy](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/strategies/PrioritySchedulingStrategy.h) (Job priority levels ke priority heap calculation)
- System facade method `setSchedulingStrategy` call karke strategy runtime par switch ho sakti hai.

---

## 2. Observer Design Pattern

### Concept
Observer pattern one-to-many relationship define karta hai, jahan subject state update hone par sabhi registered listeners/observers ko notify kiya jata hai.

### Implementation
- **Subject**: [JobObserverService](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/services/JobObserverService.h) (or corresponding service layer) registers and handles notifications.
- **Observer Interface**: [IJobObserver](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/observers/IJobObserver.h) main listener interface hai.
- **Concrete Observer**: [ConsoleJobObserver](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/observers/ConsoleJobObserver.h) console logs output prints execute karta hai. Job states changes (`SUBMITTED`, `RUNNING`, `SUCCESS`, `FAILED`) hone par observers automatically trigger hote hain.

---

## 3. Factory Design Pattern

### Concept
Factory pattern parameter configurations ke base par subclass allocations aur object construction logic ko decouple karta hai.

### Implementation
- [JobFactory](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/factories/JobFactory.h) class parameter types evaluation ke base par dynamic jobs (jaise SimpleJob, RecurringJob) objects instantiate karti hai.

---

## 4. Facade Design Pattern

### Concept
Facade pattern subsystems ki multiple services ko single unified entrance wrapper class me wrap kar deta hai.

### Implementation
- [TaskSchedulerSystem](file:///Users/shubham/Desktop/LLD/Task_Scheduler_LLD/core/TaskSchedulerSystem.h) class main facade implementation hai. Client directly simple methods call karta hai:
  - `submitJob(...)`
  - `startScheduler()`
  - `registerObserver(...)`
