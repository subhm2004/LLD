# File Manager LLD

In-memory **virtual file system** — common interview LLD (Unix-like commands on a tree).

## Quick run

```bash
cd File_Manager_LLD
./compile.sh
./file_manager_app
```

## Structure

```
File_Manager_LLD/
├── core/FileManagerSystem.h    # Facade — mkdir, cd, ls, cat, mv, cp, find, …
├── models/
│   ├── IFileSystemNode.h
│   ├── FileNode.h
│   └── DirectoryNode.h         # Composite
├── services/
│   ├── PathResolver.h
│   ├── SearchService.h
│   └── CopyService.h           # Deep copy
├── enums/NodeType.h
├── main.cpp
├── problem_statement.md
└── requirements.md
```

## Patterns

| Pattern | Usage |
|---------|--------|
| **Composite** | Directory holds files + subdirectories uniformly |
| **Facade** | `FileManagerSystem` hides path resolution + navigation |

## Related docs

- [`INHERITANCE_AND_COMPOSITION.md`](../L4%20UML_Diagrams/INHERITANCE_AND_COMPOSITION.md) — Composite theory
- [`PROJECT_DESIGN_PATTERNS.md`](../PROJECT_DESIGN_PATTERNS.md) — pattern map

## Interview extensions

- File permissions (`chmod`), shared read lock
- Symlinks and hard links
- `touch`, append, truncate
- Persist tree to JSON / load on startup
- Thread-safe `mutex` per directory
