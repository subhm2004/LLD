#!/usr/bin/env python3
"""Generate FILE_STRUCTURE.md in every project folder."""

from __future__ import annotations

import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT_NAME = "FILE_STRUCTURE.md"

SKIP_DIR_NAMES = {
    ".git",
    ".cache",
    "node_modules",
    ".clangd",
    "__pycache__",
}

# Compiled artifacts / IDE noise — skip documenting these folders entirely.
SKIP_DIR_NAMES_WALK = SKIP_DIR_NAMES | {"bin"}


def should_skip_dir(name: str) -> bool:
    return name in SKIP_DIR_NAMES_WALK


def file_role(name: str) -> str:
    lower = name.lower()
    ext = Path(name).suffix.lower()
    roles = {
        ".h": "Header",
        ".hpp": "Header",
        ".cpp": "Source",
        ".cc": "Source",
        ".c": "Source",
        ".md": "Documentation",
        ".sh": "Shell script",
        ".py": "Python script",
        ".json": "JSON config",
        ".jpeg": "Image",
        ".jpg": "Image",
        ".png": "Image",
        ".gif": "Image",
        ".svg": "Image",
        ".txt": "Text",
        ".gitignore": "Git ignore rules",
    }
    if ext in roles:
        return roles[ext]
    if lower == "main.cpp":
        return "Entry point / demo"
    if lower == "compile.sh":
        return "Build script"
    if lower in ("requirements.md", "requirement.md"):
        return "Requirements"
    if lower in ("problem.md", "problem_statement.md"):
        return "Problem statement"
    if lower == "readme.md":
        return "Overview / quick start"
    if "complete" in lower and ext == ".md":
        return "Study notes (complete guide)"
    return "File"


def list_dir(dir_path: Path) -> tuple[list[Path], list[Path]]:
    subdirs: list[Path] = []
    files: list[Path] = []
    if not dir_path.is_dir():
        return subdirs, files
    for entry in sorted(dir_path.iterdir(), key=lambda p: (not p.is_dir(), p.name.lower())):
        if entry.name == OUTPUT_NAME:
            continue
        if entry.name.startswith("."):
            continue
        if entry.is_dir():
            if should_skip_dir(entry.name):
                continue
            subdirs.append(entry)
        else:
            files.append(entry)
    return subdirs, files


def parent_link(dir_path: Path) -> str | None:
    if dir_path == ROOT:
        return None
    return "../" + OUTPUT_NAME


def title_for(dir_path: Path) -> str:
    if dir_path == ROOT:
        return "LLD Repository — File Structure"
    return f"{dir_path.name}/ — File Structure"


def build_tree_line(name: str, is_dir: bool, prefix: str, is_last: bool) -> str:
    branch = "└── " if is_last else "├── "
    suffix = "/" if is_dir else ""
    return f"{prefix}{branch}{name}{suffix}"


def generate_markdown(dir_path: Path) -> str:
    subdirs, files = list_dir(dir_path)
    rel = dir_path.relative_to(ROOT).as_posix() if dir_path != ROOT else "."
    parent = parent_link(dir_path)

    lines: list[str] = []
    lines.append(f"# {title_for(dir_path)}")
    lines.append("")

    if parent:
        lines.append(f"Parent: [{parent}]({parent})")
        lines.append("")

    lines.append("```")
    if dir_path == ROOT:
        lines.append(".")
    else:
        lines.append(f"{dir_path.name}/")
    lines.append(f"├── {OUTPUT_NAME}       # This file")

    all_entries: list[tuple[str, bool]] = [(d.name, True) for d in subdirs] + [(f.name, False) for f in files]
    for i, (name, is_dir) in enumerate(all_entries):
        is_last = i == len(all_entries) - 1
        branch = "└── " if is_last else "├── "
        suffix = "/" if is_dir else ""
        lines.append(f"{branch}{name}{suffix}")

    lines.append("```")
    lines.append("")

    if subdirs:
        lines.append("## Subfolders")
        lines.append("")
        lines.append("| Folder | Structure |")
        lines.append("|--------|-----------|")
        for d in subdirs:
            link = f"./{d.name}/{OUTPUT_NAME}"
            lines.append(f"| `{d.name}/` | [{OUTPUT_NAME}]({link}) |")
        lines.append("")

    if files:
        lines.append("## Files")
        lines.append("")
        lines.append("| File | Role |")
        lines.append("|------|------|")
        for f in files:
            lines.append(f"| `{f.name}` | {file_role(f.name)} |")
        lines.append("")

    if dir_path == ROOT:
        lines.append("## Top-level projects")
        lines.append("")
        lines.append("| Folder | Type |")
        lines.append("|--------|------|")
        for d in subdirs:
            name = d.name
            if name == "scripts":
                kind = "Repo utilities"
            elif name == "assets":
                kind = "Shared assets"
            elif name.startswith("L") and " " in name:
                kind = "Lesson / pattern"
            elif name == "Multi_threading_C++":
                kind = "Concurrency curriculum"
            elif name == "Exception_Handling":
                kind = "C++ exceptions curriculum"
            elif name.endswith("_LLD"):
                kind = "System LLD project"
            else:
                kind = "Project / notes"
            lines.append(f"| [{name}/](./{name}/{OUTPUT_NAME}) | {kind} |")
        lines.append("")
        lines.append("> Regenerate all maps: `python3 scripts/generate_file_structures.py`")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def walk_directories() -> list[Path]:
    result: list[Path] = [ROOT]
    for current, dirnames, _ in os.walk(ROOT):
        current_path = Path(current)
        dirnames[:] = sorted(
            d for d in dirnames if not should_skip_dir(d) and not d.startswith(".")
        )
        for name in dirnames:
            result.append(current_path / name)
    return result


def main() -> None:
    dirs = walk_directories()
    created = 0
    for dir_path in dirs:
        out = dir_path / OUTPUT_NAME
        content = generate_markdown(dir_path)
        out.write_text(content, encoding="utf-8")
        created += 1
    print(f"Wrote {OUTPUT_NAME} in {created} folders under {ROOT}")


if __name__ == "__main__":
    main()
