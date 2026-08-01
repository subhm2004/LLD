#ifndef MULTI_THREADING_WEB_CRAWLER_WEBCRAWLER_H
#define MULTI_THREADING_WEB_CRAWLER_WEBCRAWLER_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "MockHtmlParser.h"
#include "UrlUtils.h"

namespace web_crawler {

/** LeetCode 1242 — multithreaded BFS crawl, same hostname only. */
inline std::vector<std::string> crawl(const std::string& startUrl, int threadCount,
                                     HtmlParser& parser) {
    const std::string host = getHostname(startUrl);
    std::unordered_set<std::string> visited;
    std::queue<std::string> pending;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<int> active{0};
    bool finished = false;

    visited.insert(startUrl);
    pending.push(startUrl);

    auto worker = [&]() {
        while (true) {
            std::string url;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return finished || !pending.empty(); });
                if (finished && pending.empty()) {
                    return;
                }
                if (pending.empty()) {
                    continue;
                }
                url = pending.front();
                pending.pop();
                ++active;
            }

            std::vector<std::string> neighbors = parser.getUrls(url);
            std::vector<std::string> to_enqueue;
            for (const std::string& next : neighbors) {
                if (!isSameHost(next, host)) {
                    continue;
                }
                std::lock_guard<std::mutex> lock(mtx);
                if (visited.insert(next).second) {
                    to_enqueue.push_back(next);
                }
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                for (const std::string& u : to_enqueue) {
                    pending.push(u);
                }
                --active;
                if (pending.empty() && active == 0) {
                    finished = true;
                }
                cv.notify_all();
            }
        }
    };

    std::vector<std::thread> pool;
    pool.reserve(static_cast<size_t>(threadCount));
    for (int i = 0; i < threadCount; ++i) {
        pool.emplace_back(worker);
    }
    for (auto& t : pool) {
        t.join();
    }

    return std::vector<std::string>(visited.begin(), visited.end());
}

/** Single-threaded BFS baseline (same result, easier to debug). */
inline std::vector<std::string> crawlSequential(const std::string& startUrl,
                                                HtmlParser& parser) {
    const std::string host = getHostname(startUrl);
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;
    visited.insert(startUrl);
    q.push(startUrl);

    while (!q.empty()) {
        std::string url = q.front();
        q.pop();
        for (const std::string& next : parser.getUrls(url)) {
            if (!isSameHost(next, host)) {
                continue;
            }
            if (visited.insert(next).second) {
                q.push(next);
            }
        }
    }
    return std::vector<std::string>(visited.begin(), visited.end());
}

}  // namespace web_crawler

#endif  // MULTI_THREADING_WEB_CRAWLER_WEBCRAWLER_H
