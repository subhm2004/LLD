#include <bits/stdc++.h>

#include "core/FileManagerSystem.h"

using namespace file_manager;

int main() {
    FileManagerSystem fm;

    std::cout << "=== File Manager LLD Demo ===\n\n";

    fm.mkdir("/home");
    fm.mkdir("/home/docs");
    fm.mkdir("/home/media");

    fm.createFile("/home/docs/readme.txt", "Welcome to File Manager LLD");
    fm.createFile("/home/docs/notes.txt", "Composite pattern: Directory contains files and folders");
    fm.createFile("/home/media/song.txt", "demo track");

    fm.cd("/home/docs");
    fm.printLs();

    std::cout << "\n--- cat readme.txt ---\n";
    fm.printCat("readme.txt");

    fm.cd("/");
    fm.cp("/home/docs/readme.txt", "/home/media/readme_copy.txt");
    fm.mv("/home/docs/notes.txt", "/home/media/notes.txt");

    std::cout << "\n--- find 'readme.txt' ---\n";
    fm.printFind("readme.txt");

    std::cout << "\n--- sizes ---\n";
    std::cout << "Size /home/docs: " << fm.getSize("/home/docs") << " bytes\n";
    std::cout << "Size /home: " << fm.getSize("/home") << " bytes\n";

    fm.cd("/home/media");
    fm.printLs();

    fm.rm("song.txt");
    std::cout << "\nAfter rm song.txt:\n";
    fm.printLs();

    std::cout << "\nPWD: " << fm.pwd() << "\n";
    return 0;
}
