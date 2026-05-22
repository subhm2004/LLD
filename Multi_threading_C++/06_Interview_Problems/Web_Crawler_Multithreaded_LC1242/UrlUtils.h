#ifndef MULTI_THREADING_WEB_CRAWLER_URLUTILS_H
#define MULTI_THREADING_WEB_CRAWLER_URLUTILS_H

#include <string>

namespace web_crawler {

/** Extract hostname from http://hostname/path (LC 1242 same-host rule). */
inline std::string getHostname(const std::string& url) {
    const std::string prefix = "http://";
    if (url.size() <= prefix.size()) {
        return "";
    }
    size_t start = prefix.size();
    size_t end = url.find('/', start);
    if (end == std::string::npos) {
        return url.substr(start);
    }
    return url.substr(start, end - start);
}

inline bool isSameHost(const std::string& url, const std::string& startHost) {
    return getHostname(url) == startHost;
}

}  // namespace web_crawler

#endif  // MULTI_THREADING_WEB_CRAWLER_URLUTILS_H
