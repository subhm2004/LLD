// ============================================================================
//  DocumentEditor.cpp (GoodDesign)  —  Document Editor ka SAHI design
// ----------------------------------------------------------------------------
//  BadDesign ka fix: har document element (Text, Image, Bold, NewLine, Tab) ek
//  DocumentElement subclass hai jo apna render() khud jaanta hai (no string
//  checks). Storage ke liye Persistence STRATEGY (FileStorage/DBStorage) inject
//  hota hai. DocumentEditor sirf orchestrate karta hai. Naya element ya storage
//  add karna = nayi class, purani edit nahi (OCP follow).
//  (Raw pointer version; smart-pointer variant alag file me.)
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// ============================================================================
// 1. POLYMORPHIC COMPONENT HIERARCHY (Open-Closed Principle - OCP)
// ============================================================================
// BadDesign me elements ko direct list of strings me save kiya jata thha aur
// rendering ke time lambi string checking loops chalani padti thi.
// GoodDesign me humne har document element type ko ek alag subclass me 
// encapsulate kiya hai jo render() custom string returns handle karti hain.

// DocumentElement: Sabhi document components (Text, Image, spaces) ke liye base interface class.
class DocumentElement
{
public:
    // render(): Pure virtual function jo string format me formatted element output return karta hai.
    virtual string render() = 0;

    // Virtual destructor memory dynamic deallocations safely check karne ke liye.
    virtual ~DocumentElement() {}
};

// TextElement: Standard Plain text component content storage subclass.
class TextElement : public DocumentElement
{
private:
    string text; // User input text body character.

public:
    TextElement(string text)
    {
        this->text = text;
    }

    // render() overrides base class method to directly return plain text.
    string render() override
    {
        return text;
    }
};

// ImageElement: Rich component image paths references details wrap karne ke liye.
class ImageElement : public DocumentElement
{
private:
    string imagePath; // Image storage location paths checks.

public:
    ImageElement(string imagePath)
    {
        this->imagePath = imagePath;
    }

    // render() overrides to format path inside standard tags.
    string render() override
    {
        return "[Image: " + imagePath + "]";
    }
};

// NewLineElement: Document formatting ke line breaks coordinates setup checking class.
class NewLineElement : public DocumentElement
{
public:
    string render() override
    {
        return "\n";
    }
};

// TabSpaceElement: Text block gaps alignment formatting system.
class TabSpaceElement : public DocumentElement
{
public:
    string render() override
    {
        return "\t";
    }
};

// BoldTextElement: Markdown bold markup formatting layout representations.
// OCP Proof: Purani existing code structures ko bina touch/alter kiye humne 
// new elements support system easily add kiya hai standard inheritance subclassing ke through.
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
        return "**" + text + "**"; // Markdown syntax bold string wrapper details.
    }
};

// ============================================================================
// 2. DOCUMENT CONTAINER (Aggregator Class)
// ============================================================================

// Document: Container class jo multiple dynamic elements list coordinate trace store karti hai.
class Document
{
private:
    // Pointers list standard dynamic vector arrays.
    vector<DocumentElement *> documentElements;

public:
    // addElement: Add custom elements to standard sequence list.
    void addElement(DocumentElement *element)
    {
        documentElements.push_back(element);
    }

    // render(): Composite evaluation traversal loops trace.
    // Sabhi internal elements loop structure me pass hokar dynamic text join coordinates evaluate karte hain.
    string render()
    {
        string result;
        for (auto element : documentElements)
        {
            result += element->render(); // Polymorphic method call parameters checks.
        }
        return result;
    }
};

// ============================================================================
// 3. STRATEGY DESIGN PATTERN (Document Storage Persistence Systems)
// ============================================================================
// Game or Editor documents dynamic saving location paths structures (File saving, DB, cloud updates) 
// runtime switchable interfaces properties me isolate hone chahiye. 
// Persistence base Strategy interface concrete implementations coordinates handle karta hai.

// Persistence: Strategy Base interface class define for saving targets.
class Persistence
{
public:
    virtual void save(string data) = 0;

    virtual ~Persistence() {}
};

// FileStorage: Persistence Strategy concrete implementation. 
// Standard Document string parameters file targets me dump coordinates trace updates.
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

// DBStorage: Persistence Strategy concrete implementation to save content in DB datasets.
class DBStorage : public Persistence
{
public:
    void save(string data) override
    {
        // Database queries execute updates placeholders logic.
        cout << "Document saved to DB" << endl;
    }
};

// ============================================================================
// 4. EDITOR CONTROLLER CONTEXT (Facade Interface Wrapper)
// ============================================================================

// DocumentEditor: Context Class jo Document elements insertions aur strategies coordinate controls manage karti hai.
class DocumentEditor
{
private:
    Document *document;       // Aggregation document pointer reference details.
    Persistence *storage;     // Active storage execution strategy interface pointer reference.
    string renderedDocument;  // Cached copy of rendered data to optimize multiple saves calculations.

public:
  // Constructor: Inject document object dependencies aur storage mechanisms pointers strategies.
    DocumentEditor(Document *document, Persistence *storage)
    {
        this->document = document;
        this->storage = storage;
    }

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

    // renderDocument: Evaluates total document content representation string mappings.
    string renderDocument()
    {
        if (renderedDocument.empty())
        {
            renderedDocument = document->render();
        }
        return renderedDocument;
    }

    // saveDocument: Strategy executor callback method. Directly triggers injected persistence strategies.
    void saveDocument()
    {
        storage->save(renderDocument());
    }
};

// ============================================================================
// 5. CLIENT DRIVER ENTRY POINT
// ============================================================================
int main()
{
    // 1. Core Document creation layout memory.
    Document *document = new Document();

    // 2. Select FileStorage strategy on start.
    Persistence *filePersistence = new FileStorage();
    DocumentEditor *editor = new DocumentEditor(document, filePersistence);

    // Dynamic inputs setup structures coordinates updates.
    editor->addText("Hello, world!");
    editor->addNewLine();
    editor->addBoldText("This is a bold text.");
    editor->addNewLine();
    editor->addTabSpace();
    editor->addImage("picture.jpg");

    // Saves document coordinates file strategy targets logic checks.
    cout << "--- File Save Attempt (check document.txt) ---" << endl;
    editor->saveDocument();

    // 3. Dynamic Strategy Swapping checks.
    // Hum runtime par different storage model pass karke database saving execution trigger karte hain.
    Persistence *dbPersistence = new DBStorage();
    DocumentEditor *dbEditor = new DocumentEditor(document, dbPersistence);

    cout << "\n--- DB Save Attempt ---" << endl;
    dbEditor->saveDocument(); // Triggers "Document saved to DB" message print.

    // 4. Memory dynamic allocations deallocations sweeps to avoid leak hazards.
    delete editor;
    delete dbEditor;
    delete filePersistence;
    delete dbPersistence;
    delete document;

    return 0;
}
