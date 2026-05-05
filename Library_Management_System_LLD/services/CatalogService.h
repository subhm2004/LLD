#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_CATALOGSERVICE_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_CATALOGSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Book.h"

namespace library_mgmt {

class CatalogService {
public:
    void addBook(const Book &book) {
        booksById_.insert_or_assign(book.getId(), book);
        booksByTitle_[book.getTitle()].push_back(book.getId());
    }

    Book *getBookById(const std::string &bookId) {
        auto it = booksById_.find(bookId);
        if (it == booksById_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    std::vector<Book *> searchByTitle(const std::string &title) {
        std::vector<Book *> result;
        auto titleIt = booksByTitle_.find(title);
        if (titleIt == booksByTitle_.end()) {
            return result;
        }

        for (const std::string &id : titleIt->second) {
            Book *book = getBookById(id);
            if (book != nullptr) {
                result.push_back(book);
            }
        }
        return result;
    }

private:
    std::unordered_map<std::string, Book> booksById_;
    std::unordered_map<std::string, std::vector<std::string>> booksByTitle_;
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_CATALOGSERVICE_H
