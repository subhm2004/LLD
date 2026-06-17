# Requirements: Folder File System LLD

## Functional Requirements

1. Support folder creation using `mkdir`.
2. Support directory navigation using `cd` (relative and absolute paths).
3. Support directory removal using `rmdir` (only empty directories).
4. Support `size` query for file or directory.
5. Support file creation and resize; directory sizes must reflect changes immediately.

## Non-Functional Requirements

1. Keep operations modular with clear entities (`FsNode`, `Directory`, `FileNode`, `FileSystem`).
2. Keep implementation interview-friendly and extensible.
3. Maintain consistent aggregate size updates in O(depth) for parent chain propagation.

## Assumptions

1. In-memory system only; no disk persistence.
2. Name collisions are not allowed within the same directory.
3. Paths support `.`, `..`, and `/` root.
