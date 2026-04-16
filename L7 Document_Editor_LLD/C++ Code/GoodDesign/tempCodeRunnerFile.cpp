#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// Abstraction for document elements
class DocumentElement
{
public:
    virtual string render() = 0;

    virtual ~DocumentElement() {}
};

// Concrete implementation for text elements
class TextElement : public DocumentElement
{
private:
    string text;

public:
    TextElement(string text)
    {
        this->text = text;
    }

    string render() override
    {
        return text;
    }
};

// Concrete implementation for image elements
class ImageElement : public DocumentElement
{
private:
    string imagePath;

public:
    ImageElement(string imagePath)
    {
        this->imagePath = imagePath;
    }

    string render() override
    {
        return "[Image: " + imagePath + "]";
    }
};

// NewLineElement represents a line break in the document.
class NewLineElement : public DocumentElement
{
public:
    string render() override
    {
        return "\n";
    }
};

// TabSpaceElement represents a tab space in the document.
class TabSpaceElement : public DocumentElement
{
public:
    string render() override
    {
        return "\t";
    }
};
// --- NAYA FEATURE: Bold Text Element ---
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
        // Markdown style bold
        return "**" + text + "**";
    }
};

// Document class responsible for holding a collection of elements
class Document
{
private:
    vector<DocumentElement *> documentElements;

public:
    void addElement(DocumentElement *element)
    {
        documentElements.push_back(element);
    }

    // Renders the document by concatenating the render output of all elements.
    string render()
    {
        string result;
        for (auto element : documentElements)
        {
            result += element->render();
        }
        return result;
    }
};

// Persistence abstraction
class Persistence
{
public:
    virtual void save(string data) = 0;

    virtual ~Persistence() {}
};

// FileStorage implementation of Persistence
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

// Placeholder DBStorage implementation
class DBStorage : public Persistence
{
public:
    void save(string data) override
    {
        // Save to DB
        cout << "Document saved to DB" << endl;
    }
};

// DocumentEditor class managing client interactions
class DocumentEditor
{
private:
    Document *document;
    Persistence *storage;
    string renderedDocument;

public:
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

    // Adds a new line to the document.
    void addNewLine()
    {
        document->addElement(new NewLineElement());
    }

    // Adds a tab space to the document.
    void addTabSpace()
    {
        document->addElement(new TabSpaceElement());
    }
    // --- NAYA FEATURE: Bold Text Element ---
    void addBoldText(string text)
    {
        document->addElement(new BoldTextElement(text));
    }

    string renderDocument()
    {
        if (renderedDocument.empty())
        {
            renderedDocument = document->render();
        }
        return renderedDocument;
    }

    void saveDocument()
    {
        storage->save(renderDocument());
    }
};

// Client usage example
int main()
{
    // 1. Basic Setup
    Document *document = new Document();

    // 2. Pehle File Storage use karte hain
    Persistence *filePersistence = new FileStorage();
    DocumentEditor *editor = new DocumentEditor(document, filePersistence);

    // Document content add karna
    editor->addText("Hello, world!");
    editor->addNewLine();
    editor->addBoldText("This is a bold text.");
    editor->addNewLine();
    editor->addTabSpace();
    editor->addImage("picture.jpg");

    // File mein save karein
    cout << "--- File Save Attempt (check document.txt) ---" << endl;
    editor->saveDocument();

    // 3. Ab DB Storage use karte hain
    // Hum ek naya editor bana rahe hain jo same document use karega par storage DB wala
    Persistence *dbPersistence = new DBStorage();
    DocumentEditor *dbEditor = new DocumentEditor(document, dbPersistence);

    cout << "\n--- DB Save Attempt ---" << endl;
    dbEditor->saveDocument(); // Yeh "Document saved to DB" print karega

    // Cleanup
    delete editor;
    delete dbEditor;
    delete filePersistence;
    delete dbPersistence;
    delete document;

    return 0;
}
