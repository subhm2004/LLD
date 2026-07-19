# Requirements: Elevator System

## Functional Requirements

### A. Core (base system)

1. System should support multiple elevators.
2. User can place external request from any floor with direction:
   - UP
   - DOWN
3. Passenger inside elevator can place internal destination request.
4. Scheduler should assign best available elevator for each external request.
5. Elevator should move floor-by-floor and stop at requested floors.
6. System should expose elevator state:
   - current floor
   - direction
   - idle/busy status
7. System should run simulation in ticks.

### B. Building & Movement

8. **Building limits:** building me zyada se zyada **15 floors** ho sakti hain,
   aur unhe service karne ke liye zyada se zyada **3 elevators**.
9. **Elevator movement:** elevator UP ja sakta hai, DOWN ja sakta hai, ya IDLE
   (ruka hua) reh sakta hai.
10. **Floor access:** har elevator car building ki har floor pe ruk sakti hai.

### C. Doors

11. **Door operation:** elevator ke darwaze **sirf tabhi khul sakte hain jab wo
    chal na raha ho** (IDLE state me ho). Chalti elevator ke darwaze kabhi nahi
    khulenge.
12. Passenger inside panel se darwaza **manually open/close** kar sakta hai
    (jab elevator ruka ho).

### D. Panels

13. **Outside panel:** har elevator ke bahar ek panel hoga jisme buttons honge —
    elevator ko bulane ke liye, aur UP/DOWN direction batane ke liye.
14. **Inside panel:** har elevator ke andar ek control panel hoga jisme saari
    floors ke buttons honge, aur darwaza open/close karne ke buttons.
15. **Floor panels:** har floor pe apna panel hoga elevator call karne ke liye.

### E. Displays

16. **Elevator displays:** har elevator ke **andar aur bahar** display hoga jo
    current floor aur direction dikhayega.
17. **Inside display** additionally elevator ki **capacity** bhi dikhayega
    (kitne log baithe hain / kitna weight).
18. **Floor displays:** har floor pe apna display hoga jo elevators ka status
    dikhayega.

### F. Capacity

19. **Capacity limit:** ek elevator me ek waqt me zyada se zyada **8 log** ya kul
    **680 kilogram** weight ho sakta hai — jo bhi pehle limit tak pahunche.
20. Capacity full hone par aur passengers board nahi kar sakte (overload reject).

### G. Control & Dispatch

21. **Elevator control:** system elevators ki movement, darwazon ka operation,
    aur unke status ka dhyaan rakhta hai.
22. **Smart dispatch:** jab koi passenger elevator bulaye, system **best
    elevator** chunega — is hisaab se ki elevators **kahan hain** aur **kis
    direction me ja rahe hain**.
23. **Multiple passengers:** ek saath kai passengers ek hi elevator use kar sakte
    hain, chahe wo alag floors pe ja rahe hon ya alag directions me.

## Non-Functional Requirements

1. Design must be extensible for new scheduling algorithms.
2. Components should have single responsibility.
3. Implementation should be deterministic and testable in memory.
4. Input validation for invalid floors and elevator ids.
5. Capacity aur door rules kabhi silently violate nahi hone chahiye — galat
   request saaf message ke saath reject honi chahiye.

## Assumptions

1. Time is modeled via discrete ticks, not real-time threads.
2. Emergency mode and maintenance mode are out of current scope.
3. Passenger ka weight board karte waqt diya jaata hai (na diya ho to ek average
   weight maan liya jaata hai).
4. Door apne aap band ho jaata hai jab elevator chalne lagta hai (auto-close) —
   taaki chalti elevator ke darwaze kabhi khule na rahein.

## Design Notes

- **Door rule** (`#11`) ek **invariant** hai: `door == OPEN` tabhi ho sakta hai
  jab `direction == IDLE`. Ye rule `Door` class aur `ElevatorCar::step()` dono
  jagah enforce hota hai (defense in depth).
- **Capacity rule** (`#19`) do-tarfa hai: **8 log** _ya_ **680 kg** — dono me se
  jo pehle bhar jaye. Sirf ek check karna kaafi nahi.
- **Smart dispatch** (`#22`) sirf distance nahi dekhta — **direction bhi**. Ek
  elevator jo tumhari taraf aa raha hai, wo us elevator se behtar hai jo paas to
  hai par ULTI taraf ja raha hai.
