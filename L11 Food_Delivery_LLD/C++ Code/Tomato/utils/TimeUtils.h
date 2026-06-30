// TimeUtils.h — Chhota helper: current time / formatted time strings dene ke
// liye. Now vs Scheduled orders ke timestamps yahin se aate hain.
#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <ctime>
#include <string>
using namespace std;

class TimeUtils {
public:
    static string getCurrentTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string s(dt);
        if (!s.empty() && s.back() == '\n')
            s.pop_back();
        return s;
    }
};

#endif // TIME_UTILS_H
