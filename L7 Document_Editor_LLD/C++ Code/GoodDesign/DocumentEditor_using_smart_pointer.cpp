// ============================================================================
//  DocumentEditor_using_smart_pointer.cpp (GoodDesign)  —  smart-pointer version
// ----------------------------------------------------------------------------
//  Wahi achha design (polymorphic elements + Persistence strategy), par raw
//  pointers ki jagah `shared_ptr`/`unique_ptr`. Faayda: manual delete nahi,
//  memory automatic clean (RAII), exception-safe. Production-quality C++ idiom.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory> // Smart pointers (std::unique_ptr, std::shared_ptr) utility library file headers.

using namespace std;

// ============================================================================
// 1. POLYMORPHIC COMPONENT HIERARCHY (Open-Closed Principle - OCP)
// ============================================================================

// DocumentElement: Base interface abstract class define.
class DocumentElement
{
public:
    virtual string render() = 0;
    virtual ~DocumentElement()
    {
        // Smart pointer automatic destructions triggers verification hooks logs can be placed here.
    }
};

// TextElement: Plain text content elements configuration class.
class TextElement : public DocumentElement
{
private:
    string text;

public:
    TextElement(string t) : text(t) {}
    string render() override { 
      return text; 
    }
};

// BoldTextElement: Markdown bold markup formatting layout representations.
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

// ImageElement: Image dynamic path mapping container.
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

// NewLineElement: Line break formatting controller class.
class NewLineElement : public DocumentElement
{
public:
    string render() override { 
      return "\n"; 
    }
};

// TabSpaceElement: Gaps layout spaces selector.
class TabSpaceElement : public DocumentElement
{
public:
    string render() override { 
      return "\t"; 
    }
};

// ============================================================================
// 2. DOCUMENT CONTAINER (Aggregator using modern std::unique_ptr vectors)
// ============================================================================

// Document: Container class maintaining elements sequences.
class Document
{
private:
    // Raw pointers array vectors bypass. std::unique_ptr ensures single-ownership model safety.
    // Vector destroy hone par, saare element structures automatically clean ho jate hain heap memory se.
    vector<unique_ptr<DocumentElement>> documentElements;

public:
    // addElement: Moves ownership to internal vector dataset dynamically.
    void addElement(unique_ptr<DocumentElement> element)
    {
        // unique_ptr values are non-copyable, isliye ownership transfer ke liye std::move mandatory check hai.
        documentElements.push_back(std::move(element));
    }

    // render: Iterates loops elements, invoking respective polymorphic render overrides.
    string render()
    {
        string result;
        for (const auto &element : documentElements)
        {
            result += element->render();
        }
        return result;
    }

    // Yahan kisi manual destructor cleaner loop (~Document) ki jarurat nahi padegi.
    // unique_ptr container scope exit triggers automatically delete dynamic objects allocations.
};

// ============================================================================
// 3. STRATEGY DESIGN PATTERN (Document Storage Persistence Systems)
// ============================================================================

// Persistence: Strategy Base interface targets class defining operations contracts.
class Persistence
{
public:
    virtual void save(string data) = 0;
    virtual ~Persistence() {}
};

// FileStorage: Persistence Strategy concrete implementation. Saves document formatted strings in local txt file.
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

// DBStorage: Persistence Strategy concrete database implementation.
class DBStorage : public Persistence
{
public:
    void save(string data) override
    {
        cout << "Document saved to DB (Simulated)" << endl;
    }
};

// ============================================================================
// 4. EDITOR CONTROLLER CONTEXT (Strategy Context Controller)
// ============================================================================

// DocumentEditor: Aggregator context executing actions on Document.
class DocumentEditor
{
private:
    Document *document;              // Weak association reference pointer.
    unique_ptr<Persistence> storage; // Strategy pointer wrapped under unique_ptr context controls.

public:
    // Constructor: Injects Document dependency and transfers Persistence Strategy unique pointer ownership.
    DocumentEditor(Document *doc, unique_ptr<Persistence> s)
    {
        this->document = doc;
        this->storage = std::move(s);
    }

    // make_unique wrapper methods ensure optimal exception safe heap memory allocations setup patterns.
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

    // setStorage: Strategy runtime swapping helper method.
    // Naya storage persistence algorithm runtime par inject karne ke liye parameter utility function.
    void setStorage(unique_ptr<Persistence> newStorage)
    {
        this->storage = std::move(newStorage);
    }

    // saveDocument: Strategy executor callback method.
    void saveDocument()
    {
        storage->save(document->render());
    }
};

// ============================================================================
// 5. CLIENT DRIVER ENTRY POINT
// ============================================================================
int main()
{
    // make_unique allocations automatically handles deallocations tasks.
    auto myDocument = make_unique<Document>();

    // PEHLE Persistence strategy object setup (File Storage selection initialization)
    auto fileStore = make_unique<FileStorage>();
    DocumentEditor editor(myDocument.get(), std::move(fileStore));

    // Element addition calls mapping triggers.
    editor.addText("Hello C++!");
    editor.addNewLine();
    editor.addBoldText("Smart Pointers are great.");
    editor.addTabSpace();
    editor.addImage("tech.png");

    cout << "--- Attempting File Save ---" << endl;
    editor.saveDocument();

    // DUSRA Runtime strategy swapping check (Switch to Database saving)
    cout << "\n--- Switching to DB and Saving ---" << endl;
    editor.setStorage(make_unique<DBStorage>()); // Swaps old strategy dynamic allocations pointer automatically.
    editor.saveDocument();

    return 0;
    // Yaha kisi manual delete operation keys call ki jarurat nahi padegi.
    // RAII principles based compiler triggers automatically safely destroys allocations.
}