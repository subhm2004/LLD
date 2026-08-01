# Document Editor System - LLD Guide (Strategy Pattern and Composite Pattern ka use hua hai) 📝

## 1. Problem (Asli Challenge Kya Hai?)

Jab hum ek Document Editor banate hain, toh ye problems aati hain:

- **Multiple Elements:** Document mein text, image, bold text, aur formatting elements (new line, tab) sab alag-alag hote hain. Inhe ek hi list mein manage karna mushkil hota hai.
- **Hardcoded Storage:** Agar hum editor class ke andar hi `saveToFile()` ya `saveToDB()` likh denge, toh naya storage option add karne ke liye poora code badalna padega.
- **Violation of OCP:** Naya element (jaise Italics) ya naya storage (jaise Cloud) add karne par existing classes ko modify karna padta hai, jo ganda code hai.

---

## 2. Strategy Design Pattern (Brief Note)

Is project mein humne **Strategy Pattern** ka use kiya hai, khaas karke **Persistence (Storage)** logic ke liye.

**Concept:** Humne "Saving" ke tarike ko Editor class se alag kar diya hai. Editor ko bas itna pata hai ki uske paas ek `Persistence` interface hai. Wo file mein save kar raha hai ya Database mein, ye runtime par decide hota hai. Iske saath humne **Composition** ka use kiya hai taaki `Document` objects ko flexible banaya ja sake.

---

## 3. Solution (The Architecture)

- **DocumentElement Interface:** Ek base class banayi jo har element (Text, Image, Bold) ko ek generic `render()` method deti hai.
- **Persistence Interface:** Ek interface banaya jo `save()` method define karta hai. `FileStorage` aur `DBStorage` isi ki alag-alag strategies hain.
- **Editor Decoupling:** `DocumentEditor` class ko ye farak nahi padta ki element kya hai ya storage kaisa hai. Wo bas "Render" aur "Save" commands chalata hai.

---

## 4. Detailed Workflow (Step-by-Step) 🚀

1. **Element Construction:** Jab hum `addText("Hello")` call karte hain, toh editor ek naya `TextElement` object banata hai aur use `Document` ke vector mein push kar deta hai.
2. **Rendering Stage:** Jab humein document dekhna hota hai, `Document::render()` chalta hai. Ye loop chala kar har element ka `render()` function call karta hai aur sabko ek lambi string mein jod deta hai.
3. **Storage Selection:** `main()` function mein hum decide karte hain ki kaunsa storage use karna hai (e.g., `new FileStorage()`).
4. **The Save Action:** `editor->saveDocument()` call karne par:
   - Pehle pura document render hokar ek `string` banta hai.
   - Phir wo string `storage->save(data)` ko pass kar di jati hai.
   - Agar storage `FileStorage` hai, toh file banti hai. Agar `DBStorage` hai, toh DB mein print hota hai.

---

## 5. Important Tips & Tricks 💡

- **Virtual Destructor:** `DocumentElement` aur `Persistence` dono mein `virtual ~Class()` hona zaroori hai. Kyunki hum `delete editor` kar rahe hain, agar virtual nahi hoga toh elements memory mein hi reh jayenge (**Memory Leak**).
- **Dependency Injection:** Hum `DocumentEditor` ko constructor ke through batate hain ki use kaunsa document aur kaunsa storage use karna hai. Isse testing asan ho jati hai.
- **Separation of Concerns:** Rendering ka kaam elements ka hai, storage ka kaam persistence ka hai, aur management ka kaam editor ka. Inhe kabhi mix mat karo.
- **Pro Tip (Smart Pointers):** Agar tum modern C++ use kar rahe ho, toh `std::vector<std::unique_ptr<DocumentElement>>` use karo. Isse tumhe manually `delete` karne ki zaroorat nahi padegi.
- **Feature Extension:** Agar tumhe "Bold Text" ko HTML mein dikhana hai, toh bas `BoldTextElement` ka `render()` function badal do: `return "<b>" + text + "</b>";`. Pura system waise hi kaam karega!
