// ============================================================================
//  CompositePattern.cpp  —  COMPOSITE DESIGN PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Composite = "part-whole tree ko UNIFORM tareeke se treat karo — client ek
//               akele object (File) aur ek group (Folder) me FARQ na kare,
//               dono ko EK HI interface se use kare."
//
//  Asli duniya ka example: computer ka FILE SYSTEM 📁
//    - File = leaf ( end node, iske andar kuch nahi)
//    - Folder = composite (iske andar Files bhi + dusre Folders bhi!)
//    - Dono pe SAME operations: ls(), getSize(), openAll()...
//    - Folder ke andar Folder ke andar Folder... RECURSION natural hai!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  COMPOSITE KYUN? — bina iske client ka haal:                            │
//  │                                                                          │
//  │    // Client ko har jagah File vs Folder check karna padta:             │
//  │    if (item.isFile())   size = item.fileSize;                           │
//  │    else {  // folder                                                    │
//  │        size = 0;                                                        │
//  │        for (child : item.children)                                     │
//  │            if (child.isFile()) size += ...   // phir se check!          │
//  │            else size += recurse(child);      // manual recursion        │
//  │    }                                                                     │
//  │                                                                          │
//  │  Har operation me File/Folder ka if-else + manual recursion = mess.     │
//  │                                                                          │
//  │  COMPOSITE se: dono SAME interface (FileSystemItem) implement karte     │
//  │  hain. Client bas item->getSize() bolta hai — File apna size deti hai,  │
//  │  Folder KHUD apne children ka sum karke deta hai. Recursion pattern     │
//  │  ke ANDAR chhupi hai, client saaf! ✅                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//    1. Component (interface) -> FileSystemItem : common contract (ls/getSize/cd..)
//    2. Leaf                  -> File           : end node, koi children nahi
//    3. Composite             -> Folder         : children rakhta hai (File/Folder),
//                                                 operations ko recursively delegate
//    4. Client                -> main()         : uniform interface use karta hai
//
//  📌 SABSE BADA IDEA — RECURSION KA MAGIC:
//  Folder::getSize() apne har child ka getSize() call karta hai. Child
//  agar File hai to seedha size, agar Folder hai to wo APNE children ka
//  sum karega... aur aise poora tree automatically cover ho jaata hai.
//  Client ko sirf root pe ek call karni hai — baaki pattern sambhal leta hai!
// ============================================================================
#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ============================================================================
//  COMPONENT: common interface — File aur Folder DONO ise implement karte hain
// ----------------------------------------------------------------------------
//  Yahi Composite ka DIL hai — ek hi interface jispe client kaam karta hai,
//  usse pata bhi nahi chalta wo File se baat kar raha hai ya Folder se.
//  "Program to an interface, not an implementation."
// ============================================================================
class FileSystemItem {
public:
  virtual ~FileSystemItem() {}
  virtual void ls(int indent = 0) = 0;       // contents list karo (ek level)
  virtual void openAll(int indent = 0) = 0;  // poora tree recursively kholo
  virtual int getSize() = 0;                 // total size (folder me = sum)
  // cd = change directory: sirf Folder me andar ja sakte hain, File me nahi.
  // (Leaf isko support nahi karti — nullptr deti hai. Ye Composite ka ek
  //  known trade-off hai: kuch operations sirf composite pe meaningful hain.)
  virtual FileSystemItem *cd(const string &name) = 0;
  virtual string getName() = 0;
  virtual bool isFolder() = 0; // client kabhi-kabhi type jaanna chahe (ls me)
};

// Leaf: File — tree ka end node, iske koi children nahi.
class File : public FileSystemItem {
  string name;
  int size;

public:
  File(const string &n, int s) {
    name = n;
    size = s;
  }

  void ls(int indent = 0) override {
    cout << string(indent, ' ') << name << "\n";
  }

  void openAll(int indent = 0) override {
    cout << string(indent, ' ') << name << "\n";
  }

  int getSize() override { return size; } // leaf ka size apna hi

  // File ke andar cd nahi ho sakta -> nullptr.
  FileSystemItem *cd(const string &) override { return nullptr; }

  string getName() override { return name; }

  bool isFolder() override { return false; }
};

// Composite: Folder — ismein doosre FileSystemItem (File ya Folder) ho sakte hain.
class Folder : public FileSystemItem {
  string name;
  vector<FileSystemItem *> children; // mixed: files + sub-folders

public:
  Folder(const string &n) { name = n; }
  ~Folder() {
    // Composite apne saare children ko own karta hai -> destroy bhi karta hai.
    for (auto c : children)
      delete c;
  }

  void add(FileSystemItem *item) { children.push_back(item); }

  void ls(int indent = 0) override {
    for (auto child : children) {
      if (child->isFolder()) {
        cout << string(indent, ' ') << "+ " << child->getName() << "\n";
      } else {
        cout << string(indent, ' ') << child->getName() << "\n";
      }
    }
  }

  // >>> RECURSION KA MAGIC #1 <<<
  // Har child ka openAll() call. Child File hai to bas naam print;
  // child Folder hai to wo APNE children pe openAll() call karega...
  // aur aise poora tree depth-first khul jaata hai. Indent badhta rehta
  // hai (indent+4) taaki nesting visually dikhe.
  void openAll(int indent = 0) override {
    cout << string(indent, ' ') << "+ " << name << "\n";
    for (auto child : children) {
      child->openAll(indent + 4); // yahi call recursion ki jaan hai
    }
  }

  // >>> RECURSION KA MAGIC #2 <<<
  // Folder ka size = saare children ke size ka sum. Folder ko ye nahi
  // sochna padta ki child File hai ya Folder — bas getSize() bol do,
  // polymorphism sahi wala chala dega. File apna size deti, Folder apne
  // children ka sum. Yahi uniform-treatment ka asli fayda hai!
  int getSize() override {
    int total = 0;
    for (auto child : children) {
      total += child->getSize(); // File? seedha size. Folder? andar ka sum.
    }
    return total;
  }

  // Naam se matching sub-folder dhoondo.
  FileSystemItem *cd(const string &target) override {
    for (auto child : children) {
      if (child->isFolder() && child->getName() == target) {
        return child;
      }
    }
    // mila nahi ya folder nahi tha
    return nullptr;
  }

  string getName() override { return name; }
  bool isFolder() override { return true; }
};

int main() {
  // Ek file system tree banate hain (folders ke andar files + folders).
  Folder *root = new Folder("root");
  root->add(new File("file1.txt", 1));
  root->add(new File("file2.txt", 1));

  Folder *docs = new Folder("docs");
  docs->add(new File("resume.pdf", 1));
  docs->add(new File("notes.txt", 1));
  root->add(docs);

  Folder *images = new Folder("images");
  images->add(new File("photo.jpg", 1));
  root->add(images);

  root->ls();   // root ke direct children
  docs->ls();   // docs ke andar
  root->openAll(); // poora tree recursively

  // cd se docs me jaa kar uska content dekho — uniform interface ka faayda.
  FileSystemItem *cwd = root->cd("docs");
  if (cwd != nullptr) {
    cwd->ls();
  } else {
    cout << "\n Could not cd into docs \n";
  }

  cout << root->getSize(); // pure tree ka total size

  // Cleanup: root delete -> ~Folder() apne children delete karta hai ->
  // wo apne children... recursively poora tree free (destructor bhi
  // recursion se chalta hai, jaise operations!).
  delete root;
  return 0;
}

// ============================================================================
//  EXPECTED OUTPUT (roughly):
// ----------------------------------------------------------------------------
//  file1.txt              <- root->ls(): direct children (folders pe "+ ")
//  file2.txt
//  + docs
//  + images
//  resume.pdf             <- docs->ls(): docs ke andar
//  notes.txt
//  + root                 <- root->openAll(): poora tree indent ke saath
//      file1.txt
//      file2.txt
//      + docs
//          resume.pdf
//          notes.txt
//      + images
//          photo.jpg
//  resume.pdf             <- root->cd("docs")->ls(): docs me ghus ke list
//  notes.txt
//  5                      <- root->getSize(): 5 files × size 1 = 5
//
//  Dekho: client ne File aur Folder me kabhi FARQ nahi kiya — same
//  interface se sab hua. Yahi Composite ka pura point! ✅
// ============================================================================
