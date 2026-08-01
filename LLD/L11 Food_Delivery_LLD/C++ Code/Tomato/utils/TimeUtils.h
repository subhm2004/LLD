// ============================================================================
//  TimeUtils.h — Chhota time helper (current time string)
// ----------------------------------------------------------------------------
//  getCurrentTime() abhi ka time ek readable string me deta hai (ctime se,
//  trailing newline hata ke). NowOrderFactory isse use karta hai order ka
//  scheduled time set karne ke liye. Static utility — koi state nahi.
// ============================================================================
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
