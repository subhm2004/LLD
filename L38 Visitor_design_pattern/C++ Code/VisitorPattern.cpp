// ============================================================================
//  VisitorPattern.cpp  —  Visitor Design Pattern (Behavioral)
// ----------------------------------------------------------------------------
//  Visitor = element classes ko BINA modify kiye unpe naye OPERATIONS add karna.
//  Trick: DOUBLE DISPATCH -> element.accept(visitor) andar se visitor.visit(this)
//  call karta hai, jisse sahi element-type ka sahi visit() chalta hai.
//  Yahan files (Text/Image/Video) fixed hain, par operations (Size/Compress/
//  VirusScan) alag-alag visitor classes ke roop me add hote hain — naya
//  operation = naya visitor, files ko chhede bina.
//  Trade-off: naya OPERATION add karna easy, par naya ELEMENT type add karna
//  mushkil (har visitor me naya visit() daalna padega).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// Forward declarations — visitor ko teeno concrete types ka pata hona chahiye.
class TextFile;
class ImageFile;
class VideoFile;

// Visitor Interface: har element type ke liye ek visit() overload.
class FileSystemVisitor {
public:
    virtual ~FileSystemVisitor() = default;

    virtual void visit(TextFile* file) = 0;
    virtual void visit(ImageFile* file) = 0;
    virtual void visit(VideoFile* file) = 0;
};

// Element base: accept() dispatch #1 hai (element type pick hota hai).
class FileSystemItem {
protected:
    string name;

public:
    FileSystemItem(const string& itemName) {
        name = itemName;
    }
    virtual ~FileSystemItem() = default;

    string getName() const { return name; }

    virtual void accept(FileSystemVisitor* visitor) = 0;
};

// Concrete element: TextFile
class TextFile : public FileSystemItem {
private:
    string content;

public:
    TextFile(const string& fileName, const string& fileContent) : FileSystemItem(fileName) {
        content = fileContent;
    }

    string getContent() const {
        return content;
    }

    // Dispatch #2: yahan `this` concrete TextFile* hai -> sahi visit(TextFile*) chalega.
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Concrete element: ImageFile
class ImageFile : public FileSystemItem {

public:
    ImageFile(string fileName) : FileSystemItem(fileName) {}

    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Concrete element: VideoFile
class VideoFile : public FileSystemItem {
public:
    VideoFile(const string& fileName) : FileSystemItem(fileName) {}

    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Operation #1 — size calculation (har file type ke liye apna logic).
class SizeCalculationVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Calculating size for TEXT file: " << file->getName() << endl;
    }

    void visit(ImageFile* file) override {
        cout << "Calculating size for IMAGE file: " << file->getName() << endl;
    }

    void visit(VideoFile* file) override {
        cout << "Calculating size for VIDEO file: " <<  file->getName() << endl;
    }
};

// Operation #2 — compression. (Naya operation = bas naya visitor, files same.)
class CompressionVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Compressing TEXT file: " << file->getName() << endl;
    }

    void visit(ImageFile* file) override {
        cout << "Compressing IMAGE file: " << file->getName() << endl;
    }

    void visit(VideoFile* file) override {
        cout << "Compressing VIDEO file: " << file->getName() << endl;
    }
};

// Operation #3 — virus scanning.
class VirusScanningVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Scanning TEXT file: " << file->getName() << endl;
    }

    void visit(ImageFile* file) override {
        cout << "Scanning IMAGE file: " << file->getName() << endl;
    }

    void visit(VideoFile* file) override {
        cout << "Scanning VIDEO file: " << file->getName() << endl;
    }
};

int main() {
    // Ek image par 3 alag operations — element same, visitors alag.
    FileSystemItem* img1 = new ImageFile("sample.jpg");

    img1->accept(new SizeCalculationVisitor());
    img1->accept(new CompressionVisitor());
    img1->accept(new VirusScanningVisitor());

    // Video par compression operation.
    FileSystemItem* vid1 = new VideoFile("test.mp4");
    vid1->accept(new CompressionVisitor());

    return 0;
}
