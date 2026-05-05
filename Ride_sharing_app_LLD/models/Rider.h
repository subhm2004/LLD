#ifndef RIDE_SHARING_APP_LLD_MODELS_RIDER_H
#define RIDE_SHARING_APP_LLD_MODELS_RIDER_H

#include <string>
#include <utility>

using namespace std;

namespace ride_sharing {

class Rider {
public:
    Rider(string riderId, string name)
        : riderId_(std::move(riderId)), name_(std::move(name)) {}

    const string &getId() const { return riderId_; }
    const string &getName() const { return name_; }

private:
    string riderId_;
    string name_;
};

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_MODELS_RIDER_H
