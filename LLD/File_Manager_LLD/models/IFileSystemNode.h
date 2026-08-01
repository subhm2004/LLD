#ifndef FILE_MANAGER_LLD_MODELS_IFILESYSTEMNODE_H
#define FILE_MANAGER_LLD_MODELS_IFILESYSTEMNODE_H

#include <cstddef>
#include <string>

#include "../enums/NodeType.h"

namespace file_manager {

class IFileSystemNode {
public:
    virtual ~IFileSystemNode() = default;

    virtual NodeType getType() const = 0;
    virtual const std::string &getName() const = 0;
    virtual std::size_t getSize() const = 0;
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_MODELS_IFILESYSTEMNODE_H
