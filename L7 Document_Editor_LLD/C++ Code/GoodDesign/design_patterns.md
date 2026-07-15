# Document Editor: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L7 Document_Editor_LLD/C++ Code/GoodDesign` folder ke files me use hone wale sabhi design patterns ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Document Editor system ko clean, highly maintainable, extensible, aur SOLID principles aligned banane ke liye do key structural & behavioral patterns ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Document Editor App |
| :--- | :--- | :--- |
| **1. Strategy Pattern** | Behavioral | Document saving persistence logic (jaise File saving, database updates, cloud integrations) ko interchange karne ke liye. |
| **2. Polymorphic Component Hierarchy (Composite Foundation)** | Structural / SOLID | Har element type (`Text`, `Image`, `Bold`, etc.) ko self-contained dynamic subclasses me model karne ke liye taaki Open-Closed Principle (OCP) violate na ho. |

---

## Detailed Analysis of Design Patterns

### 1. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Document saving behavior platform-dependent ya configuration-dependent ho sakta hai. Agar hum direct code me file writing logic likhte to database integration add karne ke liye context class ko modify karna padta (violation of Single Responsibility and Open-Closed Principle). 

Strategy pattern persistence algorithms ko separate strategy classes me encapsulate kar deta hai aur context class (`DocumentEditor`) ko standard abstraction run-time inputs inject validation triggers reference deta hai.

#### **Implementation in Code:**
* **Strategy Interface:** [Persistence](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L125-L131) base abstract coordinates verification functions define karta hai.
* **Concrete Strategies:** 
  1. [FileStorage](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L134-L151) save output parameters file local write coordinates pe dump karta hai.
  2. [DBStorage](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L154-L162) dynamic simulated query values updates handle karta hai.
* **Strategy Context:** [DocumentEditor](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L165-L219) strategy targets inject mapping store reference handle karta hai.

```cpp
// Strategy Interface (Line 125-131)
class Persistence {
public:
    virtual void save(string data) = 0;
    virtual ~Persistence() {}
};

// Context Class execution trigger callback (Line 215-218)
void saveDocument() {
    storage->save(renderDocument()); // Delegates to active Strategy
}
```

* **Smart Pointer Version Verification:** [DocumentEditor_using_smart_pointer.cpp](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor_using_smart_pointer.cpp#L173-L176) me explicit strategy swapper functions implementations check easily readable hain:
```cpp
void setStorage(unique_ptr<Persistence> newStorage) {
    this->storage = std::move(newStorage); // Swaps strategy object at runtime.
}
```

---

### 2. Polymorphic Component Hierarchy (Open-Closed Principle)
#### **Kyu use kiya gaya? (Intent)**
BadDesign me har class string conditions checking run loops coordinates use karti thhi, jisse conditional matching complex aur error-prone ho jati thhi. 

GoodDesign me humne har unique formatting objects ko polymorphic interfaces subclasses me extract kiya hai, jahan har element self-render calculations logic dynamic functions overrides ke andur encapsulate rakhta hai. Isse system extensibility increase hoti hai aur dynamic checks dependencies bypass ho jati hain.

#### **Implementation in Code:**
* **Component Interface:** [DocumentElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L19-L25) interface coordinates base render templates rules.
* **Concrete Components subclasses:** [TextElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L28-L43), [ImageElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L45-L61), [BoldTextElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L83-L98), [NewLineElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L64-L71), and [TabSpaceElement](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L74-L81).
* **Composition Manager:** [Document](file:///Users/shubham/Desktop/LLD/L7%20Document_Editor_LLD/C++%20Code/GoodDesign/DocumentEditor.cpp#L101-L122) class runs loop over element instances:

```cpp
// Composition Rendering Iteration Loop (Line 113-121)
string render() {
    string result;
    for (auto element : documentElements) {
        result += element->render(); // Polymorphic rendering callback coordinates lookup.
    }
    return result;
}
```

---
*Created by Antigravity*
