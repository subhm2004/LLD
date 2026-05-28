# `NoSingleton.cpp` - Super Detailed Notes

## 1) Yeh file kyun hai?

Is file ka purpose singleton banana nahi hai.  
Is file ka purpose hai **compare baseline** dena:

- normal class ka behavior kya hota hai
- object creation kitni baar ho sakti hai
- memory addresses kyu alag aati hain

Matlab: pehle "without singleton" samjho, tab singleton ka value samajh aata hai.

---

## 2) Code ka mental model

Socho class ek "machine design" hai.  
`new` call karoge to machine ka naya piece banega.

NoSingleton class me:

- constructor public hai
- koi restriction nahi
- jitni baar `new`, utni baar naya object

---

## 3) Section-wise breakdown

### A) Includes

- `#include <bits/stdc++.h>`
  - common C++ headers ka umbrella include.
  - learning/demo file ke liye okay.

### B) `using namespace std;`

- `std::cout`, `std::endl` baar-baar likhne se bachata hai.
- production code me namespace pollution avoid karte hain, but demo me acceptable.

### C) Class definition

`class NoSingleton { ... }`

Key points:

- constructor **public**
- koi static shared pointer nahi
- copy/move delete nahi

Iska implication:

- object creation free hai
- class par koi singleton contract nahi hai

### D) Constructor

Constructor message print karta hai:

`"NoSingleton Constructor called. New object created."`

Yeh runtime proof deta hai ki constructor kitni baar call hua.

### E) `main()`

1. `NoSingleton *s1 = new NoSingleton();`
   - heap pe object-1 create
2. `NoSingleton *s2 = new NoSingleton();`
   - heap pe object-2 create
3. `cout << (s1 == s2) << endl;`
   - pointers compare, addresses compare hote hain
   - expected output: `0` (false)

Cleanup:

- `delete s1;`
- `delete s2;`

good practice: heap memory release.

---

## 4) Output ka exact meaning

Typical output pattern:

1. constructor message (first object)
2. constructor message (second object)
3. `0` (addresses alag)

Interpretation:

- objects independent hain
- shared global single object nahi hai

---

## 5) Interview me kaise bolna hai

Line you can say:

> "NoSingleton example me constructor public hai aur koi static instance guard nahi hai, isliye har `new` pe alag object banta hai; yeh singleton guarantee violate karta hai."

---

## 6) Why this file is important before Singleton

Agar yeh file skip karoge to singleton ka "problem statement" miss ho jata hai:

- Problem: uncontrolled multiple objects
- Singleton solution: controlled single object

NoSingleton = "before" state  
Singleton = "after" state

---

## 7) One-line summary

`NoSingleton.cpp` dikhaata hai ki normal class me object count unlimited hota hai, aur isi issue ko solve karne ke liye singleton pattern introduce kiya jata hai.

