// ============================================================================
//  CompositePattern.cpp  —  Composite Design Pattern (Structural)
// ----------------------------------------------------------------------------
//  Composite = "part-whole" tree ko uniform tareeke se treat karna. Yani client
//  ek single object (File = leaf) aur ek group (Folder = composite) ko EK HI
//  interface (FileSystemItem) se use kare — same method `ls()`, `getSize()`...
//  Folder ke andar File bhi ho sakti hai aur dusra Folder bhi (recursion).
//  Yahi reason hai file-systems Composite ka classic example hain.
// ============================================================================
#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Component: common interface — File aur Folder dono ise implement karte hain.
class FileSystemItem {
public:
  virtual ~FileSystemItem() {}
  virtual void ls(int indent = 0) = 0;       // contents list karo
  virtual void openAll(int indent = 0) = 0;  // poora tree recursively kholo
  virtual int getSize() = 0;                 // total size
  // cd = change directory: sirf Folder me andar ja sakte hain, File me nahi.
  virtual FileSystemItem *cd(const string &name) = 0;
  virtual string getName() = 0;
  virtual bool isFolder() = 0;
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

  // Recursion: har child ka openAll call -> poora tree khul jaata hai.
  void openAll(int indent = 0) override {
    cout << string(indent, ' ') << "+ " << name << "\n";
    for (auto child : children) {
      child->openAll(indent + 4);
    }
  }

  // Folder ka size = saare children ke size ka sum (recursively).
  int getSize() override {
    int total = 0;
    for (auto child : children) {
      total += child->getSize();
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

  // Cleanup: root delete -> recursively saare children delete.
  delete root;
  return 0;
}
