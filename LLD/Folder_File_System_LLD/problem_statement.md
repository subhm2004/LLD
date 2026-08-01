# Problem Statement: Folder File System LLD

Design an in-memory folder/file system that supports:

- `mkdir` to create directories
- `cd` to navigate directories
- `rmdir` to remove empty directories
- `size` to fetch file or folder size
- file size updates where parent directory sizes reflect changes instantly

The design should separate responsibilities across entities and keep command handling simple for interview discussion and extension.
