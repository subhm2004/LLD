#ifndef FILE_MANAGER_LLD_MODELS_DIRECTORYNODE_H
#define FILE_MANAGER_LLD_MODELS_DIRECTORYNODE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "FileNode.h"
#include "IFileSystemNode.h"

namespace file_manager {

class DirectoryNode : public IFileSystemNode {
public:
    explicit DirectoryNode(std::string name, DirectoryNode *parent = nullptr)
        : name_(std::move(name)), parent_(parent) {}

    NodeType getType() const override { return NodeType::DIRECTORY; }
    const std::string &getName() const override { return name_; }

    std::size_t getSize() const override {
        std::size_t total = 0;
        for (const auto &entry : children_) {
            total += entry.second->getSize();
        }
        return total;
    }

    DirectoryNode *getParent() const { return parent_; }
    void setParent(DirectoryNode *parent) { parent_ = parent; }

    bool hasChild(const std::string &childName) const {
        return children_.count(childName) > 0;
    }

    IFileSystemNode *getChild(const std::string &childName) const {
        auto it = children_.find(childName);
        if (it == children_.end()) {
            return nullptr;
        }
        return it->second.get();
    }

    FileNode *getFile(const std::string &childName) const {
        IFileSystemNode *node = getChild(childName);
        if (node == nullptr || node->getType() != NodeType::FILE) {
            return nullptr;
        }
        return static_cast<FileNode *>(node);
    }

    DirectoryNode *getDirectory(const std::string &childName) const {
        IFileSystemNode *node = getChild(childName);
        if (node == nullptr || node->getType() != NodeType::DIRECTORY) {
            return nullptr;
        }
        return static_cast<DirectoryNode *>(node);
    }

    void addFile(std::unique_ptr<FileNode> file) {
        const std::string &childName = file->getName();
        if (hasChild(childName)) {
            throw std::runtime_error("Entry already exists: " + childName);
        }
        children_.emplace(childName, std::move(file));
    }

    void addDirectory(std::unique_ptr<DirectoryNode> directory) {
        const std::string &childName = directory->getName();
        if (hasChild(childName)) {
            throw std::runtime_error("Entry already exists: " + childName);
        }
        directory->setParent(this);
        children_.emplace(childName, std::move(directory));
    }

    std::unique_ptr<IFileSystemNode> removeChild(const std::string &childName) {
        auto it = children_.find(childName);
        if (it == children_.end()) {
            throw std::runtime_error("Entry not found: " + childName);
        }
        std::unique_ptr<IFileSystemNode> removed = std::move(it->second);
        children_.erase(it);
        return removed;
    }

    const std::unordered_map<std::string, std::unique_ptr<IFileSystemNode>> &getChildren() const {
        return children_;
    }

private:
    std::string name_;
    DirectoryNode *parent_;
    std::unordered_map<std::string, std::unique_ptr<IFileSystemNode>> children_;
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_MODELS_DIRECTORYNODE_H
