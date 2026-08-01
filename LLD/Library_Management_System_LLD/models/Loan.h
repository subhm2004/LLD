#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_LOAN_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_LOAN_H

#include <string>
#include <utility>

namespace library_mgmt {

class Loan {
public:
    Loan(std::string id, std::string bookId, std::string memberId, int issuedDay, int dueDay)
        : id_(std::move(id)),
          bookId_(std::move(bookId)),
          memberId_(std::move(memberId)),
          issuedDay_(issuedDay),
          dueDay_(dueDay),
          returned_(false),
          returnedDay_(-1) {}

    const std::string &getId() const { return id_; }
    const std::string &getBookId() const { return bookId_; }
    const std::string &getMemberId() const { return memberId_; }
    int getIssuedDay() const { return issuedDay_; }
    int getDueDay() const { return dueDay_; }
    bool isReturned() const { return returned_; }
    int getReturnedDay() const { return returnedDay_; }

    void markReturned(int returnedDay) {
        returned_ = true;
        returnedDay_ = returnedDay;
    }

private:
    std::string id_;
    std::string bookId_;
    std::string memberId_;
    int issuedDay_;
    int dueDay_;
    bool returned_;
    int returnedDay_;
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_LOAN_H
