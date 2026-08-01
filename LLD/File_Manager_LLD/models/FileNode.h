#ifndef FILE_MANAGER_LLD_MODELS_FILENODE_H
#define FILE_MANAGER_LLD_MODELS_FILENODE_H

#include <string>
#include <utility>

#include "IFileSystemNode.h"

namespace file_manager {

class FileNode : public IFileSystemNode {
public:
    FileNode(std::string name, std::string content)
        : name_(std::move(name)), content_(std::move(content)) {}

    NodeType getType() const override { return NodeType::FILE; }
    const std::string &getName() const override { return name_; }
    std::size_t getSize() const override { return content_.size(); }

    const std::string &getContent() const { return content_; }
    void setContent(std::string content) { content_ = std::move(content); }

private:
    std::string name_;
    std::string content_;
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_MODELS_FILENODE_H
