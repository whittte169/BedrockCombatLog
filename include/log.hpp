#pragma once

#include <fstream>
#include <string>
#include <exception>
#include <optional>
#include <mutex>
#include <filesystem>

inline std::optional<std::string> log(const std::string &logStr, const std::filesystem::path &path) {
    if (logStr.empty()) return "Log message is empty!";
    if (path.empty()) return "Path is empty!";
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    try {
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) return "File wasn't opened!";
        file << logStr << '\n';
        if (!file) return "Write error!";
        return std::nullopt;
    } catch (const std::exception &e) {
        return e.what();
    }
}