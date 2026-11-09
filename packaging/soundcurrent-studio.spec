Name:           soundcurrent-studio
%global debug_package %{nil}
Version:        1.3.5
Release:        1%{?dist}
Summary:        Adjustable desktop equalizer for PipeWire
License:        GPL-3.0-only
URL:            https://github.com/rhamenator/soundcurrent-studio
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  python3
BuildRequires:  cmake >= 3.20
BuildRequires:  gcc-c++
BuildRequires:  pipewire-devel
BuildRequires:  pkgconf-pkg-config
BuildRequires:  qt6-qtbase-devel >= 6.4
Requires:       pipewire-pulseaudio
Requires:       pipewire-utils
Requires:       pulseaudio-utils
Requires:       wireplumber

%description
SoundCurrent Studio is a C++ desktop equalizer with adjustable bands, listening
presets, output device selection, and background tray controls for PipeWire.

%prep
%setup -q

%build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_INSTALL_LIBDIR=%{_libdir}
cmake --build build --parallel %{_smp_build_ncpus}

%check
ctest --test-dir build --output-on-failure --no-tests=error

%install
DESTDIR=%{buildroot} cmake --install build

%files
%license LICENSE
%doc README.md
%{_bindir}/soundcurrent-studio
%{_bindir}/soundcurrent-studio-render
%{_libdir}/libsoundcurrent-dsp.a
%{_libdir}/libsoundcurrent-engine.a
%{_libdir}/libsoundcurrent-wave.a
%{_libdir}/cmake/SoundCurrentEngine
%{_includedir}/soundcurrent
%{_datadir}/applications/io.github.rhamenator.SoundCurrentStudio.desktop
%{_datadir}/icons/hicolor/scalable/apps/io.github.rhamenator.SoundCurrentStudio.svg
%{_datadir}/doc/soundcurrent-studio/copyright
%{_datadir}/doc/soundcurrent-studio/LICENSE
%{_datadir}/doc/soundcurrent-studio/THIRD-PARTY-NOTICES.md
%{_datadir}/doc/soundcurrent-studio/studio-engine.md
%{_datadir}/doc/soundcurrent-studio/using-studio.md
%{_datadir}/doc/soundcurrent-studio/metering-dynamics-and-balance.md
%{_datadir}/doc/soundcurrent-studio/speakers
%{_datadir}/doc/soundcurrent-studio/equipment
%{_datadir}/doc/soundcurrent-studio/equipment-profiles.md
%{_datadir}/doc/soundcurrent-studio/audio-buffering.md
%{_datadir}/doc/soundcurrent-studio/analysis-and-microphone-profiles.md
%{_datadir}/doc/soundcurrent-studio/AudioSpectrogram-LICENSE

%changelog
* Mon Nov 09 2026 rhamenator <rhamenator@gmail.com> - 1.3.5-1
- Improve global controls, setup commands, responsive layout, and analysis views.

* Sun Nov 08 2026 rhamenator <rhamenator@gmail.com> - 1.3.4-1
- Improve responsive UI layouts and clarify live/offline Studio guidance

* Sun Nov 08 2026 rhamenator <rhamenator@gmail.com> - 1.3.3-1
- Avoid repeating Offline editing guidance in the Studio status area

* Sun Nov 08 2026 rhamenator <rhamenator@gmail.com> - 1.3.2-1
- Make Studio live/offline mode control Apply visibility and enlarge its buttons

* Sun Nov 08 2026 rhamenator <rhamenator@gmail.com> - 1.3.1-1
- Apply VB-CABLE Windows volume and mute control live during install and repair

* Fri Nov 06 2026 rhamenator <rhamenator@gmail.com> - 1.3.0-1
- Add inactive-tab analysis views, estimated profile review, readable scale-aware axes and startup updates

* Thu Nov 05 2026 rhamenator <rhamenator@gmail.com> - 1.2.1-1
- Restore listening presets, embed tray logo pixels, and allow DAW routes.

* Thu Nov 05 2026 rhamenator <rhamenator@gmail.com> - 1.2.0-1
- Graphical 32-point equipment profile editing, import deduplication and safe deletion
- Adjustable audio buffering and accessible persistent profile controls

* Thu Nov 05 2026 rhamenator <rhamenator@gmail.com> - 1.1.3-1
- Add measured equipment corrections and immediate profile selection
- Include profile compiler source in RPM build archives
* Mon Oct 05 2026 rhamenator <rhamenator@gmail.com> - 0.8.4-1
- Add listening enhancements and controls, with advanced Studio parameters

* Mon Oct 05 2026 rhamenator <rhamenator@gmail.com> - 0.7.0-1
- Add measured speaker profiles and amplifier imports with source attribution
- Hide disconnected microphone jacks and reject clipped calibration capture
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.5.1-1
- Add persistent EQ lock, grouped Undo, safe wheel scrolling, and mic disconnect status
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.5.0-1
- Add speaker and room measurement with 20 Hz to 25 kHz sweep and EQ preview
- Fit the desktop window to the available display area
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.4.0-1
- Add automatic microphone routing, natural voice EQ, and editable input controls
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.7-1
- Apply EQ controls immediately and avoid a second system volume reduction
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.6-1
- Add post-gain slider, stereo balance, and overall output level indicator
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.5-1
- Allow precise 1-100 ms level refresh with faster audio monitor chunks
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.4-1
- Add adjustable live level refresh and optional peak hold markers
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.3-1
- Add output gain, live level indicators, Loudness preset, and GPLv3 licensing
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.2-1
- Make the preset list scrollable, clarify group separators, and default to Flat
* Sun Oct 04 2026 rhamenator <rhamenator@gmail.com> - 0.3.1-1
- Add Fedora and RHEL-compatible RPM packaging
