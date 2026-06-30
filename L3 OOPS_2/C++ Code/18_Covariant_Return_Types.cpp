// ============================================================================
//  18_Covariant_Return_Types.cpp  —  Covariant return types
// ----------------------------------------------------------------------------
//  Normally override ka return type same hona chahiye. Exception: COVARIANT
//  return — agar base ka virtual function Base* (ya Base&) return karta hai, to
//  override usse DERIVED* (ya Derived&) return kar sakta hai. Bahut useful in
//  clone()/factory methods, jahan har subclass apne hi type ka object return kare
//  bina caller ko cast karne padte.
// ============================================================================
#include <iostream>
#include <memory>
#include <string>
using namespace std;

// --- Covariant return: override with DERIVED pointer/reference return type ---

class Document {
public:
    virtual ~Document() = default;
    virtual Document* clone() const {  // factory hook
        cout << "  Document::clone → Document*\n";
        return new Document(*this);
    }
    virtual string summary() const { return "Generic document"; }
};

class PdfDocument : public Document {
    int pages = 10;
public:
    PdfDocument() = default;
    explicit PdfDocument(int p) : pages(p) {}

    // Covariant return: PdfDocument* instead of Document*
    PdfDocument* clone() const override {
        cout << "  PdfDocument::clone → PdfDocument* (covariant)\n";
        return new PdfDocument(*this);
    }

    string summary() const override { return "PDF, pages=" + to_string(pages); }
    int getPages() const { return pages; }
};

class HtmlDocument : public Document {
public:
    Document* clone() const override {  // still valid — return type is base pointer
        cout << "  HtmlDocument::clone → Document*\n";
        return new HtmlDocument(*this);
    }
};

void useClone(Document* original) {
    unique_ptr<Document> copy(original->clone());
    cout << "  copy summary: " << copy->summary() << endl;

    // Without covariant return we need dynamic_cast:
    if (PdfDocument* pdf = dynamic_cast<PdfDocument*>(copy.get())) {
        cout << "  (via dynamic_cast) pages=" << pdf->getPages() << endl;
    }
}

void useCloneCovariant(const Document& original) {
    unique_ptr<Document> baseHolder(original.clone());
    // If original was PdfDocument, clone() returned PdfDocument* — no cast for Pdf-specific API
    if (PdfDocument* pdf = dynamic_cast<PdfDocument*>(baseHolder.get())) {
        cout << "  Covariant path: pages=" << pdf->getPages() << endl;
    }
}

int main() {
    cout << "=== Covariant return type in override ===\n";

    PdfDocument pdf(42);
    cout << "Clone via base pointer:\n";
    useClone(&pdf);

    cout << "\nDirect covariant call:\n";
    unique_ptr<PdfDocument> pdfCopy(pdf.clone());  // compile-time type PdfDocument*
    cout << "  pages=" << pdfCopy->getPages() << endl;

    cout << "\n=== Rules ===\n";
    cout << "  - Return type may be pointer/reference to DERIVED class.\n";
    cout << "  - Must be override of virtual function in base.\n";
    cout << "  - C++ does NOT allow covariant by-value return (different class sizes).\n";
    cout << "  - Use case: clone()/factory methods, deep-copy hierarchies.\n";

    HtmlDocument html;
  cout << "\nHtml (non-covariant override still OK):\n";
    useClone(&html);

    return 0;
}
