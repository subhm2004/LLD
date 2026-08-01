#ifndef MULTI_THREADING_WEB_CRAWLER_MOCKHTMLPARSER_H
#define MULTI_THREADING_WEB_CRAWLER_MOCKHTMLPARSER_H

#include <string>
#include <unordered_map>
#include <vector>

namespace web_crawler {

/** LeetCode 1242 HtmlParser interface (mock in-memory graph). */
class HtmlParser {
public:
    virtual ~HtmlParser() = default;
    virtual std::vector<std::string> getUrls(const std::string& url) = 0;
};

class MockHtmlParser : public HtmlParser {
public:
    MockHtmlParser() {
        graph_["http://leetcode.com/"] = {
            "http://leetcode.com/problems/",
            "http://leetcode.com/contest/",
            "http://google.com/about"};
        graph_["http://leetcode.com/problems/"] = {
            "http://leetcode.com/problems/foo/",
            "http://leetcode.com/discuss/"};
        graph_["http://leetcode.com/contest/"] = {"http://leetcode.com/contest/weekly/"};
        graph_["http://leetcode.com/problems/foo/"] = {"http://leetcode.com/problems/bar/"};
        graph_["http://leetcode.com/problems/bar/"] = {"http://leetcode.com/"};
        graph_["http://leetcode.com/discuss/"] = {};
        graph_["http://leetcode.com/contest/weekly/"] = {};
    }

    std::vector<std::string> getUrls(const std::string& url) override {
        auto it = graph_.find(url);
        if (it == graph_.end()) {
            return {};
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> graph_;
};

}  // namespace web_crawler

#endif  // MULTI_THREADING_WEB_CRAWLER_MOCKHTMLPARSER_H
