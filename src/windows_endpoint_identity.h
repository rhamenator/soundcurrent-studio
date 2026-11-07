// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <string_view>
namespace soundcurrent {
enum class ManagedEndpointKind { Other, Playback, MicrophoneFeed, MicrophoneCapture };
inline ManagedEndpointKind managedEndpointKind(std::wstring_view interfaceName) {
    if (interfaceName == L"SoundCurrent Audio") return ManagedEndpointKind::Playback;
    if (interfaceName == L"SoundCurrent Microphone Feed") return ManagedEndpointKind::MicrophoneFeed;
    if (interfaceName == L"SoundCurrent Microphone") return ManagedEndpointKind::MicrophoneCapture;
    return ManagedEndpointKind::Other;
}
// Core Audio's interface friendly name describes the adapter, so native
// endpoints on the same adapter can all report "SoundCurrent Audio". Resolve
// the role from the connected KS filter's driver-owned reference string.
inline ManagedEndpointKind managedTopologyKind(std::wstring_view interfaceName,
                                               std::wstring_view topologyId,
                                               bool capture) {
    if (managedEndpointKind(interfaceName) == ManagedEndpointKind::Other)
        return ManagedEndpointKind::Other;
    const auto end = topologyId.find_last_of(L'\\');
    if (end == std::wstring_view::npos) return ManagedEndpointKind::Other;
    auto reference = topologyId.substr(end + 1);
    auto equals = [&](std::wstring_view expected) {
        if (reference.size() != expected.size()) return false;
        for (std::size_t i = 0; i < reference.size(); ++i) {
            auto c = reference[i];
            if (c >= L'A' && c <= L'Z') c += L'a' - L'A';
            if (c != expected[i]) return false;
        }
        return true;
    };
    if (capture && equals(L"topologysoundcurrentmic")) return ManagedEndpointKind::MicrophoneCapture;
    if (!capture && equals(L"topologysoundcurrentmicfeed")) return ManagedEndpointKind::MicrophoneFeed;
    if (!capture && equals(L"topologysoundcurrent")) return ManagedEndpointKind::Playback;
    return ManagedEndpointKind::Other;
}
} // namespace soundcurrent
