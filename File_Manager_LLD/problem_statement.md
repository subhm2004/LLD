# Problem Statement: File Manager (Virtual File System)

Design a low-level **in-memory file manager** that simulates a Unix-like virtual file system.

## Functional requirements

1. **Directories & files** — tree structure; root at `/`.
2. **mkdir** — create directory (create missing parents optional).
3. **createFile** — create a text file with content.
4. **ls** — list current or given directory (`file` vs `dir/`).
5. **cd** — change working directory (support `.`, `..`, absolute `/path`).
6. **pwd** — print current path.
7. **cat** — read file content.
8. **rm** — remove file or directory (recursive for non-empty dir in this demo).
9. **mv** — move file/dir to another path (file rename via destination leaf name).
10. **cp** — deep copy file or directory subtree.
11. **find** — search by file/dir name in entire tree.
12. **getSize** — byte size (file = content length; dir = sum of children).

## Non-functional

- Modular packages: `models/`, `services/`, `core/`.
- Extensible for: permissions, symlinks, persistence, async I/O.
- C++17, in-memory only for interview demo.

## Out of scope (extensions)

- Real disk I/O, networking, concurrent writers, encryption.
