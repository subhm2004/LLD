#include <bits/stdc++.h>
using namespace std;

// Demo 7: Repo-style LLD — service throws runtime_error / invalid_argument
// Same style as File_Manager_LLD, Library, Parking projects

class FileNotFoundException : public runtime_error {
public:
    explicit FileNotFoundException(const string &path)
        : runtime_error("File not found: " + path), path_(path) {}
    const string &getPath() const { return path_; }

private:
    string path_;
};

class VirtualFileService {
public:
    void createFile(const string &path, const string &content) {
        if (path.empty()) {
            throw invalid_argument("path cannot be empty");
        }
        if (files_.count(path)) {
            throw runtime_error("File already exists: " + path);
        }
        files_[path] = content;
        cout << "Created: " << path << "\n";
    }

    string readFile(const string &path) const {
        auto it = files_.find(path);
        if (it == files_.end()) {
            throw FileNotFoundException(path);
        }
        return it->second;
    }

private:
    unordered_map<string, string> files_;
};

int main() {
    cout << "=== 07 LLD service validation pattern ===\n";

    VirtualFileService service;

    try {
        service.createFile("/notes.txt", "hello");
        cout << "Read: " << service.readFile("/notes.txt") << "\n";
        cout << service.readFile("/missing.txt") << "\n";
    } catch (const FileNotFoundException &ex) {
        cout << "Domain catch: " << ex.what() << " | path=" << ex.getPath() << "\n";
    } catch (const invalid_argument &ex) {
        cout << "Validation: " << ex.what() << "\n";
    } catch (const runtime_error &ex) {
        cout << "Generic runtime: " << ex.what() << "\n";
    }

    return 0;
}
