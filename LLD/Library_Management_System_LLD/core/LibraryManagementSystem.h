#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_CORE_LIBRARYMANAGEMENTSYSTEM_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_CORE_LIBRARYMANAGEMENTSYSTEM_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../enums/AccountStatus.h"
#include "../enums/BookStatus.h"
#include "../models/Loan.h"
#include "../models/Member.h"
#include "../services/CatalogService.h"
#include "../services/FineService.h"

namespace library_mgmt {

class LibraryManagementSystem {
public:
    LibraryManagementSystem() : loanCounter_(0), maxBorrowLimit_(3), loanDurationDays_(14) {}

    void addBook(const Book &book) {
        catalogService_.addBook(book);
    }

    void registerMember(const std::string &memberId, const std::string &name) {
        if (members_.count(memberId)) {
            throw std::runtime_error("Member already exists");
        }
        members_.emplace(memberId, Member(memberId, name));
    }

    std::string issueBook(const std::string &memberId, const std::string &bookId, int currentDay) {
        Member *member = getMemberOrThrow(memberId);
        Book *book = getBookOrThrow(bookId);

        if (member->getStatus() != AccountStatus::ACTIVE) {
            throw std::runtime_error("Member account is not active");
        }
        if (member->getBorrowedCount() >= maxBorrowLimit_) {
            throw std::runtime_error("Borrow limit reached");
        }
        if (book->getStatus() != BookStatus::AVAILABLE) {
            throw std::runtime_error("Book is not available");
        }

        const std::string loanId = "LOAN_" + std::to_string(++loanCounter_);
        loans_.emplace(loanId, Loan(loanId, bookId, memberId, currentDay, currentDay + loanDurationDays_));

        book->setStatus(BookStatus::LOANED);
        member->incrementBorrowedCount();

        std::cout << "Loan created: " << loanId << " | Book: " << book->getTitle() << " | Member: " << member->getName() << "\n";
        return loanId;
    }

    double returnBook(const std::string &loanId, int returnedDay) {
        Loan *loan = getLoanOrThrow(loanId);
        if (loan->isReturned()) {
            throw std::runtime_error("Book already returned");
        }

        Book *book = getBookOrThrow(loan->getBookId());
        Member *member = getMemberOrThrow(loan->getMemberId());

        loan->markReturned(returnedDay);
        book->setStatus(BookStatus::AVAILABLE);
        member->decrementBorrowedCount();

        const double fine = fineService_.calculateFine(loan->getDueDay(), returnedDay);
        std::cout << "Book returned: " << book->getTitle() << " | Fine: " << fine << "\n";
        return fine;
    }

    std::vector<Book *> searchBooksByTitle(const std::string &title) {
        return catalogService_.searchByTitle(title);
    }

private:
    CatalogService catalogService_;
    FineService fineService_;
    std::unordered_map<std::string, Member> members_;
    std::unordered_map<std::string, Loan> loans_;
    int loanCounter_;
    int maxBorrowLimit_;
    int loanDurationDays_;

    Member *getMemberOrThrow(const std::string &memberId) {
        auto it = members_.find(memberId);
        if (it == members_.end()) {
            throw std::runtime_error("Member not found");
        }
        return &it->second;
    }

    Book *getBookOrThrow(const std::string &bookId) {
        Book *book = catalogService_.getBookById(bookId);
        if (book == nullptr) {
            throw std::runtime_error("Book not found");
        }
        return book;
    }

    Loan *getLoanOrThrow(const std::string &loanId) {
        auto it = loans_.find(loanId);
        if (it == loans_.end()) {
            throw std::runtime_error("Loan not found");
        }
        return &it->second;
    }
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_CORE_LIBRARYMANAGEMENTSYSTEM_H
