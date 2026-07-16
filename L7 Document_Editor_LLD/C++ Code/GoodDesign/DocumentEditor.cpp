// ============================================================================
//  DocumentEditor.cpp (GoodDesign)  —  Document Editor ka SAHI design
// ----------------------------------------------------------------------------
//  Ye ek chhota "document editor" hai: tum text, bold text, image, newline, tab
//  add karte ho, aur phir poore document ko render (string me convert) karke
//  kahin SAVE kar dete ho (file ya DB).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  BadDesign me kya galat tha? (BadDesign/ folder me poora hai)            │
//  │    - Saare elements ek `vector<string>` me pade rehte the.               │
//  │    - Render karte waqt HAR string pe check: "ye image path hai? bold?    │
//  │      newline?" -> lambe if-else / string-parsing loops. 🤮               │
//  │    - Naya element type (jaise Bold) add karo -> us render loop ko JAAKE   │
//  │      EDIT karna padta. Ek jagah bhool gaye = bug.                        │
//  │    - Saving ka logic bhi editor ke andar hardcoded -> DB add karna ho to │
//  │      editor ko cheerna padta.                                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ GOOD DESIGN 2 cheezein theek karta hai (2 principles + 1 pattern):
//
//   1. OCP (Open/Closed Principle) — POLYMORPHISM se:
//      Har element apni CLASS hai (TextElement, BoldTextElement...) jo apna
//      `render()` KHUD jaanta hai. Koi central if-else nahi. Naya element =
//      nayi class, purana code CHHUNA nahi padta. "Extension ke liye khula,
//      modification ke liye band."
//
//   2. SRP (Single Responsibility Principle) — kaam baant diye:
//      - DocumentElement subclasses -> "main khud ko kaise render karu"
//      - Document                    -> "elements ki list sambhaalu"
//      - Persistence subclasses      -> "data kaha save karu"
//      - DocumentEditor              -> "sabko orchestrate karu" (facade)
//      Har class ka EK kaam.
//
//   3. STRATEGY PATTERN — saving ke liye:
//      Save "kaha" karna hai (file/DB/cloud) wo ek Persistence STRATEGY hai jo
//      editor me INJECT hoti hai. Runtime pe badli ja sakti hai.
//
//  ⚠ Ye "raw pointer" version hai — `new` khud karta hai. Ismein ek MEMORY LEAK
//    hai (neeche Document class me detail). Uska saaf fix
//    "DocumentEditor_using_smart_pointer.cpp" me hai (shared/unique_ptr se).
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// ============================================================================
// 1. POLYMORPHIC COMPONENT HIERARCHY (Open-Closed Principle - OCP)
// ============================================================================
//  Har document element ek DocumentElement subclass hai jo apna render() khud
//  jaanta hai. Isi wajah se koi central "ye kaunsa element hai" wala if-else
//  nahi likhna padta — har element khud batata hai wo kaise dikhega.

// DocumentElement: sabhi elements (Text, Image, Bold, NewLine, Tab) ka BASE
// interface. Ye "contract" hai — har element ko render() dena PADEGA.
class DocumentElement
{
public:
    // render(): element ko string me badalta hai. Pure virtual (`= 0`) -> is
    // class ka apna koi implementation nahi, har subclass ko likhna PADEGA.
    virtual string render() = 0;

    // Virtual destructor — Document `DocumentElement*` (base pointer) me elements
    // rakhta hai, par asal object TextElement waghairah hai. Base pointer se
    // delete karne pe derived ka destructor tabhi chalega jab ye virtual ho.
    // 📌 Rule: ek bhi virtual function hai to destructor bhi virtual hona chahiye.
    virtual ~DocumentElement() {}
};

// TextElement: aam plain text ("Hello, world!").
class TextElement : public DocumentElement
{
private:
    string text; // user ka likha hua text

public:
    TextElement(string text)
    {
        this->text = text;
    }

    // Plain text jaisa hai waisa hi return — koi formatting nahi.
    string render() override
    {
        return text;
    }
};

// ImageElement: ek image ka reference (path).
class ImageElement : public DocumentElement
{
private:
    string imagePath; // image kaha padi hai

public:
    ImageElement(string imagePath)
    {
        this->imagePath = imagePath;
    }

    // Path ko tags ke andar wrap karke dikhata hai: [Image: picture.jpg]
    string render() override
    {
        return "[Image: " + imagePath + "]";
    }
};

// NewLineElement: ek line break. Notice — iske paas koi DATA nahi (koi field
// nahi), bas ek fixed behavior. Aisi class ka ek hi object kaafi hota (L30
// Flyweight yaad karo), par yahan har baar naya banta hai (chhoti cheez).
class NewLineElement : public DocumentElement
{
public:
    string render() override
    {
        return "\n";
    }
};

// TabSpaceElement: ek tab (gap). NewLine jaisa hi — no data, fixed output.
class TabSpaceElement : public DocumentElement
{
public:
    string render() override
    {
        return "\t";
    }
};

// BoldTextElement: bold text (markdown style **...**).
// ⭐ OCP KA ASLI SABOOT: ye class BAAD me add hui hogi, aur isse koi purana code
// CHHUNA nahi pada — na Document, na DocumentEditor ka render loop, kuch nahi.
// Bas ek nayi class banayi aur DocumentEditor me ek addBoldText() jodi. Bilkul
// yahi OCP chahta hai: naye feature ke liye code JODO, purana BADLO mat.
class BoldTextElement : public DocumentElement
{
private:
    string text;

public:
    BoldTextElement(string text)
    {
        this->text = text;
    }
    string render() override
    {
        return "**" + text + "**"; // markdown bold: **text**
    }
};

// ============================================================================
// 2. DOCUMENT CONTAINER (Aggregator Class)
// ============================================================================

// Document: elements ki LIST rakhta hai aur unhe jod ke poora document render
// karta hai. Iska ek hi kaam hai — collection sambhalna (SRP). Ye NAHI jaanta
// ki element kaise render hota (wo element ka kaam), aur NAHI jaanta ki save
// kaise hota (wo Persistence ka kaam).
class Document
{
private:
    // Elements ke pointers ki list. Base pointer (`DocumentElement*`) me rakhe
    // hain, isi liye Text/Image/Bold sab ek hi list me aa jaate hain.
    vector<DocumentElement *> documentElements;

public:
    // List ke aakhir me naya element jodo.
    void addElement(DocumentElement *element)
    {
        documentElements.push_back(element);
    }

    // ⭐ POLYMORPHISM yahan chamakta hai: bas har element ka render() bulao aur
    // jod do. Document ko pata bhi nahi ki ye Text hai ya Bold ya Image — har
    // element khud jaanta hai. Ek loop, saare element types handle. Koi if-else
    // nahi. (BadDesign me yahi jagah string-checking ka jungle thi.)
    string render()
    {
        string result;
        for (auto element : documentElements)
        {
            result += element->render(); // sahi subclass ka render() apne aap chalega
        }
        return result;
    }

    // ⚠⚠ MEMORY LEAK — YAHAN DESTRUCTOR MISSING HAI!
    //   Document `new TextElement(...)` waghairah ke pointers rakhta hai, par
    //   uska koi `~Document()` nahi jo unhe `delete` kare. Jab main() me
    //   `delete document` hota hai, to sirf vector khatam hota hai — uske andar
    //   ke pointed-to element objects LEAK ho jaate hain.
    //   (Test kiya: 6 elements -> 6 leaks / 192 bytes.)
    //
    //   Hona chahiye tha:
    //       ~Document() {
    //           for (auto element : documentElements) delete element;
    //       }
    //   📌 Rule: jo container RAW pointers ka maalik hai, uska destructor unhe
    //      delete kare. Ya isse behtar — `unique_ptr` use karo, tab ye jhanjhat
    //      hi khatam. Wahi "DocumentEditor_using_smart_pointer.cpp" me kiya hai
    //      (wo version 0 leaks deta hai).
};

// ============================================================================
// 3. STRATEGY DESIGN PATTERN (Document Storage / Persistence)
// ============================================================================
//  "Data KAHA save karna hai" — file, DB, cloud — ye ek SWAPPABLE strategy hai.
//  Editor ko sirf ek `Persistence*` chahiye; use farak nahi padta ki andar
//  FileStorage hai ya DBStorage. Runtime pe badla ja sakta hai.

// Persistence: strategy ka BASE interface. Sirf ek kaam: save(data).
class Persistence
{
public:
    virtual void save(string data) = 0;
    virtual ~Persistence() {} // virtual destructor — base pointer se delete safe
};

// FileStorage: strategy #1 — data ko local file (document.txt) me likhta hai.
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
        else
        {
            cout << "Error: Unable to open file for writing." << endl;
        }
    }
};

// DBStorage: strategy #2 — DB me save (yahan sirf simulate, actual DB nahi).
class DBStorage : public Persistence
{
public:
    // `data` param use nahi hota (real DB code hota to hota) -> compiler
    // "unused parameter" warning deta hai. Real implementation me ye query
    // banane ke kaam aata.
    void save(string data) override
    {
        cout << "Document saved to DB" << endl;
    }
};

// ============================================================================
// 4. EDITOR CONTROLLER (Context / Facade)
// ============================================================================

// DocumentEditor: sabko jodne wala "conductor". Client isi se baat karta hai —
// addText/addImage/... bolta hai, aur ye andar sahi element bana ke Document me
// daal deta hai. Client ko TextElement/ImageElement ka naam tak nahi pata (facade).
class DocumentEditor
{
private:
    Document *document;       // kis document pe kaam kar rahe (INJECT hota hai)
    Persistence *storage;     // save ki strategy (INJECT hoti hai)
    string renderedDocument;  // render ka cache (baar-baar render na karna pade)

public:
    // ⭐ DEPENDENCY INJECTION: document aur storage BAHAR se aate hain, editor
    // khud nahi banata. Isi liye editor kisi bhi document + kisi bhi storage ke
    // saath chal jaata hai. Test bhi aasan (fake storage bhej do).
    DocumentEditor(Document *document, Persistence *storage)
    {
        this->document = document;
        this->storage = storage;
    }

    // ---- Add-* helpers: client ke liye aasaan API (facade) ------------------
    // Client `new TextElement(...)` nahi likhta — bas `addText("...")`. Concrete
    // class ka naam editor ke andar chhupa hai.
    void addText(string text)
    {
        document->addElement(new TextElement(text));
    }

    void addImage(string imagePath)
    {
        document->addElement(new ImageElement(imagePath));
    }

    void addNewLine()
    {
        document->addElement(new NewLineElement());
    }

    void addTabSpace()
    {
        document->addElement(new TabSpaceElement());
    }

    void addBoldText(string text)
    {
        document->addElement(new BoldTextElement(text));
    }

    // renderDocument: poore document ko string me badalta hai, aur CACHE karta.
    //
    // ⚠ CACHING BUG (chhupa hua): pehli baar render hoke `renderedDocument` me
    // cache ho jaata hai. Ab agar iske BAAD tum aur text add karo aur dobara
    // renderDocument() bulao, to ye PURANA cache lauta dega — naya text dikhega
    // hi nahi! (`.empty()` check sirf "pehli baar" pakadta hai, "badla ya nahi"
    // nahi.) Is demo me nahi dikhta kyunki add karne ke baad render sirf ek baar
    // hota hai. Behtar: jab bhi addElement ho, cache ko "gandi" (invalid) maark
    // karo (jaise ek `bool dirty` flag), ya cache rakho hi mat.
    string renderDocument()
    {
        if (renderedDocument.empty())
        {
            renderedDocument = document->render();
        }
        return renderedDocument;
    }

    // saveDocument: render karo aur strategy ko de do. Editor ko pata hi nahi ki
    // save file me ja raha ya DB me — bas `storage->save(...)` bulata hai. Yahi
    // Strategy pattern ka fayda.
    void saveDocument()
    {
        storage->save(renderDocument());
    }
};

// ============================================================================
// 5. CLIENT DRIVER (main)
// ============================================================================
int main()
{
    // 1. Ek khaali document banao.
    Document *document = new Document();

    // 2. FileStorage strategy chuno, aur editor ko document + strategy do.
    Persistence *filePersistence = new FileStorage();
    DocumentEditor *editor = new DocumentEditor(document, filePersistence);

    // Content add karo — har call andar sahi element bana ke document me daalti.
    editor->addText("Hello, world!");
    editor->addNewLine();
    editor->addBoldText("This is a bold text.");
    editor->addNewLine();
    editor->addTabSpace();
    editor->addImage("picture.jpg");

    // File me save (document.txt banegi).
    cout << "--- File Save Attempt (check document.txt) ---" << endl;
    editor->saveDocument();

    // 3. ⭐ STRATEGY SWAPPING: ab SAME document ko DB strategy se save karte hain.
    // Naya editor DBStorage ke saath — aur bas storage badalne se save ki jagah
    // badal gayi. Element/document ka code ek line bhi nahi badla.
    Persistence *dbPersistence = new DBStorage();
    DocumentEditor *dbEditor = new DocumentEditor(document, dbPersistence);

    cout << "\n--- DB Save Attempt ---" << endl;
    dbEditor->saveDocument(); // "Document saved to DB" print karega

    // 4. Cleanup — jo `new` kiya wo `delete`.
    // ⚠ Par dhyaan: `delete document` sirf Document object hataata hai, uske
    // ANDAR ke elements (TextElement waghairah) NAHI — kyunki Document ka
    // destructor missing hai (upar dekho). Isi liye yahan 6 elements LEAK hote
    // hain. Smart-pointer version me ye problem hoti hi nahi.
    delete editor;
    delete dbEditor;
    delete filePersistence;
    delete dbPersistence;
    delete document;

    return 0;
}
