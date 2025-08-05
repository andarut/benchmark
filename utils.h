#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <cstring>
#include <random>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstddef>
#include <functional>

typedef std::vector<unsigned char> blob;

inline blob randomData(size_t sizeInBytes) {
    blob data(sizeInBytes);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (std::size_t i = 0; i < sizeInBytes; ++i) {
        data[i] = static_cast<unsigned char>(dis(gen));
    }

    return data;
}

inline blob readAllFilesRecursively(const std::filesystem::path& rootPath) {
    blob result;

    if (!std::filesystem::exists(rootPath) || !std::filesystem::is_directory(rootPath)) {
        std::cerr << "Invalid directory: " << rootPath << std::endl;
        return result;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) {
                std::cerr << "Failed to open file: " << entry.path() << std::endl;
                continue;
            }

            blob buffer(std::istreambuf_iterator<char>(file), {});
            result.insert(result.end(), buffer.begin(), buffer.end());
        }
    }

    return result;
}

inline blob readFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Invalid file: " << path << std::endl;
        return {};
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return {};
    }

    blob result(std::istreambuf_iterator<char>(file), {});
    return result;
}

#endif // UTILS_H