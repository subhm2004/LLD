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
#include <memory> // Smart pointers ke liye

using namespace std;

// 1. Abstraction for document elements
class DocumentElement
{
public:
    virtual string render() = 0;
    virtual ~DocumentElement()
    {
        // cout << "Element Deleted" << endl; // Debugging ke liye
    }
};

// 2. Concrete Implementations
class TextElement : public DocumentElement
{
private:
    string text;

public:
    TextElement(string t) : text(t) {}
    string render() override { return text; }
};

class BoldTextElement : public DocumentElement
{
private:
    string text;

public:
    BoldTextElement(string t) : text(t) {}
    string render() override { return "**" + text + "**"; }
};

class ImageElement : public DocumentElement
{
private:
    string imagePath;

public:
    ImageElement(string path) : imagePath(path) {}
    string render() override { return "[Image: " + imagePath + "]"; }
};

class NewLineElement : public DocumentElement
{
public:
    string render() override { return "\n"; }
};

class TabSpaceElement : public DocumentElement
{
public:
    string render() override { return "\t"; }
};

// 3. Document Class (Container)
class Document
{
private:
    // Raw pointer vector ki jagah unique_ptr ka vector
    vector<unique_ptr<DocumentElement>> documentElements;

public:
    void addElement(unique_ptr<DocumentElement> element)
    {
        // unique_ptr ko std::move karna padta hai vector mein
        documentElements.push_back(std::move(element));
    }

    string render()
    {
        string result;
        for (const auto &element : documentElements)
        {
            result += element->render();
        }
        return result;
    }

    // Yahan koi DESTRUCTOR (~Document) likhne ki zaroorat nahi hai!
    // Vector delete hote hi saare unique_ptr apne aap saaf ho jayenge.
};

// 4. Persistence Abstraction
class Persistence
{
public:
    virtual void save(string data) = 0;
    virtual ~Persistence() {}
};

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

class DBStorage : public Persistence
{
public:
    void save(string data) override
    {
        cout << "Document saved to DB (Simulated)" << endl;
    }
};

// 5. Document Editor
class DocumentEditor
{
private:
    // Shared_ptr ya Raw pointer yahan behtar hai kyunki Editor
    // zaroori nahi ki Document ka 'Malik' ho, wo bas use 'Edit' kar raha hai.
    // Par yahan simplicity ke liye hum unique_ptr ownership handle kar rahe hain.
    Document *document;
    unique_ptr<Persistence> storage;

public:
    DocumentEditor(Document *doc, unique_ptr<Persistence> s)
    {
        this->document = doc;
        this->storage = std::move(s);
    }

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

    // Storage badalne ke liye setter
    void setStorage(unique_ptr<Persistence> newStorage)
    {
        this->storage = std::move(newStorage);
    }

    void saveDocument()
    {
        storage->save(document->render());
    }
};

// 6. Main Function
int main()
{
    // make_unique se memory allocate karein
    auto myDocument = make_unique<Document>();

    // Editor banayein (FileStorage ke saath)
    auto fileStore = make_unique<FileStorage>();
    DocumentEditor editor(myDocument.get(), std::move(fileStore));

    editor.addText("Hello C++!");
    editor.addNewLine();
    editor.addBoldText("Smart Pointers are great.");
    editor.addTabSpace();
    editor.addImage("tech.png");

    cout << "--- Attempting File Save ---" << endl;
    editor.saveDocument();

    // Ab DB mein switch karte hain
    cout << "\n--- Switching to DB and Saving ---" << endl;
    editor.setStorage(make_unique<DBStorage>());
    editor.saveDocument();

    return 0;
    // Sab kuch AUTOMATICALLY delete ho jayega!
}