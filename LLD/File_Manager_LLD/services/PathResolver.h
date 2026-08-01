#ifndef FILE_MANAGER_LLD_SERVICES_PATHRESOLVER_H
#define FILE_MANAGER_LLD_SERVICES_PATHRESOLVER_H

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace file_manager {

class PathResolver {
public:
    static std::vector<std::string> tokenize(const std::string &path) {
        std::vector<std::string> tokens;
        std::stringstream stream(path);
        std::string part;
        while (std::getline(stream, part, '/')) {
            if (!part.empty() && part != ".") {
                tokens.push_back(part);
            }
        }
        return tokens;
    }

    static std::vector<std::string> normalizeTokens(std::vector<std::string> tokens) {
        std::vector<std::string> normalized;
        for (const std::string &token : tokens) {
            if (token == "..") {
                if (normalized.empty()) {
                    throw std::runtime_error("Cannot go above root");
                }
                normalized.pop_back();
            } else {
                normalized.push_back(token);
            }
        }
        return normalized;
    }

    static std::string join(const std::vector<std::string> &tokens) {
        if (tokens.empty()) {
            return "/";
        }
        std::string path = "/";
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            path += tokens[i];
            if (i + 1 < tokens.size()) {
                path += "/";
            }
        }
        return path;
    }

    static std::vector<std::string> resolveRelative(const std::vector<std::string> &cwdTokens,
                                                    const std::string &path) {
        std::vector<std::string> tokens = tokenize(path);
        if (!path.empty() && path.front() == '/') {
            return normalizeTokens(tokens);
        }
        std::vector<std::string> merged = cwdTokens;
        merged.insert(merged.end(), tokens.begin(), tokens.end());
        return normalizeTokens(merged);
    }
};

} // namespace file_manager

#endif // FILE_MANAGER_LLD_SERVICES_PATHRESOLVER_H
