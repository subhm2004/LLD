#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_BOOK_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_BOOK_H

#include <string>
#include <utility>

#include "../enums/BookStatus.h"

namespace library_mgmt {

class Book {
public:
    Book(std::string id, std::string title, std::string author)
        : id_(std::move(id)), title_(std::move(title)), author_(std::move(author)), status_(BookStatus::AVAILABLE) {}

    const std::string &getId() const { return id_; }
    const std::string &getTitle() const { return title_; }
    const std::string &getAuthor() const { return author_; }
    BookStatus getStatus() const { return status_; }

    void setStatus(BookStatus status) { status_ = status; }

private:
    std::string id_;
    std::string title_;
    std::string author_;
    BookStatus status_;
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_BOOK_H
