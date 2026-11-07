// SPDX-License-Identifier: GPL-3.0-only
#include "../src/windows_endpoint_identity.h"
#include <cstdlib>
#include <iostream>
using namespace soundcurrent;
static void check(bool ok) { if (!ok) std::abort(); }
int main() {
    check(managedEndpointKind(L"SoundCurrent Audio") == ManagedEndpointKind::Playback);
    check(managedEndpointKind(L"SoundCurrent Microphone Feed") == ManagedEndpointKind::MicrophoneFeed);
    check(managedEndpointKind(L"SoundCurrent Microphone") == ManagedEndpointKind::MicrophoneCapture);
    for(auto name : {L"",L"USB Audio",L"VB-Audio Virtual Cable",L"SoundCurrent Audio extra",L"SoundCurrent Microphone Feed extra"})
        check(managedEndpointKind(name) == ManagedEndpointKind::Other);
    std::cout << "Managed playback and microphone identities remain distinct\n";
    check(managedTopologyKind(L"SoundCurrent Audio", L"\\\\?\\root#media#0002\\TopologySoundCurrent", false) == ManagedEndpointKind::Playback);
    check(managedTopologyKind(L"SoundCurrent Audio", L"\\\\?\\root#media#0002\\topologysoundcurrentmicfeed", false) == ManagedEndpointKind::MicrophoneFeed);
    check(managedTopologyKind(L"SoundCurrent Audio", L"\\\\?\\root#media#0002\\TopologySoundCurrentMic", true) == ManagedEndpointKind::MicrophoneCapture);
    check(managedTopologyKind(L"SoundCurrent Audio", L"\\TopologySoundCurrentMic", false) == ManagedEndpointKind::Other);
    check(managedTopologyKind(L"SoundCurrent Audio", L"\\TopologySoundCurrentMicFeedExtra", false) == ManagedEndpointKind::Other);
    check(managedTopologyKind(L"USB Audio", L"\\TopologySoundCurrentMic", true) == ManagedEndpointKind::Other);
    check(managedTopologyKind(L"SoundCurrent Audio", L"", false) == ManagedEndpointKind::Other);
}
