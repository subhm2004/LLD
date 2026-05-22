/**
 * 04 — Same graph, threadCount 1 vs 4 (result set must match).
 */
#include <algorithm>
#include <chrono>
#include <iostream>

#include "MockHtmlParser.h"
#include "WebCrawler.h"

using namespace std;
using namespace web_crawler;

int main() {
    const string start = "http://leetcode.com/";
    MockHtmlParser parser;

    auto run = [&](int threads) {
        auto t0 = chrono::steady_clock::now();
        vector<string> urls = crawl(start, threads, parser);
        auto ms = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0)
                      .count();
        sort(urls.begin(), urls.end());
        cout << "  threads=" << threads << " urls=" << urls.size() << " time_ms=" << ms << "\n";
        return urls;
    };

    cout << "====== Crawl thread count comparison ======\n";
    vector<string> one = run(1);
    vector<string> four = run(4);

    cout << "Same URL set: " << (one == four ? "YES" : "NO") << "\n";
    return 0;
}
