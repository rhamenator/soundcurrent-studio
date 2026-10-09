// SPDX-License-Identifier: GPL-3.0-only
#include "cli_localization.h"
#include <iostream>
#include <set>

int main() {
    using namespace soundcurrent::cli;
    std::set<std::string> languages;
    for (const auto &entry : entries) languages.emplace(entry.language);
    const std::string detail = "filesystem detail: %1 %2; 音声";
    const std::string source = "Cannot publish output: %1; choose a new name on a filesystem supporting hard links";
    const std::string diagnostic = "Cannot publish output: " + detail +
        "; choose a new name on a filesystem supporting hard links";
    for (const auto &tag : languages) {
        selectLanguage(tag);
        const auto translated = std::find_if(std::begin(entries), std::end(entries),
            [&](const Entry &entry) { return entry.language == tag && entry.source == source; });
        const auto wrapper = std::find_if(std::begin(entries), std::end(entries),
            [&](const Entry &entry) { return entry.language == tag && entry.source == "Render: %1"; });
        if (translated == std::end(entries) || wrapper == std::end(entries)) return 1;
        std::string expected(translated->text);
        expected.replace(expected.find("%1"), 2, detail);
        std::string wrapped(wrapper->text);
        wrapped.replace(wrapped.find("%1"), 2, expected);
        if (renderError(diagnostic) != wrapped || wrapped.find(detail) == std::string::npos) {
            std::cerr << "Publication diagnostic mismatch: " << tag << '\n';
            return 1;
        }
        // A partial lookalike must retain its original external diagnostic text.
        const std::string external = "Cannot publish output: unrelated filesystem exception";
        wrapped = wrapper->text;
        wrapped.replace(wrapped.find("%1"), 2, external);
        if (renderError(external) != wrapped) return 1;
    }
    std::cout << "PASS: 34 publication templates preserve external details and literal placeholders\n";
}
