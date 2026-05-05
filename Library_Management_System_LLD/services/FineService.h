#ifndef LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_FINESERVICE_H
#define LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_FINESERVICE_H

namespace library_mgmt {

class FineService {
public:
    double calculateFine(int dueDay, int returnedDay) const {
        if (returnedDay <= dueDay) {
            return 0.0;
        }
        const int delayedDays = returnedDay - dueDay;
        const double finePerDay = 5.0;
        return delayedDays * finePerDay;
    }
};

} // namespace library_mgmt

#endif // LIBRARY_MANAGEMENT_SYSTEM_LLD_SERVICES_FINESERVICE_H
