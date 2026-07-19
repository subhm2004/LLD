// ============================================================================
//  VisitorPattern.cpp  —  VISITOR DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Visitor = "element classes ko BINA modify kiye unpe naye OPERATIONS add
//             karo — har operation apni alag VISITOR class me jaata hai, aur
//             DOUBLE DISPATCH se sahi type ke liye sahi code apne aap chalta hai."
//
//  Asli duniya ka example: file system ki files pe operations:
//    - Elements:   TextFile / ImageFile / VideoFile  (STABLE — badalte nahi)
//    - Operations: Size nikaalo / Compress karo / Virus scan karo
//                  (BADHTE rehte hain — kal encrypt, backup, preview aayenge...)
//  Har operation har file type ke liye ALAG kaam karta hai — text ka size
//  character count se, image ka resolution se, video ka duration se.
//  DO cheezein mil ke decide karti hain kya chale: FILE TYPE + OPERATION.
//  Isi "do-type dispatch" ko Visitor ka double dispatch solve karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  VISITOR KYUN? — bina iske code ka haal:                                │
//  │                                                                          │
//  │    class TextFile  { getSize(); compress(); scan(); };                  │
//  │    class ImageFile { getSize(); compress(); scan(); };                  │
//  │    class VideoFile { getSize(); compress(); scan(); };                  │
//  │                                                                          │
//  │  Naya operation "encrypt" aaya -> TEENO classes kholo, teeno me method  │
//  │  daalo. Parso "backup" aaya -> phir teeno... ye kabhi rukta nahi! 😵    │
//  │                                                                          │
//  │  1) OCP break — har naya operation purani tested classes chhedta hai    │
//  │  2) "Compression" ka logic 3 alag files me BIKHRA hai                   │
//  │  3) File classes har unrelated operation ka kachra jama karti jaati hain│
//  │                                                                          │
//  │  VISITOR se: har operation EK visitor class me — naya operation = nayi  │
//  │  visitor class, bas. FILE CLASSES KO CHHUNA TAK NAHI. Elements me sirf  │
//  │  EK method rehta hai: accept(visitor). Ye OPEN/CLOSED PRINCIPLE hai —   │
//  │  operations ke liye khula, element modification ke liye band.           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//    1. Visitor (interface) -> FileSystemVisitor      : har element type ke
//                                                       liye ek visit() overload
//    2. ConcreteVisitor     -> SizeCalculationVisitor : size ka SAARA logic
//                           -> CompressionVisitor     : compression ka SAARA logic
//                           -> VirusScanningVisitor   : scanning ka SAARA logic
//    3. Element (interface) -> FileSystemItem         : sirf accept(visitor)
//    4. ConcreteElement     -> TextFile / ImageFile / VideoFile
//                              : accept() me visitor->visit(this) karte hain
//
//  ============================================================================
//   ⭐ DOUBLE DISPATCH — pattern ka magic trick (interview ka #1 sawaal)
//  ----------------------------------------------------------------------------
//   Normal virtual call sirf EK type pe dispatch hota hai (receiver ke).
//   Humein DO chahiye: element ka type + operation ka version. Do-step solution:
//
//   Step | Call                  | Dispatch ka type      | Kya decide hua
//   -----+-----------------------+-----------------------+---------------------
//    1   | item->accept(visitor) | VIRTUAL dispatch      | element ka type
//        |                       | (runtime pe)          | (Text? Image? Video?)
//    2   | visitor->visit(this)  | OVERLOAD resolution   | us type ke liye sahi
//        | (accept ke andar)     | (`this` concrete hai, | visit() ka version
//        |                       |  compile-time choice) |
//
//   Do dispatch hue -> isliye naam "DOUBLE dispatch"!
//
//  ============================================================================
//   OPERATION × ELEMENT MATRIX — extension ka trade-off ek nazar me
//   (har cell = kisi visitor ka ek visit() method)
//  ----------------------------------------------------------------------------
//              | SizeCalc | Compression | VirusScan | Encrypt (NAYA)
//   -----------+----------+-------------+-----------+-----------------
//   TextFile   |  visit   |   visit     |   visit   |   visit    <- naya operation:
//   ImageFile  |  visit   |   visit     |   visit   |   visit       NAYA COLUMN =
//   VideoFile  |  visit   |   visit     |   visit   |   visit       sirf 1 nayi
//   AudioFile  |  visit   |   visit     |   visit   |   visit       class ✅ EASY
//   (NAYA)     |    ↑ naya element: NAYI ROW = visitor interface +
//              |      HAR concrete visitor me naya visit() ❌ MUSHKIL
//
//   ⭐ Isliye Visitor tabhi lo jab ELEMENTS STABLE ho aur OPERATIONS badhte
//   ho — jaise compiler ke AST nodes (Clang ka ASTVisitor yahi karta hai!).
//   Agar elements zyada badalte hain to Visitor ulta padega — seedha
//   virtual methods hi rakho.
//
//   📌 SABSE BADA RULE — accept() HAR CONCRETE CLASS ME LIKHNA PADTA HAI:
//   Teeno elements me `visitor->visit(this)` same dikhta hai par ye
//   copy-paste NAHI hai — har class me `this` ka TYPE alag hai (TextFile* /
//   ImageFile* / VideoFile*), aur WAHI type sahi visit() overload chunta
//   hai. Base class me daal do to `this` FileSystemItem* ban jaata —
//   jiske liye koi visit() overload exist hi nahi karta. Pattern hi toot jaata!
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============================================================================
//  FORWARD DECLARATIONS
// ----------------------------------------------------------------------------
//  Visitor interface ko teeno concrete file types ke naam chahiye
//  (visit ke parameters me), par unki poori definition abhi neeche hai.
//  Forward declaration compiler ko bata deti hai — "ye classes exist
//  karti hain, definition baad me milegi."
// ============================================================================
class TextFile;
class ImageFile;
class VideoFile;

// ============================================================================
//  SECTION 1: VISITOR INTERFACE
// ----------------------------------------------------------------------------
//  HAR element type ke liye EK visit() overload — yahi pattern ka
//  contract hai: "jo bhi operation banega, use har file type ko handle
//  karna aana chahiye."
//
//  ⚠️ Isi wajah se naya ELEMENT add karna mehnga hai — VideoFile ke baad
//  AudioFile aaya to yahan visit(AudioFile*) add hoga → HAR concrete
//  visitor class tootegi jab tak wo bhi implement na kare.
// ============================================================================
class FileSystemVisitor {
public:
    // Virtual destructor — visitor bhi base pointer se delete ho sakta hai
    virtual ~FileSystemVisitor() = default;

    // Teen overloads — same naam, alag parameter type.
    // Compiler OVERLOAD RESOLUTION se sahi wala chunta hai (compile-time pe),
    // jab accept() ke andar concrete `this` pass hota hai.
    virtual void visit(TextFile* file) = 0;
    virtual void visit(ImageFile* file) = 0;
    virtual void visit(VideoFile* file) = 0;
};

// ============================================================================
//  SECTION 2: ELEMENT BASE CLASS — FileSystemItem
// ----------------------------------------------------------------------------
//  Saari files ka common base. Iske paas operations ka KOI method nahi
//  (na getSize, na compress...) — sirf accept() hai jo visitor ko
//  welcome karta hai. Yahi to point hai — operations bahar visitor me!
// ============================================================================
class FileSystemItem {
protected:
    string name;   // har file ka naam — derived classes bhi use karengi

public:
    FileSystemItem(const string& itemName) {
        name = itemName;
    }
    // Virtual destructor — FileSystemItem* se derived delete hoga to
    // derived ka destructor sahi chale
    virtual ~FileSystemItem() = default;

    string getName() const { return name; }

    // -------------------- DISPATCH #1 --------------------
    // Ye pure virtual hai — har concrete file apna accept() degi.
    // Jab client base pointer se accept() call karega, VIRTUAL DISPATCH
    // decide karega kaunsi file ka accept() chale — ye pehla dispatch hai
    // (element ka type yahan pick hota hai).
    virtual void accept(FileSystemVisitor* visitor) = 0;
};

// ============================================================================
//  SECTION 3: CONCRETE ELEMENTS — TextFile / ImageFile / VideoFile
// ----------------------------------------------------------------------------
//  Dhyan do — teeno classes me accept() ka code EK JAISA dikhta hai:
//      visitor->visit(this);
//  Par ye COPY-PASTE nahi hai! Har class me `this` ka TYPE alag hai:
//    TextFile ke andar  → this = TextFile*  → visit(TextFile*) chalega
//    ImageFile ke andar → this = ImageFile* → visit(ImageFile*) chalega
//  Isliye ye ek-ek line har class me likhni HI padti hai — base me
//  daal ke kaam nahi chalta (base me `this` FileSystemItem* hota,
//  jiska koi visit() overload hai hi nahi).
// ============================================================================

// --- Concrete Element #1: TextFile ---
class TextFile : public FileSystemItem {
private:
    string content;   // text file ke paas apna extra data bhi hai

public:
    TextFile(const string& fileName, const string& fileContent) : FileSystemItem(fileName) {
        content = fileContent;
    }

    // Visitor is getter se content padh sakta hai — visitor ko element
    // ka public interface hi milta hai (encapsulation ka dhyan)
    string getContent() const {
        return content;
    }

    // -------------------- DISPATCH #2 --------------------
    // YAHI double dispatch ki dusri kadi hai! Yahan `this` ka static
    // type TextFile* hai → compiler visit(TextFile*) overload chunega.
    // Element ne khud bataya "main TextFile hu" — visitor ko type
    // check (dynamic_cast/if-else) karne ki zaroorat hi nahi padi!
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// --- Concrete Element #2: ImageFile ---
class ImageFile : public FileSystemItem {

public:
    ImageFile(string fileName) : FileSystemItem(fileName) {}

    // Yahan `this` = ImageFile* → visit(ImageFile*) chalega
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// --- Concrete Element #3: VideoFile ---
class VideoFile : public FileSystemItem {
public:
    VideoFile(const string& fileName) : FileSystemItem(fileName) {}

    // Yahan `this` = VideoFile* → visit(VideoFile*) chalega
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);
    }
};

// ============================================================================
//  SECTION 4: CONCRETE VISITORS — har ek EK OPERATION ka specialist
// ----------------------------------------------------------------------------
//  Ab dekho pattern ka asli fayda:
//  - "Size" ka SAARA logic (teeno file types ka) EK class me — bikhra nahi
//  - Naya operation chahiye? Naya visitor class — file classes UNTOUCHED
//  - Har file type ke liye ALAG implementation likh sakte ho —
//    text ka size character count se, video ka duration × bitrate se
// ============================================================================

// --- Operation #1: Size Calculation ---
// Real me: text → character count, image → resolution, video → duration.
// Har type ka apna visit() = har type ka apna size formula!
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

// --- Operation #2: Compression ---
// DHYAN DO: ye poora naya operation add hua — aur TextFile/ImageFile/
// VideoFile classes me EK character bhi nahi badla! Yahi OCP hai —
// elements "closed for modification", operations "open for extension".
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

// --- Operation #3: Virus Scanning ---
// Teesra operation — phir wahi baat, file classes ko haath tak nahi lagaya.
// Kal ko EncryptionVisitor, BackupVisitor... jitne chaho add karo!
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

// ============================================================================
//  SECTION 5: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Client ka kaam simple: element banao, uspe jo operation chahiye
//  uska visitor accept() me pass kar do.
// ============================================================================
int main() {
    // EK image file par TEEN alag operations — element SAME, visitors ALAG.
    // Har accept() call ke andar double dispatch chalega:
    //   accept() [dispatch #1: ImageFile chuna] → visit(this)
    //   [dispatch #2: visit(ImageFile*) overload chuna]
    FileSystemItem* img1 = new ImageFile("sample.jpg");

    img1->accept(new SizeCalculationVisitor());  // → "Calculating size for IMAGE file..."
    img1->accept(new CompressionVisitor());      // → "Compressing IMAGE file..."
    img1->accept(new VirusScanningVisitor());    // → "Scanning IMAGE file..."

    // Video par sirf compression — same visitor class VideoFile ke liye
    // apna wala visit(VideoFile*) chalayegi.
    FileSystemItem* vid1 = new VideoFile("test.mp4");
    vid1->accept(new CompressionVisitor());      // → "Compressing VIDEO file..."

    // NOTE: Demo me visitors/files delete nahi kiye (chhota program,
    // OS cleanup kar dega) — production me unique_ptr use karo ya
    // manually delete karo, warna memory leak!

    return 0;
}
