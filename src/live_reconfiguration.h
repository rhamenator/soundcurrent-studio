// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <type_traits>
#include <utility>
namespace soundcurrent {
// Control-thread operation shared by the Linux and Windows route adapters.
// Preflight and copies finish before stopping playback. Callers capture stable
// device/profile values for start(); stop() restores normal output ownership.
// Failure restores the old session and attempts to start it once.
template<class Session,class Preflight,class Stop,class Start>
void replaceLiveSession(Session &live,const Session &proposed,Preflight preflight,Stop stop,Start start) {
    static_assert(std::is_nothrow_move_assignable_v<Session>);
    preflight();
    auto previous=live;
    auto next=proposed;
    try{stop();live=std::move(next);start();}catch(...){
        live=std::move(previous);
        try{start();}catch(...){} // Preserve the original error for the UI.
        throw;
    }
}
}
