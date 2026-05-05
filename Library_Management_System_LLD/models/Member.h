#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_MEMBER_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_MEMBER_H

#include <string>
#include <utility>

#include "../enums/AccountStatus.h"

namespace library_mgmt {

class Member {
public:
    Member(std::string id, std::string name)
        : id_(std::move(id)), name_(std::move(name)), status_(AccountStatus::ACTIVE), borrowedCount_(0) {}

    const std::string &getId() const { return id_; }
    const std::string &getName() const { return name_; }
    AccountStatus getStatus() const { return status_; }
    int getBorrowedCount() const { return borrowedCount_; }

    void setStatus(AccountStatus status) { status_ = status; }
    void incrementBorrowedCount() { ++borrowedCount_; }
    void decrementBorrowedCount() { if (borrowedCount_ > 0) { --borrowedCount_; } }

private:
    std::string id_;
    std::string name_;
    AccountStatus status_;
    int borrowedCount_;
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_MODELS_MEMBER_H
