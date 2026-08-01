#ifndef FILE_MANAGER_LLD_CORE_FILEMANAGERSYSTEM_H
#define FILE_MANAGER_LLD_CORE_FILEMANAGERSYSTEM_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../enums/NodeType.h"
#include "../models/DirectoryNode.h"
#include "../models/FileNode.h"
#include "../services/CopyService.h"
#include "../services/PathResolver.h"
#include "../services/SearchService.h"

namespace file_manager {

class FileManagerSystem {
public:
    FileManagerSystem() {
        root_ = std::make_unique<DirectoryNode>("");
        current_ = root_.get();
    }

    std::string pwd() const { return PathResolver::join(getCurrentTokens()); }

    void mkdir(const std::string &path) {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        if (tokens.empty()) {
            throw std::runtime_error("Cannot create root again");
        }

        DirectoryNode *parent = navigateToDirectory(tokens, tokens.size() - 1, true);
        const std::string &dirName = tokens.back();
        if (parent->hasChild(dirName)) {
            throw std::runtime_error("Directory already exists: " + dirName);
        }
        parent->addDirectory(std::make_unique<DirectoryNode>(dirName, parent));
        std::cout << "Created directory: " << PathResolver::join(tokens) << "\n";
    }

    void createFile(const std::string &path, const std::string &content) {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        if (tokens.empty()) {
            throw std::runtime_error("Cannot create file at root without name");
        }

        DirectoryNode *parent = navigateToDirectory(tokens, tokens.size() - 1, true);
        const std::string &fileName = tokens.back();
        if (parent->hasChild(fileName)) {
            throw std::runtime_error("File already exists: " + fileName);
        }
        parent->addFile(std::make_unique<FileNode>(fileName, content));
        std::cout << "Created file: " << PathResolver::join(tokens) << "\n";
    }

    void cd(const std::string &path) {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        current_ = navigateToDirectory(tokens, tokens.size(), false);
        std::cout << "Current directory: " << pwd() << "\n";
    }

    std::vector<std::string> ls(const std::string &path = "") const {
        DirectoryNode *target = current_;
        if (!path.empty()) {
            const std::vector<std::string> tokens = resolvePathTokens(path);
            target = navigateToDirectory(tokens, tokens.size(), false);
        }

        std::vector<std::string> entries;
        for (const auto &entry : target->getChildren()) {
            const IFileSystemNode *node = entry.second.get();
            entries.push_back(node->getName() + (node->getType() == NodeType::DIRECTORY ? "/" : ""));
        }
        return entries;
    }

    void printLs(const std::string &path = "") const {
        const std::vector<std::string> entries = ls(path);
        std::string listingPath = pwd();
        if (!path.empty()) {
            listingPath = PathResolver::join(resolvePathTokens(path));
        }
        std::cout << "Listing " << listingPath << ":\n";
        for (const std::string &entry : entries) {
            std::cout << "  " << entry << "\n";
        }
        if (entries.empty()) {
            std::cout << "  (empty)\n";
        }
    }

    std::string cat(const std::string &path) const {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        if (tokens.empty()) {
            throw std::runtime_error("Invalid file path");
        }

        DirectoryNode *parent = navigateToDirectory(tokens, tokens.size() - 1, false);
        FileNode *file = parent->getFile(tokens.back());
        if (file == nullptr) {
            throw std::runtime_error("File not found: " + path);
        }
        return file->getContent();
    }

    void printCat(const std::string &path) const {
        std::cout << cat(path) << "\n";
    }

    void rm(const std::string &path) {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        if (tokens.empty()) {
            throw std::runtime_error("Cannot remove root");
        }

        DirectoryNode *parent = navigateToDirectory(tokens, tokens.size() - 1, false);
        const std::string &entryName = tokens.back();
        parent->removeChild(entryName);
        std::cout << "Removed: " << PathResolver::join(tokens) << "\n";
    }

    void mv(const std::string &sourcePath, const std::string &destPath) {
        const std::vector<std::string> srcTokens = resolvePathTokens(sourcePath);
        const std::vector<std::string> destTokens = resolvePathTokens(destPath);
        if (srcTokens.empty() || destTokens.empty()) {
            throw std::runtime_error("Invalid mv paths");
        }

        DirectoryNode *srcParent = navigateToDirectory(srcTokens, srcTokens.size() - 1, false);
        const std::string &srcName = srcTokens.back();
        std::unique_ptr<IFileSystemNode> node = srcParent->removeChild(srcName);

        DirectoryNode *destParent = navigateToDirectory(destTokens, destTokens.size() - 1, true);
        const std::string &destName = destTokens.back();

        if (destParent->hasChild(destName)) {
            attachWithName(srcParent, srcName, std::move(node));
            throw std::runtime_error("Destination already exists: " + destName);
        }

        attachWithName(destParent, destName, std::move(node));
        std::cout << "Moved " << PathResolver::join(srcTokens) << " -> " << PathResolver::join(destTokens) << "\n";
    }

    void cp(const std::string &sourcePath, const std::string &destPath) {
        IFileSystemNode *sourceNode = getNodeAtPath(sourcePath);
        const std::vector<std::string> destTokens = resolvePathTokens(destPath);
        if (destTokens.empty()) {
            throw std::runtime_error("Invalid destination path");
        }

        DirectoryNode *destParent = navigateToDirectory(destTokens, destTokens.size() - 1, true);
        const std::string &destName = destTokens.back();
        if (destParent->hasChild(destName)) {
            throw std::runtime_error("Destination already exists: " + destName);
        }

        std::unique_ptr<IFileSystemNode> copied = copyService_.deepCopy(sourceNode);
        attachWithName(destParent, destName, std::move(copied));
        std::cout << "Copied " << sourcePath << " -> " << PathResolver::join(destTokens) << "\n";
    }

    std::size_t getSize(const std::string &path) const {
        IFileSystemNode *node = getNodeAtPath(path);
        return node->getSize();
    }

    std::vector<std::string> find(const std::string &name) const {
        std::vector<std::string> results;
        searchService_.findByName(root_.get(), {}, name, results);
        return results;
    }

    void printFind(const std::string &name) const {
        const std::vector<std::string> results = find(name);
        std::cout << "Find '" << name << "':\n";
        for (const std::string &path : results) {
            std::cout << "  " << path << "\n";
        }
        if (results.empty()) {
            std::cout << "  (no matches)\n";
        }
    }

private:
    std::unique_ptr<DirectoryNode> root_;
    DirectoryNode *current_;
    CopyService copyService_;
    SearchService searchService_;

    std::vector<std::string> getCurrentTokens() const {
        std::vector<std::string> tokens;
        const DirectoryNode *node = current_;
        while (node != nullptr && node != root_.get()) {
            tokens.push_back(node->getName());
            node = node->getParent();
        }
        std::reverse(tokens.begin(), tokens.end());
        return tokens;
    }

    std::vector<std::string> resolvePathTokens(const std::string &path) const {
        return PathResolver::resolveRelative(getCurrentTokens(), path);
    }

    DirectoryNode *navigateToDirectory(const std::vector<std::string> &tokens, std::size_t depth,
                                         bool createMissing) const {
        DirectoryNode *node = root_.get();
        for (std::size_t i = 0; i < depth; ++i) {
            const std::string &part = tokens[i];
            DirectoryNode *child = node->getDirectory(part);
            if (child == nullptr) {
                if (!createMissing) {
                    throw std::runtime_error("Directory not found: " + part);
                }
                auto created = std::make_unique<DirectoryNode>(part, node);
                DirectoryNode *createdPtr = created.get();
                node->addDirectory(std::move(created));
                child = createdPtr;
            }
            node = child;
        }
        return node;
    }

    IFileSystemNode *getNodeAtPath(const std::string &path) const {
        const std::vector<std::string> tokens = resolvePathTokens(path);
        if (tokens.empty()) {
            return root_.get();
        }

        DirectoryNode *parent = navigateToDirectory(tokens, tokens.size() - 1, false);
        IFileSystemNode *node = parent->getChild(tokens.back());
        if (node == nullptr) {
            throw std::runtime_error("Path not found: " + path);
        }
        return node;
    }

    static void attachWithName(DirectoryNode *parent, const std::string &name,
                               std::unique_ptr<IFileSystemNode> node) {
        if (node->getType() == NodeType::FILE) {
            const FileNode *file = static_cast<const FileNode *>(node.get());
            parent->addFile(std::make_unique<FileNode>(name, file->getContent()));
            return;
        }

        auto directory = std::unique_ptr<DirectoryNode>(static_cast<DirectoryNode *>(node.release()));
        if (directory->getName() != name) {
            throw std::runtime_error("Directory leaf name must match on attach (rename dirs via extension)");
        }
        directory->setParent(parent);
        parent->addDirectory(std::move(directory));
    }
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_CORE_FILEMANAGERSYSTEM_H
