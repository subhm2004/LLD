// ============================================================================
//  07_lld_service_validation.cpp  —  LLD service layer me validation + errors
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/07_lld_service_validation.cpp" -o bin/07_lld_service_validation
//
//  Ye real LLD/enterprise ka common pattern hai: service ke har public method
//  apne inputs aur state ko VALIDATE karta hai, aur galti pe SAHI TYPE ka
//  exception throw karta hai. Caller (ya facade) type ke hisaab se handle karta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ 3 TARAH KE VALIDATION, 3 TARAH KE EXCEPTION                           │
//  │                                                                          │
//  │   1. Argument galat (khaali path)  -> invalid_argument (logic_error)      │
//  │   2. Business rule toota (duplicate)-> runtime_error                      │
//  │   3. Resource nahi mila (file gayab)-> FileNotFoundException (custom)     │
//  │                                                                          │
//  │  Alag TYPE isliye taaki caller alag-alag response de sake: validation ->  │
//  │  "input theek karo", not-found -> "404 dikhao", business -> "conflict".   │
//  │  Sab ek generic error hote to ye farak karna mushkil hota.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 "FAIL FAST": galat state milte hi turant throw karo (aage badhne se pehle).
//     Isse bug jahan hua wahin pakda jaata hai, corrupt data aage nahi jaata.
//
//  📌 Catch order (file 04 wala rule): custom/specific pehle, generic runtime_error
//     sabse aakhri me (safety net).
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using namespace std;

// Custom domain exception — file na milne par.
class FileNotFoundException : public runtime_error {
public:
    explicit FileNotFoundException(const string &path)
        : runtime_error("File not found: " + path), path_(path) {}

    const string &getPath() const { return path_; } // extra data: kaunsa path

private:
    string path_;
};

// Ek mock file service (memory map = virtual DB).
class VirtualFileService {
public:
    void createFile(const string &path, const string &content) {
        // Validation 1: argument check (invalid_argument = caller ki galti).
        if (path.empty()) {
            throw invalid_argument("path cannot be empty");
        }
        // Validation 2: business rule (duplicate resource).
        if (files_.count(path)) {
            throw runtime_error("File already exists: " + path);
        }
        files_[path] = content;
        cout << "Created: " << path << "\n";
    }

    string readFile(const string &path) const {
        auto it = files_.find(path);
        // Validation 3: resource availability (custom domain exception).
        if (it == files_.end()) {
            throw FileNotFoundException(path);
        }
        return it->second;
    }

private:
    unordered_map<string, string> files_; // virtual file storage
};

int main() {
    cout << "=== 07 LLD service validation pattern ===\n";

    VirtualFileService service;

    try {
        service.createFile("/notes.txt", "hello");                  // OK
        cout << "Read: " << service.readFile("/notes.txt") << "\n"; // OK
        cout << service.readFile("/missing.txt") << "\n";           // throw (not found)
    }
    // ⭐ Custom/specific PEHLE — iske extra data (getPath) use kar sakein.
    catch (const FileNotFoundException &ex) {
        cout << "Domain catch: " << ex.what() << " | path=" << ex.getPath() << "\n";
    }
    catch (const invalid_argument &ex) { // specific standard
        cout << "Validation: " << ex.what() << "\n";
    }
    catch (const runtime_error &ex) { // general (base of the above custom) — safety net
        cout << "Generic runtime: " << ex.what() << "\n";
    }

    return 0;
}

// Expected output:
//   Created: /notes.txt
//   Read: hello
//   Domain catch: File not found: /missing.txt | path=/missing.txt
