// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "cli_catalog_generated.h"
#include <algorithm>
#include <string>
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
inline std::string renderError(std::string_view diagnostic) {
    // Display alias for the same no-overwrite condition as the desktop.
    // The underlying exception and filesystem behavior remain invariant.
    if (diagnostic == "Output already exists; choose a new filename")
        diagnostic = "Output already exists; select a new filename";
    auto localized = text(diagnostic);
    constexpr std::string_view unknownPrefix = "Unknown option: ";
    if (diagnostic.starts_with(unknownPrefix)) {
        localized = text("Unknown option: %1");
        const auto placeholder = localized.find("%1");
        if (placeholder != std::string::npos)
            localized.replace(placeholder, 2, diagnostic.substr(unknownPrefix.size()));
    }
    auto result = text("Render: %1");
    const auto position = result.find("%1");
    if (position != std::string::npos) result.replace(position, 2, localized);
    return result;
}
}
