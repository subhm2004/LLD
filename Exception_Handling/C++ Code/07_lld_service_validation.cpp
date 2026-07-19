#include <bits/stdc++.h>
using namespace std;

// Demo 7: Standard LLD Service validation pattern — errors handling in service layers
// Ye pattern standard enterprise applications aur complex LLD tasks me implement kiya jata hai.

/**
 * @class FileNotFoundException
 * @brief Custom domain exception jo file miss hone par throw hota hai.
 */
class FileNotFoundException : public runtime_error {
public:
    explicit FileNotFoundException(const string &path)
        : runtime_error("File not found: " + path), path_(path) {}
    
    const string &getPath() const { return path_; }

private:
    string path_;
};

/**
 * @class VirtualFileService
 * @brief Memory mapping ke structure par files save aur retrieve karne wali mock service.
 */
class VirtualFileService {
public:
    // File create karne ka logic check valid path ke saath.
    void createFile(const string &path, const string &content) {
        // Validation check 1: invalid arguments test.
        if (path.empty()) {
            throw invalid_argument("path cannot be empty");
        }
        // Validation check 2: duplicate resource check.
        if (files_.count(path)) {
            throw runtime_error("File already exists: " + path);
        }
        files_[path] = content;
        cout << "Created: " << path << "\n";
    }

    // File content read logic.
    string readFile(const string &path) const {
        auto it = files_.find(path);
        // Validation check 3: Resource availability check.
        if (it == files_.end()) {
            throw FileNotFoundException(path); // domain specific error
        }
        return it->second;
    }

private:
    unordered_map<string, string> files_; // File storage map (virtual DB memory)
};

int main() {
    cout << "=== 07 LLD service validation pattern ===\n";

    VirtualFileService service;

    try {
        // Normal Flow Execution.
        service.createFile("/notes.txt", "hello");
        cout << "Read: " << service.readFile("/notes.txt") << "\n";
        
        // Exceptional Flow: file missing.
        cout << service.readFile("/missing.txt") << "\n";
    } 
    // Domain Specific Exception handling (highly detailed error reporting)
    catch (const FileNotFoundException &ex) {
        cout << "Domain catch: " << ex.what() << " | path=" << ex.getPath() << "\n";
    } 
    // Standard Parameter mismatch check.
    catch (const invalid_argument &ex) {
        cout << "Validation: " << ex.what() << "\n";
    } 
    // Baki runtime errors handling.
    catch (const runtime_error &ex) {
        cout << "Generic runtime: " << ex.what() << "\n";
    }

    return 0;
}
