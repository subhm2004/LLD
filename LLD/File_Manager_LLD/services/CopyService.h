#ifndef FILE_MANAGER_LLD_SERVICES_COPYSERVICE_H
#define FILE_MANAGER_LLD_SERVICES_COPYSERVICE_H

#include <memory>

#include "../models/DirectoryNode.h"
#include "../models/FileNode.h"

namespace file_manager {

class CopyService {
public:
    std::unique_ptr<IFileSystemNode> deepCopy(const IFileSystemNode *source) const {
        if (source->getType() == NodeType::FILE) {
            const FileNode *file = static_cast<const FileNode *>(source);
            return std::unique_ptr<IFileSystemNode>(
                std::make_unique<FileNode>(file->getName(), file->getContent()).release());
        }

        const DirectoryNode *directory = static_cast<const DirectoryNode *>(source);
        auto copiedDirectory = std::make_unique<DirectoryNode>(directory->getName());

        for (const auto &entry : directory->getChildren()) {
            std::unique_ptr<IFileSystemNode> childCopy = deepCopy(entry.second.get());
            if (childCopy->getType() == NodeType::FILE) {
                auto fileCopy = std::unique_ptr<FileNode>(static_cast<FileNode *>(childCopy.release()));
                copiedDirectory->addFile(std::move(fileCopy));
            } else {
                auto dirCopy = std::unique_ptr<DirectoryNode>(static_cast<DirectoryNode *>(childCopy.release()));
                copiedDirectory->addDirectory(std::move(dirCopy));
            }
        }

        return std::unique_ptr<IFileSystemNode>(copiedDirectory.release());
    }
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_SERVICES_COPYSERVICE_H
