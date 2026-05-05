# DocumentEditor.cpp - Detailed Design Analysis (Bad Design Example)

## Overview
`DocumentEditor.cpp` ek intentionally poor design example hai jahan document editor ka sara behavior ek hi class (`DocumentEditor`) me tightly coupled hai. Code ka output kaam karta hai, lekin design maintainability, extensibility aur correctness ke perspective se weak hai.

---

## Current Responsibilities in `DocumentEditor`
Single class currently ye sab handle karti hai:

1. Document data store (`vector<string> documentElements`)
2. Content type handling (text vs image infer karna)
3. Rendering logic (`renderDocument`)
4. Caching (`renderedDocument`)
5. File I/O (`saveToFile`)

Isse class ka scope bahut bada ho jata hai.

---

## How Current Logic Works

### Data Insertion
- `addText(string text)` -> direct push to `documentElements`
- `addImage(string imagePath)` -> image path bhi as plain string push hota hai

### Rendering
- `renderDocument()` lazy cache approach use karta hai:
  - Agar `renderedDocument` empty hai tabhi render karta hai
  - Har element ke liye runtime check:
    - ends with `.jpg` ya `.png` -> `[Image: ...]`
    - warna plain text
- Rendered output cache me store hota hai aur next calls par wahi return hota hai

### Save
- `saveToFile()` fixed file `document.txt` me rendered content likhta hai

---

## Why This Is "Bad Design"

## 1) SRP Violation (Single Responsibility Principle)
`DocumentEditor` ka ek hi reason to change nahi hai. Agar:
- rendering format badle
- file save mechanism badle
- new element type add ho
- cache strategy badle  
to same class edit karni padegi.

## 2) OCP Violation (Open/Closed Principle)
New element types (video, table, formula, code block, etc.) add karne ke liye `renderDocument()` me `if/else` modify karna padega. Existing code repeatedly change hoga.

## 3) Weak Domain Modeling
Text aur image dono `string` se represent ho rahe hain.  
Type-safe abstraction absent hai (`DocumentElement`, `TextElement`, `ImageElement` jaisi hierarchy nahi hai).

## 4) Fragile Type Detection
Image detection sirf file extension se ho raha hai:
- `.jpeg` support nahi
- uppercase `.JPG` fail ho sakta hai
- image URL ya binary source unsupported
- koi bhi text jo `.png` pe end ho mistakenly image treat ho sakta hai

## 5) Cache Staleness Bug
`renderedDocument` once set hone ke baad invalidate nahi hota.  
Agar render ke baad naya text/image add karein to fresh output reflect nahi hoga.

## 6) Hardcoded Output Destination
`saveToFile()` fixed `"document.txt"` me save karta hai. Caller file path choose nahi kar sakta.

## 7) Low Testability
Console output aur file write direct class me embedded hai; behavior isolation difficult hota hai.

---

## Example Failure Scenario
1. `addText("A")`
2. `renderDocument()` -> cache set: `"A\n"`
3. `addText("B")`
4. `renderDocument()` -> still `"A\n"` (stale cache)

Expected: `"A\nB\n"`  
Actual: old cached result

---

## Better Design Direction (High-Level)

Recommended decomposition:

1. `IDocumentElement` interface (polymorphic render)
2. `TextElement`, `ImageElement`, ... concrete types
3. `Document` class only stores elements
4. `Renderer` class output banaye
5. `DocumentSaver` class persistence handle kare
6. Cache optional aur properly invalidated

Is approach se:
- new element add karna easy
- logic isolated aur testable
- conditional explosion avoid hota hai

---

## Suggested Class Sketch (Conceptual)

- `class IDocumentElement { virtual string render() const = 0; }`
- `class TextElement : public IDocumentElement`
- `class ImageElement : public IDocumentElement`
- `class Document { vector<unique_ptr<IDocumentElement>> elements; }`
- `class DocumentRenderer { string render(const Document&) const; }`
- `class FileDocumentSaver { void save(const string& output, const string& path); }`

---

## Conclusion
Current implementation small demo ke liye theek hai, but production-grade design ke liye risky hai.  
Yeh file good teaching example hai to show:
- tight coupling
- low cohesion
- scalability issues
- hidden correctness bug (stale cache)

Diagram reference: `DocumentEditor.mmd`
