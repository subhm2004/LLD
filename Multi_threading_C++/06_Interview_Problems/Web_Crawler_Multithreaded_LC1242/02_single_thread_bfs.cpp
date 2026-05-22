/**
 * 02 — Sequential BFS baseline.
 */
#include <algorithm>
#include <iostream>

#include "MockHtmlParser.h"
#include "WebCrawler.h"

using namespace std;
using namespace web_crawler;

int main() {
    const string start = "http://leetcode.com/";
    cout << "====== Sequential crawl ======\n";

    MockHtmlParser parser;
    vector<string> urls = crawlSequential(start, parser);
    sort(urls.begin(), urls.end());

    cout << "Visited " << urls.size() << " URLs:\n";
    for (const string& u : urls) {
        cout << "  " << u << "\n";
    }
    return 0;
}
