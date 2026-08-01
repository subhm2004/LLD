/**
 * 03 — Multithreaded crawl (LC 1242 style).
 */
#include <algorithm>
#include <iostream>

#include "MockHtmlParser.h"
#include "WebCrawler.h"

using namespace std;
using namespace web_crawler;

int main() {
    const string start = "http://leetcode.com/";
    constexpr int kThreads = 3;

    cout << "====== Multithreaded crawl (threads=" << kThreads << ") ======\n";

    MockHtmlParser parser;
    vector<string> urls = crawl(start, kThreads, parser);
    sort(urls.begin(), urls.end());

    cout << "Visited " << urls.size() << " URLs:\n";
    for (const string& u : urls) {
        cout << "  " << u << "\n";
    }

    vector<string> expected = crawlSequential(start, parser);
    sort(expected.begin(), expected.end());
    cout << "\nMatch sequential BFS: " << (urls == expected ? "YES" : "NO") << "\n";
    return 0;
}
