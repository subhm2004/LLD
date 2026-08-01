/**
 * 01 — Mock crawl graph + hostname filter explanation.
 */
#include <iostream>

#include "MockHtmlParser.h"
#include "UrlUtils.h"

using namespace std;
using namespace web_crawler;

int main() {
    cout << "====== Web Crawler LC 1242 — Mock graph ======\n\n";

    MockHtmlParser parser;
    const string start = "http://leetcode.com/";
    cout << "Start: " << start << " (host=" << getHostname(start) << ")\n\n";

    cout << "Outgoing links from start:\n";
    for (const string& u : parser.getUrls(start)) {
        cout << "  " << u << (isSameHost(u, getHostname(start)) ? " [same host]" : " [SKIP]")
             << "\n";
    }

    cout << "\nRule: only crawl URLs with same hostname as startUrl.\n";
    cout << "google.com link above is filtered out during crawl.\n";
    return 0;
}
