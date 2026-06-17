#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class NodeType { FILE_NODE, DIRECTORY_NODE };

class Directory;

class FsNode {
public:
    FsNode(string name, NodeType type, Directory *parent)
        : name_(std::move(name)), type_(type), parent_(parent), sizeBytes_(0) {}
    virtual ~FsNode() = default;

    const string &name() const { return name_; }
    NodeType type() const { return type_; }
    long long sizeBytes() const { return sizeBytes_; }
    Directory *parent() const { return parent_; }
    void setParent(Directory *parent) { parent_ = parent; }

    virtual bool isDirectory() const = 0;

protected:
    void setSizeDirect(long long newSize) { sizeBytes_ = newSize; }
    void addSizeDirect(long long delta) { sizeBytes_ += delta; }

private:
    string name_;
    NodeType type_;
    Directory *parent_;
    long long sizeBytes_;
};

class FileNode : public FsNode {
public:
    FileNode(string name, long long sizeBytes, Directory *parent)
        : FsNode(std::move(name), NodeType::FILE_NODE, parent) {
        setSizeDirect(sizeBytes);
    }

    bool isDirectory() const override { return false; }

    void resize(long long newSizeBytes) { setSizeDirect(newSizeBytes); }
};

class Directory : public FsNode {
public:
    explicit Directory(string name, Directory *parent) : FsNode(std::move(name), NodeType::DIRECTORY_NODE, parent) {}

    bool isDirectory() const override { return true; }

    bool hasChild(const string &name) const { return children_.count(name) != 0; }

    FsNode *getChild(const string &name) const {
        auto it = children_.find(name);
        return it == children_.end() ? nullptr : it->second.get();
    }

    vector<FsNode *> listChildren() const {
        vector<FsNode *> result;
        result.reserve(children_.size());
        for (const auto &entry : children_) {
            result.push_back(entry.second.get());
        }
        return result;
    }

    bool addChild(unique_ptr<FsNode> child) {
        const string key = child->name();
        if (hasChild(key)) {
            return false;
        }
        child->setParent(this);
        const long long childSize = child->sizeBytes();
        children_[key] = std::move(child);
        updateSizeRecursively(childSize);
        return true;
    }

    bool removeChild(const string &name) {
        auto it = children_.find(name);
        if (it == children_.end()) {
            return false;
        }
        const long long childSize = it->second->sizeBytes();
        children_.erase(it);
        updateSizeRecursively(-childSize);
        return true;
    }

    void updateSizeRecursively(long long delta) {
        addSizeDirect(delta);
        if (parent() != nullptr) {
            parent()->updateSizeRecursively(delta);
        }
    }

private:
    unordered_map<string, unique_ptr<FsNode>> children_;
};

class FileSystem {
public:
    FileSystem() {
        root_ = make_unique<Directory>("/", nullptr);
        cwd_ = root_.get();
    }

    bool mkdir(const string &name) {
        if (!isValidName(name) || cwd_->hasChild(name)) {
            return false;
        }
        return cwd_->addChild(make_unique<Directory>(name, cwd_));
    }

    bool mkfile(const string &name, long long sizeBytes) {
        if (!isValidName(name) || sizeBytes < 0 || cwd_->hasChild(name)) {
            return false;
        }
        return cwd_->addChild(make_unique<FileNode>(name, sizeBytes, cwd_));
    }

    bool resizeFile(const string &name, long long newSize) {
        if (newSize < 0) {
            return false;
        }
        FsNode *node = cwd_->getChild(name);
        if (node == nullptr || node->isDirectory()) {
            return false;
        }
        auto *file = dynamic_cast<FileNode *>(node);
        const long long oldSize = file->sizeBytes();
        file->resize(newSize);
        cwd_->updateSizeRecursively(newSize - oldSize);
        return true;
    }

    bool rmfile(const string &name) {
        FsNode *node = cwd_->getChild(name);
        if (node == nullptr || node->isDirectory()) {
            return false;
        }
        return cwd_->removeChild(name);
    }

    bool rmdir(const string &name) {
        FsNode *node = cwd_->getChild(name);
        if (node == nullptr || !node->isDirectory()) {
            return false;
        }
        auto *dir = dynamic_cast<Directory *>(node);
        if (!dir->listChildren().empty()) {
            return false; // safe delete: only empty directory
        }
        return cwd_->removeChild(name);
    }

    bool cd(const string &path) {
        Directory *target = resolveDirectory(path);
        if (target == nullptr) {
            return false;
        }
        cwd_ = target;
        return true;
    }

    long long size(const string &path = ".") const {
        FsNode *node = resolveNode(path);
        if (node == nullptr) {
            return -1;
        }
        return node->sizeBytes();
    }

    void ls() const {
        vector<FsNode *> nodes = cwd_->listChildren();
        cout << "Listing: " << pwd() << "\n";
        if (nodes.empty()) {
            cout << "  (empty)\n";
            return;
        }
        for (FsNode *node : nodes) {
            cout << "  " << (node->isDirectory() ? "[DIR]  " : "[FILE] ") << node->name() << "  size=" << node->sizeBytes()
                 << "\n";
        }
    }

    string pwd() const {
        if (cwd_ == root_.get()) {
            return "/";
        }
        vector<string> parts;
        const Directory *curr = cwd_;
        while (curr != nullptr && curr != root_.get()) {
            parts.push_back(curr->name());
            curr = curr->parent();
        }
        string result;
        for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
            result += "/" + *it;
        }
        return result.empty() ? "/" : result;
    }

private:
    unique_ptr<Directory> root_;
    Directory *cwd_;

    static bool isValidName(const string &name) {
        return !name.empty() && name.find('/') == string::npos && name != "." && name != "..";
    }

    FsNode *resolveNode(const string &path) const {
        if (path.empty()) {
            return nullptr;
        }
        if (path == ".") {
            return cwd_;
        }
        if (path == "/") {
            return root_.get();
        }

        Directory *start = (path[0] == '/') ? root_.get() : cwd_;
        FsNode *current = start;

        vector<string> parts = splitPath(path);
        for (const string &part : parts) {
            if (part == ".") {
                continue;
            }
            if (part == "..") {
                if (current->isDirectory()) {
                    auto *currDir = dynamic_cast<Directory *>(current);
                    current = (currDir->parent() == nullptr) ? currDir : currDir->parent();
                }
                continue;
            }
            if (!current->isDirectory()) {
                return nullptr;
            }
            auto *currDir = dynamic_cast<Directory *>(current);
            current = currDir->getChild(part);
            if (current == nullptr) {
                return nullptr;
            }
        }
        return current;
    }

    Directory *resolveDirectory(const string &path) const {
        FsNode *node = resolveNode(path);
        if (node == nullptr || !node->isDirectory()) {
            return nullptr;
        }
        return dynamic_cast<Directory *>(node);
    }

    static vector<string> splitPath(const string &path) {
        vector<string> parts;
        string token;
        stringstream ss(path);
        while (getline(ss, token, '/')) {
            if (!token.empty()) {
                parts.push_back(token);
            }
        }
        return parts;
    }
};

static void printHelp() {
    cout << "Commands:\n";
    cout << "  mkdir <name>\n";
    cout << "  mkfile <name> <sizeBytes>\n";
    cout << "  resize <name> <newSizeBytes>\n";
    cout << "  rmfile <name>\n";
    cout << "  rmdir <name>\n";
    cout << "  cd <path>\n";
    cout << "  ls\n";
    cout << "  pwd\n";
    cout << "  size [path]\n";
    cout << "  help\n";
    cout << "  exit\n";
}

int main() {
    FileSystem fs;
    cout << "Folder File System LLD Demo\n";
    printHelp();

    string line;
    while (true) {
        cout << "\n" << fs.pwd() << " $ ";
        if (!getline(cin, line)) {
            break;
        }
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") {
            break;
        }
        if (cmd == "help") {
            printHelp();
            continue;
        }
        if (cmd == "mkdir") {
            string name;
            ss >> name;
            cout << (fs.mkdir(name) ? "OK" : "FAILED") << "\n";
            continue;
        }
        if (cmd == "mkfile") {
            string name;
            long long sizeBytes = -1;
            ss >> name >> sizeBytes;
            cout << (fs.mkfile(name, sizeBytes) ? "OK" : "FAILED") << "\n";
            continue;
        }
        if (cmd == "resize") {
            string name;
            long long sizeBytes = -1;
            ss >> name >> sizeBytes;
            cout << (fs.resizeFile(name, sizeBytes) ? "OK" : "FAILED") << "\n";
            continue;
        }
        if (cmd == "rmfile") {
            string name;
            ss >> name;
            cout << (fs.rmfile(name) ? "OK" : "FAILED") << "\n";
            continue;
        }
        if (cmd == "rmdir") {
            string name;
            ss >> name;
            cout << (fs.rmdir(name) ? "OK" : "FAILED (dir missing or non-empty)") << "\n";
            continue;
        }
        if (cmd == "cd") {
            string path;
            ss >> path;
            cout << (fs.cd(path) ? "OK" : "FAILED") << "\n";
            continue;
        }
        if (cmd == "ls") {
            fs.ls();
            continue;
        }
        if (cmd == "pwd") {
            cout << fs.pwd() << "\n";
            continue;
        }
        if (cmd == "size") {
            string path;
            ss >> path;
            long long out = path.empty() ? fs.size(".") : fs.size(path);
            if (out < 0) {
                cout << "FAILED\n";
            } else {
                cout << out << "\n";
            }
            continue;
        }

        cout << "Unknown command. Type: help\n";
    }

    return 0;
}
