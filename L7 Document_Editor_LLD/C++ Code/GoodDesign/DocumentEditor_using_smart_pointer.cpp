// ============================================================================
//  DocumentEditor_using_smart_pointer.cpp (GoodDesign)  —  smart-pointer version
// ----------------------------------------------------------------------------
//  Ye WAHI achha design hai (polymorphic elements + Persistence strategy) jo
//  DocumentEditor.cpp me hai — par raw pointers ki jagah SMART POINTERS
//  (`unique_ptr`, `make_unique`). Design bilkul same; sirf MEMORY MANAGEMENT
//  ka tareeka behtar.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  Raw pointer version me kya dikkat thi?                                  │
//  │    - `Document` ka destructor missing tha -> saare elements LEAK hote    │
//  │      the (6 leaks / 192 bytes, test kiya tha).                           │
//  │    - Har `new` ke liye ek `delete` yaad rakhna padta tha. Ek bhoole =    │
//  │      leak. Beech me exception aaye = leak.                               │
//  │                                                                          │
//  │  Smart pointer isse kaise theek karta hai?                              │
//  │    `unique_ptr` ek "maalik" pointer hai jo SCOPE khatam hote hi apne     │
//  │    object ko KHUD delete kar deta hai. Koi manual `delete` nahi, koi     │
//  │    destructor likhne ki zaroorat nahi. Ise RAII kehte hain — "Resource   │
//  │    Acquisition Is Initialization" (resource ka jeevan object ke jeevan   │
//  │    se bandha hota hai). Ye version 0 LEAKS deta hai. ✅                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ TEEN naye C++ concept jo is file me seekhne ko milte hain:
//     1. unique_ptr<T>  -> ek "single owner" pointer, auto-delete karta hai
//     2. make_unique<T> -> object banane ka safe tareeka (`new` ki jagah)
//     3. std::move      -> ownership ek jagah se doosri jagah "transfer" karna
//  Teeno ko neeche jahan-jahan aate hain wahan detail me samjhaya hai.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory> // smart pointers (unique_ptr, make_unique) yahi header me hain

using namespace std;

// ============================================================================
// 1. POLYMORPHIC COMPONENT HIERARCHY (Open-Closed Principle - OCP)
// ============================================================================
//  Bilkul raw-pointer version jaisa — har element apna render() khud jaanta hai.
//  Koi central if-else nahi. Naya element = nayi class, purana code untouched.

// DocumentElement: sabhi elements ka BASE interface.
class DocumentElement
{
public:
    virtual string render() = 0;

    // Virtual destructor — smart pointer (`unique_ptr<DocumentElement>`) jab
    // object delete karega, wo BASE type ke through karega. Isi liye ye virtual
    // hona zaroori hai, warna derived (TextElement) ka destructor chalta hi nahi.
    // Smart pointer memory KHUD sambhalta hai, par "kaunsa destructor chale" ye
    // phir bhi virtual keyword pe depend karta hai.
    virtual ~DocumentElement()
    {
        // Yahan chaaho to cleanup/logging daal sakte ho — smart pointer isko
        // automatically sahi waqt pe bulata hai.
    }
};

// TextElement: aam plain text.
class TextElement : public DocumentElement
{
private:
    string text;

public:
    // ⭐ Initializer-list (`: text(t)`) — member ko seedha construct karta hai
    // (raw version me `this->text = text` body-assignment tha). Ye thoda behtar
    // aur modern C++ style hai.
    TextElement(string t) : text(t) {}
    string render() override {
      return text;
    }
};

// BoldTextElement: markdown bold (**text**). OCP ka saboot — nayi class, purana
// code untouched.
class BoldTextElement : public DocumentElement
{
private:
    string text;

public:
    BoldTextElement(string t) : text(t) {}
    string render() override {
      return "**" + text + "**";
    }
};

// ImageElement: image path ka reference.
class ImageElement : public DocumentElement
{
private:
    string imagePath;

public:
    ImageElement(string path) : imagePath(path) {}
    string render() override {
      return "[Image: " + imagePath + "]";
    }
};

// NewLineElement: line break. Koi data nahi, fixed output.
class NewLineElement : public DocumentElement
{
public:
    string render() override {
      return "\n";
    }
};

// TabSpaceElement: tab (gap). Koi data nahi, fixed output.
class TabSpaceElement : public DocumentElement
{
public:
    string render() override {
      return "\t";
    }
};

// ============================================================================
// 2. DOCUMENT CONTAINER (Aggregator using std::unique_ptr)
// ============================================================================

// Document: elements ki list sambhaalta hai. Raw version se ANTAR yahi hai —
// list `unique_ptr<DocumentElement>` ki hai, `DocumentElement*` ki nahi.
class Document
{
private:
    // ⭐ `vector<unique_ptr<DocumentElement>>` — ye vector apne elements ka
    // MAALIK hai. Jab ye vector destroy hota hai (Document ke saath), to har
    // unique_ptr apne element ko KHUD delete kar deta hai. Yahi wajah hai ki
    // raw version wala leak yahan hai hi nahi. Ye single-ownership model hai:
    // har element ka theek EK maalik.
    vector<unique_ptr<DocumentElement>> documentElements;

public:
    // ⭐ addElement `unique_ptr` BY VALUE leta hai, aur `std::move` se use vector
    // me daalta hai. Ye samajhna zaroori hai:
    //   - unique_ptr COPY nahi ho sakta (do maalik allowed nahi — warna dono
    //     delete karte, double-free ho jaata). Isi liye normal assignment
    //     compile hi nahi hoga.
    //   - `std::move` ownership ko "transfer" (khiskata) karta hai: caller ka
    //     pointer khaali ho jaata hai, aur vector naya maalik ban jaata hai.
    void addElement(unique_ptr<DocumentElement> element)
    {
        documentElements.push_back(std::move(element));
    }

    // render: raw version jaisa hi — har element ka render() jod do.
    // `const auto&` -> reference se ghumo, unique_ptr ko copy karne ki koshish
    // mat karo (wo ban hi nahi sakti).
    string render()
    {
        string result;
        for (const auto &element : documentElements)
        {
            result += element->render();
        }
        return result;
    }

    // ⭐ Yahan koi `~Document()` destructor NAHI likhna pada — aur ye khoobsurti
    // hai! Jab Document scope se bahar jaata hai, uska `documentElements` vector
    // apne aap destroy hota hai, aur har unique_ptr apna element khud delete kar
    // deta hai. Raw version me hume manual destructor likhna padta (jo wo bhool
    // gaya tha -> leak). Yahan galti karne ki gunjaish hi nahi. ✅
};

// ============================================================================
// 3. STRATEGY DESIGN PATTERN (Persistence)
// ============================================================================
//  Bilkul same idea — save "kaha" karna ek swappable strategy hai.

// Persistence: strategy ka base interface.
class Persistence
{
public:
    virtual void save(string data) = 0;
    virtual ~Persistence() {} // virtual destructor — smart pointer safe delete
};

// FileStorage: file me save.
class FileStorage : public Persistence
{
public:
    void save(string data) override
    {
        ofstream outFile("document.txt");
        if (outFile.is_open())
        {
            outFile << data;
            outFile.close();
            cout << "Document saved to document.txt" << endl;
        }
    }
};

// DBStorage: DB me save (simulate).
class DBStorage : public Persistence
{
public:
    // `data` param yahan use nahi hota (simulate kar rahe) -> compiler warning.
    void save(string data) override
    {
        cout << "Document saved to DB (Simulated)" << endl;
    }
};

// ============================================================================
// 4. EDITOR CONTROLLER (Strategy Context)
// ============================================================================

// DocumentEditor: sabko jodne wala conductor (facade). Yahan OWNERSHIP ka ek
// interesting mix hai — dhyaan se dekho.
class DocumentEditor
{
private:
    // ⭐ DO alag ownership styles, jaan-boojh ke:
    //
    //   Document *document;  -> RAW pointer. Editor document ka MAALIK NAHI hai,
    //                           bas use "dekhta" hai (weak association). Document
    //                           ka jeevan main() sambhalta hai (myDocument). Isi
    //                           liye editor ise delete NAHI karta.
    //
    //   unique_ptr<Persistence> storage; -> editor storage ka MAALIK hai. Jab
    //                           editor khatam hoga, storage apne aap delete.
    //
    // Ye farak important hai: "kya main is cheez ka maalik hu, ya bas use kar
    // raha hu?" — smart pointer ye baat SIGNATURE me hi saaf kar deta hai.
    // (Raw version me ye baat pata hi nahi chalti thi — sab `T*` jaisa dikhta.)
    Document *document;
    unique_ptr<Persistence> storage;

public:
    // Constructor: document ka pointer (borrow) leta hai, aur Persistence ka
    // OWNERSHIP `std::move` se apne andar transfer kar leta hai.
    DocumentEditor(Document *doc, unique_ptr<Persistence> s)
    {
        this->document = doc;
        this->storage = std::move(s); // ownership editor ke paas aa gaya
    }

    // ---- Add-* helpers: `make_unique` se element banate hain ----------------
    // ⭐ `make_unique<TextElement>(text)` = `new TextElement(text)` ka SAFE,
    // modern replacement. Ye seedha ek `unique_ptr<TextElement>` deta hai. Faayde:
    //   - kabhi "delete bhoolna" ki problem nahi
    //   - exception-safe (agar beech me kuch fail ho to leak nahi hota)
    //   - `new` keyword aankhon se gayab -> code saaf
    void addText(string text)
    {
        document->addElement(make_unique<TextElement>(text));
    }

    void addBoldText(string text)
    {
        document->addElement(make_unique<BoldTextElement>(text));
    }

    void addImage(string path)
    {
        document->addElement(make_unique<ImageElement>(path));
    }

    void addNewLine()
    {
        document->addElement(make_unique<NewLineElement>());
    }

    void addTabSpace()
    {
        document->addElement(make_unique<TabSpaceElement>());
    }

    // ⭐ setStorage: runtime pe strategy BADALNE ka tareeka. Naya storage
    // `std::move` se andar aata hai — aur purana storage AUTO-DELETE ho jaata
    // hai! Kyun? Kyunki `this->storage` (unique_ptr) me naya assign hone se wo
    // apne purane object ko khud release kar deta hai. Manual delete ki zaroorat
    // hi nahi. (Raw version me purana storage delete karna khud yaad rakhna padta.)
    //
    // Note: raw version me ye method tha hi nahi — wahan naye storage ke liye ek
    // poora NAYA editor banana padta tha (dbEditor). Yahan same editor me strategy
    // badal sakte hain — zyada saaf.
    void setStorage(unique_ptr<Persistence> newStorage)
    {
        this->storage = std::move(newStorage);
    }

    // saveDocument: render karke strategy ko de do.
    // Note: raw version wala render-CACHE yahan nahi hai — har baar fresh render
    // hota hai. Isse wo "stale cache" wala bug bhi yahan nahi hai. (Simpler aur
    // correct — trade-off: har save pe dobara render.)
    void saveDocument()
    {
        storage->save(document->render());
    }
};

// ============================================================================
// 5. CLIENT DRIVER (main)
// ============================================================================
int main()
{
    // ⭐ `make_unique<Document>()` -> `unique_ptr<Document>`. Ye `myDocument`
    // document ka MAALIK hai; main() ke khatam hote hi apne aap delete ho jaayega.
    auto myDocument = make_unique<Document>();

    // FileStorage strategy banao, aur editor ko do.
    auto fileStore = make_unique<FileStorage>();

    // ⭐ `myDocument.get()` -> unique_ptr ke andar ka RAW pointer nikaalta hai
    // (ownership TRANSFER kiye bina — "bas dekhne ke liye udhaar"). Editor
    // document ko borrow kar raha hai, maalik nahi ban raha. Maalik `myDocument`
    // hi rahega. Isi liye editor document ko delete nahi karega.
    //
    // `std::move(fileStore)` -> storage ka OWNERSHIP editor ko de diya. Iske baad
    // `fileStore` khaali (null) ho jaata hai — use dobara mat use karna.
    DocumentEditor editor(myDocument.get(), std::move(fileStore));

    // Content add karo.
    editor.addText("Hello C++!");
    editor.addNewLine();
    editor.addBoldText("Smart Pointers are great.");
    editor.addTabSpace();
    editor.addImage("tech.png");

    cout << "--- Attempting File Save ---" << endl;
    editor.saveDocument();

    // ⭐ RUNTIME STRATEGY SWAP: raw version me naya editor banana padta tha; yahan
    // bas storage badal do. Naya DBStorage andar move ho jaata hai, aur purana
    // FileStorage apne aap delete ho jaata hai (unique_ptr ka kamaal).
    cout << "\n--- Switching to DB and Saving ---" << endl;
    editor.setStorage(make_unique<DBStorage>());
    editor.saveDocument();

    return 0;
    // ⭐ Yahan ek bhi `delete` nahi! Sab kuch RAII se apne aap saaf ho jaata:
    //   - `editor` scope-end pe destroy -> uska `storage` (unique_ptr) delete
    //   - `myDocument` destroy -> uska elements-vector delete -> har element delete
    // Isi liye ye version 0 LEAKS deta hai. Yahi smart pointers ki asli taakat.
}
