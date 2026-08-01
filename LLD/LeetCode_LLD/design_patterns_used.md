# Design Patterns Used in LeetCode LLD

LeetCode LLD system me code compilation sandbox execution, problems catalog management, submissions evaluation, aur user stats coordination ko decoupled rakhne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern sub-systems aur components (jaise parsing, compilation, grading, rankings) ki internal complexities ko wrap karke client applications (jaise CLI or Web console) ko ek simplified standard unified API interface deta hai.

### Implementation
- [LeetCodeSystem](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/core/LeetCodeSystem.h) class main facade implementation hai.
- Client applications direct compilation engine (`MockCodeRunner`), validation unit (`JudgeService`), catalogue search (`ProblemCatalogService`), or rankings analytics (`LeaderboardService`) se direct link nahi karti.
- Client sirf Facade methods call karta hai:
  - `submitSolution(...)`
  - `searchProblemsByTag(...)`
  - `getLeaderboard(...)`
  - `registerUser(...)`

---

## 2. Strategy Design Pattern

### Concept
Strategy design pattern algorithms aur execution engines ko dynamic abstraction models (`interface`) me decouple karta hai taaki standard application runtime par strategies easily badli ja sakein.

### Implementation
- [ICodeRunner](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/services/ICodeRunner.h) interface compile-and-execute algorithm ki strategy specification define karta hai.
- [MockCodeRunner](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/services/MockCodeRunner.h) class is execution strategy ka mocking concrete instance hai.
- Is decoupling ke chalte, agar future me real compilers inject karne hon (jaise C++, Java, Python containerized compilers), toh hum bina evaluation pipeline code ko touch kiye dynamic compiler strategies apply kar sakte hain.

---

## 3. Separation of Concerns / Modular Services

### Concept
System requirements ko standalone single-focus tasks services layers me distribute kiya gaya hai.

### Implementation
- **Catalogue management**: Problems list aur difficulty searches [ProblemCatalogService](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/services/ProblemCatalogService.h) manage karti hai.
- **Auditing**: User submissions history, unique ID creation, aur results tracking [SubmissionService](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/services/SubmissionService.h) handle karti hai.
- **Leaderboard**: Solve counts, ratings sorting, aur users stats evaluation [LeaderboardService](file:///Users/shubham/Desktop/LLD/LeetCode_LLD/services/LeaderboardService.h) manage karti hai.
