# Web Crawler Multithreaded — LeetCode 1242

BFS crawl from `startUrl` — only **same hostname** links; `threadCount` worker threads share a task queue.

| # | File | Topic |
|---|------|-------|
| 01 | `01_mock_graph_overview.cpp` | Mock `HtmlParser` graph + host filter |
| 02 | `02_single_thread_bfs.cpp` | Sequential BFS baseline |
| 03 | `03_multithreaded_crawl.cpp` | Worker pool + `visited` set (mutex) |
| 04 | `04_compare_thread_counts.cpp` | 1 vs 4 threads — same URL set |

## Run

```bash
./compile.sh && ./bin/03_multithreaded_crawl
```

## Headers

| File | Role |
|------|------|
| `MockHtmlParser.h` | In-memory link graph (replaces LeetCode `HtmlParser`) |
| `WebCrawler.h` | `crawl()` + `crawlSequential()` |
| `UrlUtils.h` | `getHostname()`, `isSameHost()` |

## Interview points

| Piece | Why |
|-------|-----|
| `visited` + mutex | No duplicate crawl |
| `queue` + `cv` | Workers block when empty; shutdown when idle |
| `active` counter | Know when all workers finished current URL |
| Same-host filter | LC requirement — ignore external domains |

## Related

Producer-consumer queue: [`../Bounded_Blocking_Queue/`](../Bounded_Blocking_Queue/)
