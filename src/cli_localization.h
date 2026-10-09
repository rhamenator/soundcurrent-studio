// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "cli_catalog_generated.h"
#include <algorithm>
#include <string>
#include <initializer_list>
namespace soundcurrent::cli {
inline std::string language = "en";
inline void selectLanguage(std::string tag) {
    std::replace(tag.begin(), tag.end(), '_', '-');
    const auto lower = [](std::string value) {
        for (auto &c : value) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        return value;
    };
    tag = lower(tag);
    language = "en";
    for (const auto &entry : entries)
        if (lower(std::string(entry.language)) == tag) { language = entry.language; return; }
    const auto dash = tag.find('-');
    if (dash != std::string::npos)
        for (const auto &entry : entries)
            if (lower(std::string(entry.language)) == tag.substr(0, dash)) {
                language = entry.language; return;
            }
}
inline std::string text(std::string_view source) {
    for (const auto &entry : entries)
        if (entry.language == language && entry.source == source) return std::string(entry.text);
    return std::string(source);
}
// Expand placeholders from the translated template in one pass. Argument
// text is copied literally and never scanned for additional placeholders.
inline std::string format(std::string_view source, std::initializer_list<std::string> arguments) {
    const auto pattern = text(source);
    std::string result;
    for (std::size_t i = 0; i < pattern.size();) {
        if (pattern[i] == '%' && i + 1 < pattern.size() && pattern[i + 1] >= '1' && pattern[i + 1] <= '9') {
            std::size_t index = pattern[i + 1] - '0';
            std::size_t width = 2;
            if (i + 2 < pattern.size() && pattern[i + 2] >= '0' && pattern[i + 2] <= '9') {
                index = index * 10 + pattern[i + 2] - '0';
                width = 3;
            }
            if (index <= arguments.size()) {
                result += *(arguments.begin() + index - 1);
                i += width;
                continue;
            }
        }
        result += pattern[i++];
    }
    return result;
}
inline std::string renderError(std::string_view diagnostic) {
    // Display alias for the same no-overwrite condition as the desktop.
    // The underlying exception and filesystem behavior remain invariant.
    if (diagnostic == "Output already exists; choose a new filename")
        diagnostic = "Output already exists; select a new filename";
    if (diagnostic == "Too many EQ bands for one channel")
        diagnostic = "Too many Studio channel filters";
    auto localized = text(diagnostic);
    constexpr std::string_view unknownPrefix = "Unknown option: ";
    if (diagnostic.starts_with(unknownPrefix)) {
        localized = text("Unknown option: %1");
        const auto placeholder = localized.find("%1");
        if (placeholder != std::string::npos)
            localized.replace(placeholder, 2, diagnostic.substr(unknownPrefix.size()));
    }
    constexpr std::string_view publishPrefix = "Cannot publish output: ";
    constexpr std::string_view publishSuffix = "; choose a new name on a filesystem supporting hard links";
    if (diagnostic.starts_with(publishPrefix) && diagnostic.ends_with(publishSuffix)) {
        localized = text("Cannot publish output: %1; choose a new name on a filesystem supporting hard links");
        const auto placeholder = localized.find("%1");
        if (placeholder != std::string::npos)
            localized.replace(placeholder, 2, diagnostic.substr(publishPrefix.size(),
                diagnostic.size() - publishPrefix.size() - publishSuffix.size()));
    }
    auto result = text("Render: %1");
    const auto position = result.find("%1");
    if (position != std::string::npos) result.replace(position, 2, localized);
    return result;
}
}
