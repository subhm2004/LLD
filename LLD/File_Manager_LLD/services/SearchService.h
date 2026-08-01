#ifndef FILE_MANAGER_LLD_SERVICES_SEARCHSERVICE_H
#define FILE_MANAGER_LLD_SERVICES_SEARCHSERVICE_H

#include <string>
#include <vector>

#include "../models/DirectoryNode.h"
#include "PathResolver.h"

namespace file_manager {

class SearchService {
public:
    void findByName(DirectoryNode *root, const std::vector<std::string> &baseTokens,
                    const std::string &queryName, std::vector<std::string> &results) const {
        for (const auto &entry : root->getChildren()) {
            const IFileSystemNode *node = entry.second.get();
            std::vector<std::string> childTokens = baseTokens;
            childTokens.push_back(node->getName());
            const std::string childPath = PathResolver::join(childTokens);

            if (node->getName() == queryName) {
                results.push_back(childPath);
            }

            if (node->getType() == NodeType::DIRECTORY) {
                findByName(static_cast<DirectoryNode *>(const_cast<IFileSystemNode *>(node)), childTokens,
                           queryName, results);
            }
        }
    }
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_SERVICES_SEARCHSERVICE_H
