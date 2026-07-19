# Design Patterns Used in Library Management System LLD

Library Management LLD system me books tracking, loans auditing, fine calculations, aur user management processes को decoupled aur structured rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern sub-systems aur standalone modules ki dynamic interactions ko single simplified unified wrapper class me wrap kar deta hai taaki standard integration simple aur scalable ho sake.

### Implementation
- [LibraryManagementSystem](file:///Users/shubham/Desktop/LLD/Library_Management_System_LLD/core/LibraryManagementSystem.h) is system ka main **Facade** interface hai.
- Client applications direct multiple modules (jaise books indexing database `CatalogService`, fine rules evaluator `FineService`, loans tracking registries, or members configuration index) se talk nahi karti.
- Client sirf Facade methods call karta hai:
  - `issueBook(...)`
  - `returnBook(...)`
  - `searchBooksByTitle(...)`
  - `registerMember(...)`

---

## 2. Delegation Pattern & Single Responsibility Principle (SRP)

### Concept
System ke domain responsibilities ko single-focus classes me divide karke, operations un custom classes par delegate kiye jate hain.

### Implementation
- **Book Catalog management**: Title indexing aur category wise lookup logic [CatalogService](file:///Users/shubham/Desktop/LLD/Library_Management_System_LLD/services/CatalogService.h) me delegate kiya gaya hai.
- **Fine Calculations**: Return date details ke according fine pricing policy evaluate karne ka logic [FineService](file:///Users/shubham/Desktop/LLD/Library_Management_System_LLD/services/FineService.h) me delegate kiya gaya hai.

---

## 3. State Management (Book & Member status transitions)

### Concept
Entity attributes updates ko strict predefined enum bounds me coordinate kiya jata hai taaki inconsistencies block ho sakein.

### Implementation
- **Book States**: Book status ([BookStatus](file:///Users/shubham/Desktop/LLD/Library_Management_System_LLD/enums/BookStatus.h)) `AVAILABLE` $\leftrightarrow$ `LOANED` changes handle karta hai.
- **Member Account States**: Member profile ([AccountStatus](file:///Users/shubham/Desktop/LLD/Library_Management_System_LLD/enums/AccountStatus.h)) checks verify hoti hain transaction authorization se pehle (active check).
