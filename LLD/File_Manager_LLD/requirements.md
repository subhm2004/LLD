# Requirements — File Manager LLD

## Entities

| Entity | Description |
|--------|-------------|
| `IFileSystemNode` | Common interface for file and directory |
| `FileNode` | Leaf — name + string content |
| `DirectoryNode` | Composite — map of child name → node |
| `FileManagerSystem` | Facade — all user commands |

## Services

| Service | Responsibility |
|---------|----------------|
| `PathResolver` | Tokenize, normalize `..`, join paths |
| `SearchService` | DFS find by name |
| `CopyService` | Deep clone subtree |

## API (Facade)

| Method | Description |
|--------|-------------|
| `mkdir(path)` | Create directory path |
| `createFile(path, content)` | Create file |
| `cd(path)` | Change cwd |
| `pwd()` | Current path string |
| `ls(path?)` | List entries |
| `cat(path)` | File content |
| `rm(path)` | Delete node |
| `mv(src, dest)` | Move / rename file |
| `cp(src, dest)` | Deep copy |
| `find(name)` | All matching paths |
| `getSize(path)` | Recursive size |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Composite** | `DirectoryNode` contains `FileNode` + child `DirectoryNode` |
| **Facade** | `FileManagerSystem` |
| **Strategy** (future) | `ISearchStrategy`, `IStorageBackend` |

## Assumptions

- Single user, single process.
- File names unique within one directory.
- Text files only (string content).
- Root cannot be deleted.
