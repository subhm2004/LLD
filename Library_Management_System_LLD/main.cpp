#include <iostream>

#include "core/LibraryManagementSystem.h"
#include "models/Book.h"

using namespace library_mgmt;

int main() {
    LibraryManagementSystem system;

    system.addBook(Book("B1", "Clean Code", "Robert C. Martin"));
    system.addBook(Book("B2", "Design Patterns", "GoF"));
    system.addBook(Book("B3", "Clean Code", "Robert C. Martin"));

    system.registerMember("M1", "Shubham");

    std::cout << "Search result for title 'Clean Code':\n";
    std::vector<Book *> matches = system.searchBooksByTitle("Clean Code");
    for (Book *book : matches) {
        std::cout << "BookId: " << book->getId() << " | Author: " << book->getAuthor() << "\n";
    }

    std::string loanId = system.issueBook("M1", "B1", 1);
    double fine = system.returnBook(loanId, 20);

    std::cout << "Final fine charged: " << fine << "\n";
    return 0;
}
